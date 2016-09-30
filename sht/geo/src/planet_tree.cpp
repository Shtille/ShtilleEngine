#include "planet_tree.h"
#include "planet_tile_mesh.h"
#include "../include/planet_cube.h"
#include "../include/constants.h"
#include "../../graphics/include/renderer/shader.h"

namespace sht {
	namespace geo {

		PlanetTreeNode::PlanetTreeNode(PlanetTree * tree)
			: owner_(tree)
			, lod_(0)
			, x_(0)
			, y_(0)
			, has_children_(false)
			, page_out_(false)
			, has_renderable_(false)
			, request_page_out_(false)
			, request_map_tile_(false)
			, request_renderable_(false)
			, request_split_(false)
			, request_merge_(false)
			, parent_slot_(-1)
			, parent_(nullptr)
		{
			last_opened_ = last_rendered_ = owner_->cube_->GetFrameCounter();
			for (int i = 0; i < kNumChildren; ++i)
				children_[i] = nullptr;
		}
		PlanetTreeNode::~PlanetTreeNode()
		{
			owner_->cube_->Unrequest(this);
			if (parent_)
				parent_->children_[parent_slot_] = nullptr;
			DestroyMapTile();
			DestroyRenderable();
			for (int i = 0; i < kNumChildren; ++i)
				DetachChild(i);
		}
		const float PlanetTreeNode::GetPriority() const
		{
			if (!has_renderable_)
			{
				if (parent_)
					return parent_->GetPriority();
				return 0.0f;
			}
			return mRenderable->getLODPriority();
		}
		bool PlanetTreeNode::IsSplit()
		{
			// TODO: exchange on has_chilren ?
			return children_[0] || children_[1] || children_[2] || children_[3];
		}
		void PlanetTreeNode::AttachChild(PlanetTreeNode * child, int position)
		{
			if (children_[position])
				throw "Attaching child where one already exists.";

			children_[position] = child;
			child->parent_ = this;
			child->parent_slot_ = position;

			child->lod_ = lod_ + 1;
			child->x_ = x_ * 2 + (position % 2);
			child->y_ = y_ * 2 + (position / 2);

			has_children_ = true;
		}
		void PlanetTreeNode::DetachChild(int position)
		{
			if (children_[position])
			{
				delete children_[position];
				children_[position] = nullptr;

				has_children_ = children_[0] || children_[1] || children_[2] || children_[3];
			}
		}
		void PlanetTreeNode::PropagateLodDistances()
		{
			if (has_renderable_)
			{
				float max_child_distance = 0.0f;
				// Get maximum LOD distance of all children.
				for (int i = 0; i < kNumChildren; ++i)
				{
					if (children_[i] && children_[i]->has_renderable_)
					{
						// Increase LOD distance w/ centroid distances, to ensure proper LOD nesting.
						float child_distance = children_[i]->mRenderable->getLODDistance();
						if (max_child_distance < child_distance)
							max_child_distance = child_distance;
					}
				}
				// Store in renderable.
				mRenderable->setChildLODDistance(max_child_distance);
			}
			// Propagate changes to parent.
			if (parent_)
				parent_->PropagateLodDistances();
		}
		bool PlanetTreeNode::PrepareMapTile(/*PlanetMap* map*/)
		{
			return true;
			//return map->prepareTile(this);
		}
		void PlanetTreeNode::CreateMapTile(/*PlanetMap* map*/)
		{
			/*if (mMapTile) {
				throw "Creating map tile that already exists.";
			}
			mMapTile = map->finalizeTile(this);*/
		}
		void PlanetTreeNode::DestroyMapTile()
		{
			/*if (mMapTile) delete mMapTile;
			mMapTile = 0;*/
		}
		void PlanetTreeNode::CreateRenderable(/*PlanetMapTile* map*/)
		{
			if (has_renderable_)
				throw "Creating renderable that already exists.";
			if (page_out_)
				page_out_ = false;
			//mRenderable = new PlanetRenderable(this, map);
			has_renderable_ = true;
			PropagateLodDistances();
		}
		void PlanetTreeNode::DestroyRenderable()
		{
			/*if (mRenderable) delete mRenderable;
			mRenderable = 0;*/
			has_renderable_ = false;
			PropagateLodDistances();
		}
		bool PlanetTreeNode::WillRender()
		{
			// Being asked to render ourselves.
			if (!has_renderable_)
			{
				last_opened_ = last_rendered_ = owner_->cube_->GetFrameCounter();

				if (page_out_ && has_children_)
					return true;

				if (!request_renderable_)
				{
					request_renderable_ = true;
					owner_->cube_->Request(this, PlanetCube::REQUEST_RENDERABLE);
				}
				return false;
			}
			return true;
		}
		int PlanetTreeNode::Render()
		{
			PlanetCube * cube = owner_->cube_;
			// Determine if this node's children are render-ready.
			bool will_render_children = true;
			for (int i = 0; i < 4; ++i)
			{
				// Note: intentionally call WillRender on /all/ children, not just until one fails,
				// to ensure all 4 children are queued in immediately.
				if (!children_[i] || !children_[i]->WillRender())
					will_render_children = false;
			}

			// If node is paged out, always recurse.
			if (page_out_)
			{
				// Recurse down, calculating min recursion level of all children.
				int min_level = children_[0]->Render();
				for (int i = 1; i < 4; ++i)
				{
					int level = children_[i]->Render();
					if (level < min_level)
						min_level = level;
				}
				// If we are a shallow node.
				if (!request_renderable_ && min_level <= 1)
				{
					request_renderable_ = true;
					cube->Request(this, PlanetCube::REQUEST_RENDERABLE);
				}
				return min_level + 1;
			}

			// If we are renderable, check LOD/visibility.
			if (has_renderable_)
			{
				mRenderable->setFrameOfReference(lod);

				// If invisible, return immediately.
				if (mRenderable->isClipped()) {
					return 1;
				}

				// Whether to recurse down.
				bool recurse = false;

				// If the texture is not fine enough...
				if (!mRenderable->isInMIPRange()) {
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
						PlanetTreeNode *ancestor = this;
						bool parent_request = false;
						while (ancestor && !ancestor->mMapTile && !ancestor->page_out_)
						{
							if (ancestor->request_map_tile_ || ancestor->request_renderable_)
							{
								parent_request = true;
								break;
							}
							ancestor = ancestor->parent_;
						}

						if (!parent_request)
						{
							// Request a native res map tile.
							request_map_tile_ = true;
							cube->Request(this, PlanetCube::REQUEST_MAPTILE);
						}
					}
				}

				// If the geometry is not fine enough...
				if ((has_children_ || !request_map_tile_) && !mRenderable->isInLODRange())
				{
					// Go down an LOD level.
					recurse = true;
				}

				// If a recursion was requested...
				if (recurse)
				{
					// Update recursion counter, used to find least recently used nodes to page out.
					last_opened_ = cube->GetFrameCounter();

					// And children are available and renderable...
					if (has_children_)
					{
						if (will_render_children)
						{
							// Recurse down, calculating min recursion level of all children.
							int min_level = children_[0]->Render();
							for (int i = 1; i < 4; ++i)
							{
								int level = children_[i]->Render();
								if (level < min_level)
									min_level = level;
							}
							// If we are a shallow node with a tile that is not being rendered or close to being rendered.
							if (min_level > 1 && mMapTile && mMapTile->getReferences() == 1)
							{
								page_out_ = true;
								DestroyRenderable();
								DestroyMapTile();
							}
							return min_level + 1;
						}
					}
					// If no children exist yet, request them.
					else if (!request_split_)
					{
						request_split_ = true;
						cube->Request(this, PlanetCube::REQUEST_SPLIT);
					}
				}

				// Last rendered flag, used to find ancestor patches that can be paged out.
				last_rendered_ = cube->GetFrameCounter();

				// Otherwise, render ourselves.
				mRenderable->setup();
				// <----- Render code from code down here

				return 1;
			}
			return 0;

