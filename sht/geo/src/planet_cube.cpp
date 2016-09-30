#include "../include/planet_cube.h"
#include "planet_tree.h"
#include "planet_tile_mesh.h"

namespace {
	const float kGeoDetail = 6.0f;
	const float kTexDetail = 1.0f;
}

namespace sht {
	namespace geo {

		PlanetCube::PlanetCube(graphics::Renderer * renderer, graphics::Shader * shader,
			utility::CameraManager * camera, float radius)
			: shader_(shader)
			, camera_(camera)
			, grid_size_(17)
			, radius_(radius)
			, frame_counter_(0)
			, lod_freeze_(false)
			, tree_freeze_(false)
		{
			for (int i = 0; i < kNumFaces; ++i)
				faces_[i] = new PlanetTree(this, i);
			tile_ = new PlanetTileMesh(renderer, grid_size_);
		}
		PlanetCube::~PlanetCube()
		{
			delete tile_;
			for (int i = 0; i < kNumFaces; ++i)
				delete faces_[i];
		}
		bool PlanetCube::Initialize()
		{
			tile_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	        tile_->Create();
	        if (!tile_->MakeRenderable())
	            return false;
			return true;
		}
		void PlanetCube::SetParameters(float fovy_in_radians, int screen_height)
		{
			float fov = 2.0f * tan(fovy_in_radians);

			float geo_detail = std::max(1.0f, kGeoDetail);
			lod_params_.geo_factor = screen_height / (geo_detail * fov);
			lod_params_.geo_factor_squared = lod_params_.geo_factor * lod_params_.geo_factor;

			float tex_detail = std::max(1.0f, kTexDetail);
			lod_params_.tex_factor = screen_height / (tex_detail * fov);
			lod_params_.tex_factor_squared = lod_params_.tex_factor * lod_params_.tex_factor;
		}
		void PlanetCube::Update()
		{
			// Handle delayed requests (for rendering new tiles).
			HandleRenderRequests();

			if (!tree_freeze_)
			{
				// Prune the LOD tree
				PruneTree();

				// Update LOD requests.
				HandleInlineRequests();
			}
		}
		void PlanetCube::Render()
		{
			// Update LOD state.
			if (!lod_freeze_)
			{
				// TODO: need to compensate for full transform on camera position.
				lod_params_.camera_position = *camera_->position();
				lod_params_.camera_front = camera_->GetForward();

				lod_params_.sphere_plane = lod_params_.camera_position;
				lod_params_.sphere_plane.Normalize();

				const float planet_height = 100.0f;

				float camera_distance = lod_params_.camera_position.Length();
				if (camera_distance > radius_)
				{
					lod_params_.sphere_clip = cos(
						acos((radius_ + planet_height / 2.0f) / (radius_ + planet_height)) +
						acos(radius_ / camera_distance)
						);
				}
				else {
					lod_params_.sphere_clip = -1.0f;
				}

			}

			for (int i = 0; i < kNumFaces; ++i)
				faces_[i]->Render();

			++frame_counter_;
		}
		void PlanetCube::SplitQuadTreeNode(PlanetTreeNode* node)
		{
			// Parent is no longer an open node, now has at least one child.
			if (node->parent_)
				open_nodes_.erase(node->parent_);
			// This node is now open.
			open_nodes_.insert(node);
			// Create children.
			for (int i = 0; i < 4; ++i)
			{
				PlanetTreeNode* child = new PlanetTreeNode(node->owner_);
				node->AttachChild(child, i);
			}
		}
		void PlanetCube::MergeQuadTreeNode(PlanetTreeNode* node)
		{
			// Delete children.
			for (int i = 0; i < 4; ++i)
			{
				if (node->children_[i]->children_[0] ||
					node->children_[i]->children_[1] ||
					node->children_[i]->children_[2] ||
					node->children_[i]->children_[3]) {
					printf("Lickety split. Faulty merge.\n");
					assert(false);
				}
				node->DetachChild(i);
			}
			// This node is now closed.
			open_nodes_.erase(node);
			if (node->parent_)
			{
				// Check to see if any siblings are split.
				for (int i = 0; i < 4; ++i)
					if (node->parent_->children_[i]->IsSplit())
						return;
				// If not, the parent is now open.
				open_nodes_.insert(node->parent_);
			}
		}
		void PlanetCube::Request(PlanetTreeNode* node, int type, bool priority)
		{
			RequestQueue& request_queue = (type == REQUEST_MAPTILE) ? render_requests_ : inline_requests_;
			if (priority)
				request_queue.push_front(RequestType(node, type));
			else
				request_queue.push_back(RequestType(node, type));
		}
		void PlanetCube::Unrequest(PlanetTreeNode* node)
		{
			RequestQueue* request_queues[] = { &render_requests_, &inline_requests_ };
			for (int q = 0; q < 2; ++q)
			{
				RequestQueue::iterator i = (*request_queues[q]).begin(), j;
				while (i != (*request_queues[q]).end())
				{
					if ((*i).mNode == node)
					{
						// Remove item at front of queue.
						if (i == (*request_queues[q]).begin())
						{
							// Special case: if unrequesting a maptile current being generated,
							// make sure temp/unclaimed resources are cleaned up.
							if ((*i).mType == REQUEST_MAPTILE)
								mMap->resetTile();

							(*request_queues[q]).erase(i);
							i = (*request_queues[q]).begin();
							continue;
						}
						else // Remove item mid-queue.
						{
							j = i;
							--i;
							(*request_queues[q]).erase(j);
						}
					}
					++i;
				}
			}
		}
		void PlanetCube::HandleRequests(RequestQueue& requests)
		{
			// Ensure we only use up x time per frame.
			int weights[] = { 10, 10, 1, 2 };
			bool(PlanetCube::*handlers[4])(PlanetTreeNode*) = {
				&PlanetCube::HandleRenderable,
				&PlanetCube::HandleMapTile,
				&PlanetCube::HandleSplit,
				&PlanetCube::HandleMerge
			};
			int limit = 10;
			bool sorted = false;

			while (requests.size() > 0) {
				RequestType request = *requests.begin();
				PlanetTreeNode* node = request.mNode;

				// If not a root level task.
				if (node->parent_)
				{
					// Verify job limits.
					if (limit <= 0) return;
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
			HandleRequests(render_requests_);
		}
		void PlanetCube::HandleInlineRequests()
		{
			HandleRequests(inline_requests_);
		}
		bool PlanetCube::HandleRenderable(PlanetTreeNode* node)
		{
			// Determine max relative LOD depth between grid and tile
			int maxLODRatio = (GetTextureSize() - 1) / (grid_size_ - 1);
			int maxLOD = 0;
			while (maxLODRatio > 1) {
				maxLODRatio >>= 1;
				maxLOD++;
			}

			// See if we can find a maptile to derive from.
			PlanetTreeNode * ancestor = node;
			while (ancestor->mMapTile == 0 && ancestor->parent_) { ancestor = ancestor->parent_; };

			// See if map tile found is in acceptable LOD range (ie. gridsize <= texturesize).
			if (ancestor->mMapTile)
			{
				int relativeLOD = node->lod_ - ancestor->lod_;
				if (relativeLOD <= maxLOD) {
					// Replace existing renderable.
					node->DestroyRenderable();
					// Create renderable relative to the map tile.
					node->CreateRenderable(ancestor->mMapTile);
					node->mRenderable->setProxy(mProxy);
					node->request_renderable_ = false;
				}
			}

			// If no renderable was created, try creating a map tile.
			if (node->request_renderable_ && !node->mMapTile && !node->request_map_tile_)
			{
				// Request a map tile for this node's LOD level.
				node->request_map_tile_ = true;
				Request(node, REQUEST_MAPTILE, true);
			}
			return true;
		}
		bool PlanetCube::HandleMapTile(PlanetTreeNode* node)
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
				node->request_map_tile_ = false;

				// Request a new renderable to match.
				node->request_renderable_ = true;
				Request(node, REQUEST_RENDERABLE, true);

				// See if any child renderables use the old maptile.
				PlanetMapTile * old_tile = node->mRenderable->getMapTile();
				RefreshMapTile(node, old_tile);
				return true;
			}
		}
		bool PlanetCube::HandleSplit(PlanetTreeNode* node)
		{
			if (node->lod_ < GetLodLimit())
			{
				SplitQuadTreeNode(node);
				node->request_split_ = false;
			}
			else
			{
				// request_split_ is stuck on, so will not be requested again.
			}
			return true;
		}
		bool PlanetCube::HandleMerge(PlanetTreeNode* node)
		{
			MergeQuadTreeNode(node);
			node->request_merge_ = false;
			return true;
		}
		void PlanetCube::PruneTree()
		{
			NodeHeap heap;

			NodeSet::iterator open_node = open_nodes_.begin();
			while (open_node != open_nodes_.end())
			{
				heap.push(*open_node);
				++open_node;
			};

			while (!heap.empty())
			{
				PlanetTreeNode* old_node = heap.top();
				if (!old_node->page_out_ &&
					!old_node->request_merge_ &&
					(GetFrameCounter() - old_node->last_opened_ > 100))
				{
					old_node->mRenderable->setFrameOfReference(mLOD);
					// Make sure node's children are too detailed rather than just invisible.
					if (old_node->mRenderable->isFarAway() ||
						(old_node->mRenderable->isInLODRange() && old_node->mRenderable->isInMIPRange())
						)
					{
						old_node->request_merge_ = true;
						Request(old_node, REQUEST_MERGE, true);
						return;
					}
					else
					{
						old_node->last_opened_ = GetFrameCounter();
					}
				}
				heap.pop();
			}
		}
		void PlanetCube::RefreshMapTile(PlanetTreeNode* node/*, PlanetMapTile* tile*/)
		{
			for (int i = 0; i < 4; ++i)
			{
				PlanetTreeNode* child = node->children_[i];
				if (child && child->has_renderable_ && child->mRenderable->getMapTile() == tile)
				{
					child->request_renderable_ = true;
					Request(child, REQUEST_RENDERABLE, true);

					// Recurse
					RefreshMapTile(child, tile);
				}
			}
		}
		const float PlanetCube::radius() const
		{
			return radius_;
		}
		math::Matrix3 PlanetCube::GetFaceTransform(int face)
		{
			switch (face)
			{
			default:
			case 0:
				return math::Matrix3(	0, 0, 1,
										0, 1, 0,
										1, 0, 0
									);
			case 1:
				return math::Matrix3(	0, 0,-1,
										0, 1, 0,
										-1, 0, 0
									);
			case 2:
				return math::Matrix3(	1, 0, 0,
										0, 0, 1,
										0, 1, 0
									);
			case 3:
				return math::Matrix3(	1, 0, 0,
										0, 0,-1,
										0,-1, 0
									);
			case 4:
				return math::Matrix3(	-1, 0, 0,
										0, 1, 0,
										0, 0, 1
										);
			case 5:
				return math::Matrix3(	1, 0, 0,
										0, 1, 0,
										0, 0,-1
									);
    		}
		}
		int PlanetCube::GetFrameCounter() const
		{
			return frame_counter_;
		}
		const int PlanetCube::GetLodLimit() const
		{
			return 12;
		}
		const int PlanetCube::GetTextureSize() const
		{
			return 257;
		}
		bool PlanetCube::RequestComparePriority::operator()(const RequestType& a, const RequestType& b) const
		{
			return (a.mNode->GetPriority() > b.mNode->GetPriority());
		}

	} // namespace geo
} // namespace sht