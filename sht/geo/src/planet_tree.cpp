#include "planet_tree.h"

#include <assert.h>

namespace sht {
	namespace geo {

		QuadTreeNode::QuadTreeNode(PlanetCube * cube)
			: mCube(cube),
			, mRenderable(0)
			, mMapTile(0)
			, mParent(0)
			, mParentSlot(-1)
			, mFace(0)
			, mX(0)
			, mY(0)
			, mLOD(0)
			, mRequestMapTile(false)
			, mRequestRenderable(false)
			, mRequestSplit(false)
			, mRequestMerge(false)
			, mPageOut(false)
			, mHasChildren(false)
		{
			mLastOpened = mLastRendered = mCube->getFrameCounter();

			for (int i = 0; i < 4; ++i)
				mChildren[i] = nullptr;
		}
		QuadTreeNode::~QuadTreeNode()
		{
			mCube->Unrequest(this);
			if (mParent)
				mParent->mChildren[mParentSlot] = nullptr;
			DestroyMapTile();
			DestroyRenderable();
			for (int i = 0; i < 4; ++i)
				DetachChild(i);
		}
		void QuadTreeNode::AttachChild(QuadTreeNode* child, int position)
		{
			mChildren[position] = child;
			child->mParent = this;
			child->mParentSlot = position;

			child->mFace = mFace;
			child->mLOD = mLOD + 1;
			child->mX = mX * 2 + (position % 2);
			child->mY = mY * 2 + (position / 2);

			mHasChildren = true;
		}
		void QuadTreeNode::DetachChild(int position)
		{
			if (mChildren[position])
			{
				delete mChildren[position];
				mChildren[position] = nullptr;

				mHasChildren = mChildren[0] || mChildren[1] || mChildren[2] || mChildren[3];
			}
		}
		bool QuadTreeNode::IsSplit()
		{
			return mChildren[0] || mChildren[1] || mChildren[2] || mChildren[3];
		}
		void QuadTreeNode::PropagateLodDistances()
		{
			if (mRenderable)
			{
				double childDistance = 0.0;
				// Get maximum LOD distance of all children.
				for (int i = 0; i < 4; ++i)
				{
					if (mChildren[i] && mChildren[i]->mRenderable)
					{
						// Increase LOD distance w/ centroid distances, to ensure proper LOD nesting.
						childDistance = maxf(childDistance,
							mChildren[i]->mRenderable->getLODDistance() +
							0 * (mChildren[i]->mRenderable->getCenter() - mRenderable->getCenter()).length());
					}
				}
				// Store in renderable.
				mRenderable->setChildLODDistance(childDistance);
			}
			// Propagate changes to parent.
			if (mParent)
				mParent->PropagateLodDistances();
		}
		const double QuadTreeNode::getPriority() const
		{
			if (!mRenderable)
			{
				if (mParent)
					return mParent->getPriority();
				else
					return 0.0;
			}
			return mRenderable->getLODPriority();
		}
		bool QuadTreeNode::PrepareMapTile(PlanetMap* map)
		{
			map->PrepareTile(this);
		}
		void QuadTreeNode::CreateMapTile(PlanetMap* map)
		{
			assert(!mMapTile);
			mMapTile = map->FinalizeTile(this);
		}
		void QuadTreeNode::DestroyMapTile()
		{
			if (mMapTile)
			{
				delete mMapTile;
				mMapTile = nullptr;
			}
		}
		void QuadTreeNode::CreateRenderable(PlanetMapTile* map)
		{
			assert(!mRenderable);
			if (mPageOut)
				mPageOut = false;
			mRenderable = new PlanetRenderable(this, map);
			PropagateLodDistances();
		}
		void QuadTreeNode::DestroyRenderable()
		{
			if (mRenderable)
			{
				delete mRenderable;
				mRenderable = nullptr;
			}
			PropagateLodDistances();
		}
		bool QuadTreeNode::WillRender()
		{
			// Being asked to render ourselves.
			if (!mRenderable)
			{
				mLastOpened = mLastRendered = mCube->getFrameCounter();

				if (mPageOut && mHasChildren)
					return true;

				if (!mRequestRenderable)
				{
					mRequestRenderable = true;
					mCube->Request(this, PlanetCube::REQUEST_RENDERABLE);
				}
				return false;
			}
			return true;
		}
		int QuadTreeNode::Render(RenderQueue* queue, PlanetLODConfiguration& lod)
		{
			// Determine if this node's children are render-ready.
			bool willRenderChildren = true;
			for (int i = 0; i < 4; ++i)
			{
				// Note: intentionally call willRender on /all/ children, not just until one fails,
				// to ensure all 4 children are queued in immediately.
				if (!mChildren[i] || !mChildren[i]->WillRender())
					willRenderChildren = false;
			}

			// If node is paged out, always recurse.
			if (mPageOut)
			{
				// Recurse down, calculating min recursion level of all children.
				int level = 9999;
				for (int i = 0; i < 4; ++i)
				{
					level = min(level, mChildren[i]->Render(queue, lod));
				}
				// If we are a shallow node.
				if (!mRequestRenderable && level <= 1)
				{
					mRequestRenderable = true;
					mCube->Request(this, PlanetCube::REQUEST_RENDERABLE);
				}
				return level + 1;
			}

			// If we are renderable, check LOD/visibility.
			if (mRenderable)
			{
				mRenderable->setFrameOfReference(lod);

				// If invisible, return immediately.
				if (mRenderable->isClipped())
					return 1;

				// Whether to recurse down.
				bool recurse = false;

				// If the texture is not fine enough...
				if (!mRenderable->isInMIPRange())
				{
					// If there is already a native res map-tile...
					if (mMapTile)
					{
						// Make sure the renderable is up-to-date.
						if (mRenderable->getMapTile() == mMapTile)
						{
							// Split so we can try this again on the child tiles.
							recurse = true;
						}
					}
					// Otherwise try to get native res tile data.
					else
					{
						// Make sure no parents are waiting for tile data update.
						QuadTreeNode *ancestor = this;
						bool parentRequest = false;
						while (ancestor && !ancestor->mMapTile && !ancestor->mPageOut)
						{
							if (ancestor->mRequestMapTile || ancestor->mRequestRenderable)
							{
								parentRequest = true;
								break;
							}
							ancestor = ancestor->mParent;
						}

						if (!parentRequest)
						{
							// Request a native res map tile.
							mRequestMapTile = true;
							mCube->Request(this, PlanetCube::REQUEST_MAPTILE);
						}
					}
				}

				// If the geometry is not fine enough...
				if ((mHasChildren || !mRequestMapTile) && !mRenderable->isInLODRange())
				{
					// Go down an LOD level.
					recurse = true;
				}

				// If a recursion was requested...
				if (recurse)
				{
					// Update recursion counter, used to find least recently used nodes to page out.
					mLastOpened = mCube->getFrameCounter();

					// And children are available and renderable...
					if (mHasChildren)
					{
						if (willRenderChildren)
						{
							// Recurse down, calculating min recursion level of all children.
							int level = 9999;
							for (int i = 0; i < 4; ++i)
							{
								level = min(level, mChildren[i]->Render(queue, lod));
							}
							// If we are a shallow node with a tile that is not being rendered or close to being rendered.
							if (level > 1 && mMapTile && mMapTile->getReferences() == 1)
							{
								mPageOut = true;
								DestroyRenderable();
								DestroyMapTile();
							}
							return level + 1;
						}
					}
					// If no children exist yet, request them.
					else if (!mRequestSplit)
					{
						mRequestSplit = true;
						mCube->Request(this, PlanetCube::REQUEST_SPLIT);
					}
				}

				// Last rendered flag, used to find ancestor patches that can be paged out.
				mLastRendered = mCube->getFrameCounter();

				// Otherwise, render ourselves.
				mRenderable->updateRenderQueue(queue);

				return 1;
			}
			return 0;
		}

		QuadTree::QuadTree(PlanetCube * cube, int face)
		{
			mRoot = new QuadTreeNode(cube);
			mRoot = root;
			mRoot->mFace = face;
			mRoot->mLOD = 0;
		}
		QuadTree::~QuadTree()
		{
			delete mRoot;
		}

	} // namespace geo
} // namespace sht