#include "planet_cube.h"

namespace {
	const int kTextureSize = 257;
	const int kGridSize = 17;
	const int kLodLimit = 17;
	const float kGeoDetail = 6.0f;
	const float kTexDetail = 1.0f;
}

namespace sht {
	namespace geo {

			PlanetCube::PlanetCube()
			{
				for (int face = 0; face < 6; ++face)
				{
					mFaces[face] = new QuadTree(this, face);
				}
			}
			PlanetCube::~PlanetCube()
			{
				for (int face = 0; face < 6; ++face)
					delete mFaces[face];
			}
			void PlanetCube::SplitQuadTreeNode(QuadTreeNode* node)
			{
				// Parent is no longer an open node, now has at least one child.
				if (node->mParent)
					mOpenNodes.erase(node->mParent);
				// This node is now open.
				mOpenNodes.insert(node);
				// Create children.
				for (int i = 0; i < 4; ++i)
				{
					QuadTreeNode* child = new QuadTreeNode(this);
					node->AttachChild(child, i);
				}
			}
			void PlanetCube::MergeQuadTreeNode(QuadTreeNode* node)
			{
				// Delete children.
				for (int i = 0; i < 4; ++i)
				{
					if (node->mChildren[i]->mChildren[0] ||
						node->mChildren[i]->mChildren[1] ||
						node->mChildren[i]->mChildren[2] ||
						node->mChildren[i]->mChildren[3])
					{
						printf("Lickety split. Faulty merge.\n");
						assert(false);
					}
					node->DetachChild(i);
				}
				// This node is now closed.
				mOpenNodes.erase(node);
				if (node->mParent)
				{
					// Check to see if any siblings are split.
					for (int i = 0; i < 4; ++i)
					{
						if (node->mParent->mChildren[i]->IsSplit())
							return;
					}
					// If not, the parent is now open.
					mOpenNodes.insert(node->mParent);
				}
			}
			void PlanetCube::Request(QuadTreeNode* node, int type, bool priority)
			{
				RequestQueue& requestQueue = (type == REQUEST_MAPTILE) ? mRenderRequests : mInlineRequests;
				if (priority)
					requestQueue.push_front(Request(node, type));
				else
					requestQueue.push_back(Request(node, type));
			}
			void PlanetCube::Unrequest(QuadTreeNode* node)
			{
				RequestQueue* requestQueues[] = { &mRenderRequests, &mInlineRequests };
				for (int q = 0; q < 2; q++)
				{
					RequestQueue::iterator i = (*requestQueues[q]).begin(), j;
					while (i != (*requestQueues[q]).end())
					{
						if ((*i).mNode == node)
						{
							// Remove item at front of queue.
							if (i == (*requestQueues[q]).begin())
							{
								// Special case: if unrequesting a maptile current being generated,
								// make sure temp/unclaimed resources are cleaned up.
								if ((*i).mType == REQUEST_MAPTILE)
								{
									mMap->ResetTile();
								}

								(*requestQueues[q]).erase(i);
								i = (*requestQueues[q]).begin();
								continue;
							}
							// Remove item mid-queue.
							else
							{
								j = i;
								--i;
								(*requestQueues[q]).erase(j);
							}
						}
						++i;
					}
				}
			}
			void PlanetCube::HandleRequests(RequestQueue& queue)
			{
				// Ensure we only use up x time per frame.
				int weights[] = { 10, 10, 1, 2 };
				bool(PlanetCube::*handlers[4])(QuadTreeNode*) = {
					&PlanetCube::handleRenderable,
					&PlanetCube::handleMapTile,
					&PlanetCube::handleSplit,
					&PlanetCube::handleMerge
				};
				int limit = 10;
				bool sorted = false;

				while (requests.size() > 0)
				{
					Request request = *requests.begin();
					QuadTreeNode* node = request.mNode;

					// If not a root level task.
					if (node->mParent)
					{
						// Verify job limits.
						if (limit <= 0)
							return;
						limit -= weights[request.mType];
					}

					requests.pop_front();
					// Call handler.
					if ((this->*handlers[request.mType])(node))
					{
						// Job was completed. We can re-sort the priority queue.
						if (!sorted)
						{
							requests.sort(RequestComparePriority());
							sorted = true;
						}
					}
				}
			}
			void PlanetCube::HandleRenderRequests()
			{
				HandleRequests(mRenderRequests);
			}
			void PlanetCube::HandleInlineRequests()
			{
				HandleRequests(mInlineRequests);
			}
			bool PlanetCube::HandleRenderable(QuadTreeNode* node)
			{
				// Determine max relative LOD depth between grid and tile
				int maxLODRatio = (kTextureSize - 1) / (kGridSize - 1);
				int maxLOD = 0;
				while (maxLODRatio > 1)
				{
					maxLODRatio >>= 1;
					maxLOD++;
				}

				// See if we can find a maptile to derive from.
				QuadTreeNode *ancestor = node;
				while (ancestor->mMapTile == 0 && ancestor->mParent)
				{
					ancestor = ancestor->mParent;
				}

				// See if map tile found is in acceptable LOD range (ie. gridsize <= texturesize).
				if (ancestor->mMapTile)
				{
					int relativeLOD = node->mLOD - ancestor->mLOD;
					if (relativeLOD <= maxLOD)
					{
						// Replace existing renderable.
						node->DestroyRenderable();
						// Create renderable relative to the map tile.
						node->CreateRenderable(ancestor->mMapTile);
						node->mRenderable->setProxy(mProxy);
						node->mRequestRenderable = false;
					}
				}

				// If no renderable was created, try creating a map tile.
				if (node->mRequestRenderable && !node->mMapTile && !node->mRequestMapTile)
				{
					// Request a map tile for this node's LOD level.
					node->mRequestMapTile = true;
					Request(node, REQUEST_MAPTILE, true);
				}
				return true;
			}
			bool PlanetCube::HandleMapTile(QuadTreeNode* node)
			{
				// See if the map tile object for this node is ready yet.
				if (!node->PrepareMapTile(mMap))
				{
					// Needs more work.
					Request(node, REQUEST_MAPTILE, true);
					return false;
				}
				else
				{
					// Assemble a map tile object for this node.
					node->CreateMapTile(mMap);
					node->mRequestMapTile = false;

					// Request a new renderable to match.
					node->mRequestRenderable = true;
					Request(node, REQUEST_RENDERABLE, true);

					// See if any child renderables use the old maptile.
					PlanetMapTile* oldTile;
					RefreshMapTile(node, oldTile);
					return true;
				}
			}
			bool PlanetCube::HandleSplit(QuadTreeNode* node)
			{
				if (node->mLOD < kLodLimit)
				{
					SplitQuadTreeNode(node);
					node->mRequestSplit = false;
				}
				else
				{
					// mRequestSplit is stuck on, so will not be requested again.
				}
				return true;
			}
			bool PlanetCube::HandleMerge(QuadTreeNode* node)
			{
				MergeQuadTreeNode(node);
				node->mRequestMerge = false;
				return true;
			}
			void PlanetCube::PruneTree()
			{
				NodeHeap heap;

				NodeSet::iterator openNode = mOpenNodes.begin();
				while (openNode != mOpenNodes.end())
				{
					heap.push(*openNode);
					++openNode;
				}

				while (heap.size() > 0)
				{
					QuadTreeNode* oldNode = heap.top();
					if (!oldNode->mPageOut && 
						!oldNode->mRequestMerge && 
						(getFrameCounter() - oldNode->mLastOpened > 100))
					{
						oldNode->mRenderable->setFrameOfReference(mLOD);
						// Make sure node's children are too detailed rather than just invisible.
						if (oldNode->mRenderable->isFarAway() ||
							(oldNode->mRenderable->isInLODRange() && oldNode->mRenderable->isInMIPRange())
							)
						{
							oldNode->mRequestMerge = true;
							//handleMerge(oldNode);
							Request(oldNode, REQUEST_MERGE, TRUE);
							return;
						}
						else
						{
							oldNode->mLastOpened = getFrameCounter();
						}
					}
					heap.pop();
				}
			}
			void PlanetCube::RefreshMapTile(QuadTreeNode* node, PlanetMapTile* tile)
			{
				for (int i = 0; i < 4; ++i)
				{
					QuadTreeNode* child = node->mChildren[i];
					if (child && child->mRenderable && child->mRenderable->getMapTile() == tile)
					{
						child->mRequestRenderable = true;
						Request(child, REQUEST_RENDERABLE, true);

						// Recurse
						RefreshMapTile(child, tile);
					}
				}
			}
			void PlanetCube::SetParameters(int screen_height, double fovy_degrees)
			{
				double fov = 2.0 * tan(fovy_degrees * kDegToRad);

				double geoDetail = 6.0;
				mLOD.mGeoFactor = screen_height / (geoDetail * fov);
				mLOD.mGeoFactorSquared = mLOD.mGeoFactor * mLOD.mGeoFactor;

				double texDetail = 1.0;
				mLOD.mTexFactor = screen_height / (texDetail * fov);
				mLOD.mTexFactorSquared = mLOD.mTexFactor * mLOD.mTexFactor;
			}
			const double PlanetCube::getScale() const
			{
				return (double)kEarthRadius;
			}
			inline int PlanetCube::getFrameCounter()
			{
				return mFrameCounter;
			}
			void PlanetCube::Update()
			{

			}
			void PlanetCube::Render()
			{
				// Update LOD state.
				if (!getBool("planet.lodFreeze"))
				{
					mLOD.mCameraPosition = mLODCamera->getPosition() - mProxy->getParentNode()->getPosition();
					mLOD.mCameraFrustum.setModelViewProjMatrix(mLODCamera->getProjectionMatrix() * viewMatrix * fullTransform);
					mLOD.mCameraFront = Vector3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);

					mLOD.mSpherePlane = mLOD.mCameraPosition;
					mLOD.mSpherePlane.Normalize();

					if (mLOD.mCameraPosition.length() > planetRadius)
					{
						mLOD.mSphereClip = cos(
						acos((planetRadius + planetHeight / 2) / (planetRadius + planetHeight)) +
						acos(planetRadius / mLOD.mCameraPosition.length())
						);
					}
					else
					{
						mLOD.mSphereClip = -1;
					}

				}

				for (int i = 0; i < 6; ++i)
				{
					if (mFaces[i]->mRoot->WillRender())
						mFaces[i]->mRoot->Render(queue, mLOD);
				}
    
    			++mFrameCounter;
			}

	} // namespace geo
} // namespace sht