			graphics::Shader * shader = owner_->cube_->shader_;

			// Calculate scales, offsets for tile position on cube face.
			const float inv_scale = 2.0f / (1 << lod_);
			const float position_x = -1.f + inv_scale * x_;
			const float position_y = -1.f + inv_scale * y_;

			// Correct for GL texture mapping at borders.
			//const float uvCorrection = .05f / (256 + 1);

			// Calculate scales, offset for tile position in map tile.
			//int relativeLOD = lod_ - mMapTile->getNode()->mLOD;
			//const float invTexScale = 1.0f / (1 << relativeLOD) * (1.0f - uvCorrection);
			//const float textureX = invTexScale * (x_ - (mMapTile->getNode()->mX << relativeLOD)) + uvCorrection;
			//const float textureY = invTexScale * (y_ - (mMapTile->getNode()->mY << relativeLOD)) + uvCorrection;

			// bind (inv_scale, inv_scale, invTexScale, invTexScale)
			// bind (position_x, position_y, textureX, textureY)
			// bind planet radius
			// bind planet height
			// bind skirt_height = (1 - cos(angle)) * 1.4f * mPlanetRadius;

			math::Matrix3 face_transform = PlanetCube::GetFaceTransform(owner_->face_);
			shader->Uniform4f("u_stuv_scale", inv_scale, inv_scale, 1.0f, 1.0f);
			shader->Uniform4f("u_stuv_position", position_x, position_y, 0.0f, 0.0f);
			//shader->Uniform1f("u_planet_height", 0.0f);
			shader->Uniform1f("u_skirt_height", 0.0f);
			shader->UniformMatrix3fv("u_face_transform", face_transform);

			// Bind face_transform here
			owner_->cube_->tile_->Render();
			return 0;
		}

		PlanetTree::PlanetTree(PlanetCube * cube, int face)
			: cube_(cube)
			, face_(face)
		{
			root_ = new PlanetTreeNode(this);
		}
		PlanetTree::~PlanetTree()
		{
			delete root_;
		}
		void PlanetTree::Render()
		{
			if (root_->WillRender())
				root_->Render();
		}

	} // namespace geo
} // namespace sht