#include "chunk_node.h"

namespace sht {
	namespace geo {

		ChunkNode::ChunkNode()
			: data_(nullptr)
			, texture_(nullptr)
			, split_(false)
		{
			for (int i = 0; i < kNumNodes; ++i)
				node_[i] = nullptr;
		}
		ChunkNode::~ChunkNode()
		{
			if (data_)
			{
				delete data_;
				data_ = nullptr;
			}

			//release_texture();

			// for (int i = 0; i < kNumNodes; ++i)
			// 	if (node_[i])
			// 		delete node_[i];
		}
		void ChunkNode::Clear()
		{
			if (split_)
			{
				split_ = false;

				// Recurse to children.
				if (has_children()) {
					for (int i = 0; i < 4; ++i) {
						children_[i]->clear();
					}
				}
			}
			// else
			// {
			// 	if (has_children()) {
			// 		for (int i = 0; i < 4; ++i) {
			// 			count_chunk_stats(children_[i]);
			// 		}
			// 	}
			// }
		}
		void ChunkNode::Update(ChunkTree * tree, const vec3& viewpoint)
		{
			vec3 box_center, box_extent;
			ComputeBoundingBox(*tree, &box_center, &box_extent);

			u16 desired_lod = tree->ComputeLod(box_center, box_extent, viewpoint);

			if (has_children()
				&& desired_lod > (m_lod | 0x0FF)
				&& CanSplit(tree))
			{
				DoSplit(tree, viewpoint);

				// Recurse to children.
				for (int i = 0; i < 4; ++i)
					children_[i]->Update(tree, viewpoint);
			}
			else
			{
				// We're good... this chunk can represent its region within the max error tolerance.
				if ((m_lod & 0xFF00) == 0) {
					// Root chunk -- make sure we have valid morph value.
					m_lod = iclamp(desired_lod, m_lod & 0xFF00, m_lod | 0x0FF);

					assert((m_lod >> 8) == level_);
				}

				// Request residency for our children, and request our
				// grandchildren and further descendents be unloaded.
				if (has_children())
				{
					float priority = 0.0f;
					if (desired_lod > (m_lod & 0xFF00)) {
						priority = (m_lod & 0x0FF) / 255.0f;
					}

					if (priority < 0.5f)
					{
						for (int i = 0; i < 4; ++i)
							children_[i]->RequestUnloadSubtree(tree);
					}
					else
					{
						for (int i = 0; i < 4; ++i)
							children_[i]->WarmUpData(tree, priority);
					}
				}
			}
		}
		void ChunkNode::UpdateTexture(ChunkTree * tree, const vec3& viewpoint)
		{
			assert(tree->m_texture_quadtree != NULL);

			if (m_level >= tree->m_texture_quadtree->get_depth())
			{
				// No texture tiles at this level, so don't bother
				// thinking about them.
				assert(m_texture_id == 0);
				return;
			}

			vec3 box_center, box_extent;
			ComputeBoundingBox(*tree, &box_center, &box_extent);

			int	desired_tex_level = tree->compute_texture_lod(box_center, box_extent, viewpoint);

			if (m_texture_id != 0)
			{
				assert(m_parent == NULL || m_parent->m_texture_id != 0);

				// Decide if we should release our texture.
				if (m_data == NULL
				    || desired_tex_level < m_level)
				{
					// Release our texture, and the texture of any
					// descendents.  Really should go into a cache
					// or something, in case we want to revive it
					// soon.
					request_unload_textures(tree);
				}
				else
				{
					// Keep status quo for this node, and recurse to children.
					if (has_children()) {
						for (int i = 0; i < 4; i++) {
							m_children[i]->update_texture(tree, viewpoint);
						}
					}
				}
			}
			else
			{
				// Decide if we should load our texture.
				if (desired_tex_level >= m_level
				    && m_data)
				{
					// Yes, we would like to load.
					tree->m_loader->request_chunk_load_texture(this);
				}
				else
				{
					// No need to load anything, or to check children.
				}
			}
		}
		void ChunkNode::Render(const ChunkTree& tree)
		{
			assert(m_data != NULL);

			vec3 box_center, box_extent;
			ComputeBoundingBox(tree, &box_center, &box_extent);

			// Frustum culling.
			// if (cull_info.active_planes) {
			// 	cull_info = cull::compute_box_visibility(box_center, box_extent, v.m_frustum, cull_info);
			// 	if (cull_info.culled) {
			// 		// Bounding box is not visible; no need to draw this node or its children.
			// 		return 0;
			// 	}
			// }

			if (texture_bound == false && opt.show_geometry == true)
			{
				// Decide whether to bind a texture.
				
				if (m_texture_id == 0)
				{
					// No texture id, so nothing to bind in any case.
					assert(0);	// this should not happen!
				}
				else
				{
					// If there's no possibility of binding a
					// texture further down the tree, then bind
					// now.
					if (m_split == false
					    || m_children[0]->m_texture_id == 0
					    || m_children[1]->m_texture_id == 0
					    || m_children[2]->m_texture_id == 0
					    || m_children[3]->m_texture_id == 0)
					{
						bind_texture_tile(m_texture_id, box_center, box_extent);
						texture_bound = true;
					}
				}
			}


			if (split_)
			{
				assert(has_children());

				// Recurse to children.  Some subset of our descendants will be rendered in our stead.
				for (int i = 0; i < 4; ++i)
					children_[i]->Render(tree);
			}
			else
			{
				// Display our data.
				data_->Render(tree, *this, v, cull_info, opt, box_center, box_extent);
			}
		}
		void ChunkNode::DoSplit(ChunkTree * tree, const vec3& viewpoint)
		{
			if (split_ == false)
			{
				assert(this->CanSplit(tree));
				assert(has_resident_data());

				m_split = true;

				if (has_children())
				{
					// Make sure children have a valid lod value.
					for (int i = 0; i < 4; ++i)
					{
						ChunkNode * c = children_[i];
						vec3 box_center, box_extent;
						c->ComputeBoundingBox(*tree, &box_center, &box_extent);
						u16	desired_lod = tree->compute_lod(box_center, box_extent, viewpoint);
						c->m_lod = iclamp(desired_lod, c->m_lod & 0xFF00, c->m_lod | 0x0FF);
					}
				}

				// Make sure ancestors are split...
				for (ChunkNode * p = parent_; p && p->split_ == false; p = p->parent_)
					p->DoSplit(tree, viewpoint);
			}
		}
		bool ChunkNode::CanSplit(ChunkTree * tree)
		{
			if (split_)
			{
				// Already split.  Also our data & dependents' data is already
				// freshened, so no need to request it again.
				return true;
			}

			if (has_children() == false)
			{
				// Can't ever split.  No data to request.
				return false;
			}

			bool can_split = true;

			// Check the data of the children.
			for (int i = 0; i < 4; ++i)
			{
				ChunkNode * c = children_[i];
				if (c->has_resident_data() == false)
				{
					tree->m_loader->request_chunk_load(c, 1.0f);
					can_split = false;
				}
			}

			// Make sure ancestors have data...
			for (ChunkNode * p = parent_; p && p->split_ == false; p = p->parent_)
			{
				if (p->CanSplit(tree) == false)
					can_split = false;
			}

			// Make sure neighbors have data at a close-enough level in the tree.
			for (int i = 0; i < 4; ++i)
			{
				ChunkNode * n = neighbour_[i];

				// Allow up to two levels of difference between chunk neighbors.
				const int	MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE = 2;
				for (int count = 0;
				     n && count < MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE;
				     ++count)
				{
					n = n->m_parent;
				}

				if (n && n->CanSplit(tree) == false)
					can_split = false;
			}

			return can_split;
		}
		void ChunkNode::UnloadData()
		{
			assert(parent_ != nullptr && parent_->split_ == false);
			assert(split_ == false);

			// Do the unloading.
			if (data_)
			{
				delete data_;
				data_ = nullptr;
			}
		}
		void ChunkNode::WarmUpData(ChunkTree * tree, float priority)
		{
			if (data_ == nullptr)
			{
				// Request our data.
				tree->m_loader->request_chunk_load(this, priority);
			}

			// Request unload.  Skip a generation if our priority is
			// fairly high.
			if (has_children())
			{
				if (priority < 0.5f)
				{
					// Dump our child nodes' data.
					for (int i = 0; i < 4; ++i)
						children_[i]->RequestUnloadSubtree(tree);
				}
				else
				{
					// Fairly high priority; leave our children
					// loaded, but dump our grandchildren's data.
					for (int i = 0; i < 4; ++i)
					{
						ChunkNode * c = children_[i];
						if (c->has_children())
						{
							for (int j = 0; j < 4; ++j)
								c->children_[j]->RequestUnloadSubtree(tree);
						}
					}
				}
			}
		}
		void ChunkNode::RequestUnloadSubtree(ChunkTree * tree)
		{
			if (data_)
			{
				// Put descendents in the queue first, so they get
				// unloaded first.
				if (has_children())
				{
					for (int i = 0; i < 4; ++i)
						children_[i]->RequestUnloadSubtree(tree);
				}

				tree->m_loader->request_chunk_unload(this);
			}
		}
		void ChunkNode::RequestUnloadTextures(ChunkTree * tree)
		{
			if (texture_)
			{
				// Put descendents in the queue first, so they get
				// unloaded first.
				if (has_children())
				{
					for (int i = 0; i < 4; ++i)
						children_[i]->RequestUnloadTextures(tree);
				}

				tree->m_loader->request_chunk_unload_texture(this);
			}
		}
		void ChunkNode::LookupNeighbours(ChunkTree * tree)
		{
			// TODO: pretty strange function
			for (int i = 0; i < 4; ++i)
			{
				if (neighbour_[i].m_label == -1) {
					m_neighbor[i].m_chunk = NULL;
				} else {
					m_neighbor[i].m_chunk = tree->m_chunk_table[neighbour_[i]];
				}
			}

			if (has_children())
			{
				for (int i = 0; i < 4; ++i)
					children_[i]->LookupNeighbours(tree);
			}
		}
		bool ChunkNode::has_resident_data() const
		{
			return data_ != nullptr;
		}
		bool ChunkNode::has_children() const
		{
			return children_[0] != nullptr;
		}
		int	ChunkNode::level() const
		{
			return static_cast<int>(level_);
		}
		void ChunkNode::ComputeBoundingBox(const ChunkTree& tree, vec3* box_center, vec3* box_extent)
		{
			float level_factor = (1 << (tree.m_tree_depth - 1 - level_));

			box_center->y = (max_y_ + min_y_) * 0.5f * tree.m_vertical_scale;
			box_extent->y = (max_y_ - min_y_) * 0.5f * tree.m_vertical_scale;
				     
			box_center->x = (x_ + 0.5f) * level_factor * tree.m_base_chunk_dimension;
			box_center->z = (z_ + 0.5f) * level_factor * tree.m_base_chunk_dimension;
				     
			const float EXTRA_BOX_SIZE = 1e-3f;	// this is to make chunks overlap by about a millimeter, to avoid cracks.
			box_extent->x = level_factor * tree.m_base_chunk_dimension * 0.5f + EXTRA_BOX_SIZE;
			box_extent->z = box_extent->x;
		}

	} // namespace geo
} // namespace sht