#include "chunk_tree.h"
#include "chunk_node.h"
#include "chunk_loader.h"
#include "../include/constants.h"
#include "../../math/sht_math.h"

namespace sht {
	namespace geo {

		ChunkTree::ChunkTree(int tree_id, ChunkLoader * loader, int depth)
			: tree_id_(tree_id)
			, loader_(loader)
			, depth_(depth)
		{
			// Set parameters if neglects to set it
			SetParameters(5.0f, 1.0f, 640.0f, 90.0f);

			error_lod_max_ = 5.0f; // TODO
			/* How do I compute base chunk dimension?
			kMetersPerDegree = kEarthRadius * 2.0f * math::kPi / 360.0f;
			One tree has base node of 90 degrees of Earth longitude/latitude.
			So base node has arc length of about 90 * 111111 meters
			kBaseChunkLength = 90.0f * kMetersPerDegree;
			*/
			const float kBaseChunkLength = kEarthRadius * (math::kPi * 0.5f);
			base_chunk_dimension_ = kBaseChunkLength / (float)(1 << (depth - 1));

			node_count_ = 0x55555555 & ((1 << depth*2) - 1);
			chunks_ = new ChunkNode[node_count_];
		}
		ChunkTree::~ChunkTree()
		{
			delete[] chunks_;
		}
		void ChunkTree::Update(const vec3& viewpoint)
		{
			if (chunks_[0].data_ == nullptr)
			{
				// Get root-node data!
				loader_->RequestChunkLoad(tree_id_, &chunks_[0], 1.0f);
			}

			chunks_[0].Clear();
			chunks_[0].Update(this, viewpoint);
			chunks_[0].UpdateTexture(this, viewpoint);
		}
		void ChunkTree::Render()
		{
			chunks_[0].Render(*this);
		}
		void ChunkTree::GetBoundingBox(vec3* box_center, vec3* box_extent)
		{
			chunks_[0].ComputeBoundingBox(*this, box_center, box_extent);
		}
		void ChunkTree::SetParameters(float max_pixel_error, float max_texel_size,
			float screen_width_pixels, float horizontal_fov_degrees)
		{
			assert(max_pixel_error > 0);
			assert(max_texel_size > 0);
			assert(screen_width_pixels > 0);
			assert(horizontal_fov_degrees > 0 && horizontal_fov_degrees < 180.0f);

			const float	tan_half_fov = tanf(0.5f * horizontal_fov_degrees * (float) M_PI / 180.0f);
			const float	K = screen_width_pixels / tan_half_fov;

			// distance_lod_max_ is the distance below which we need to be
			// at the maximum LOD.  It's used in ComputeLod(), which is
			// called by the chunks during Update().
			distance_lod_max_ = (error_lod_max_ / max_pixel_error) * K;

			// texture_distance_lod_max_ is the distance below which we
			// need to be at the leaf texture resolution.  It's used in
			// ComputeTextureLod(), which is called by the chunks during
			// UpdateTexture() to decide when to load textures.

			// Compute the geometric size of a texel at the
			// highest LOD in our chunk tree.
			const int tile_size = 256;
			float texel_size_lod_max = base_chunk_dimension_ / (tile_size - 1);	// 1 texel used up by the border.

			texture_distance_lod_max_ = (texel_size_lod_max / max_texel_size) * K;
		}
		u16	ChunkTree::ComputeLod(const vec3& center, const vec3& extent, const vec3& viewpoint) const
		{
			vec3 disp = viewpoint - center;
			disp.x = std::max<float>(0.0f, fabsf(disp.x) - extent.x);
			disp.y = std::max<float>(0.0f, fabsf(disp.y) - extent.y);
			disp.z = std::max<float>(0.0f, fabsf(disp.z) - extent.z);

			float d = disp.Length();

			return depth_ - 1 - int(log2(fmax(1, d / distance_lod_max_)));
		}
		int	ChunkTree::ComputeTextureLod(const vec3& center, const vec3& extent, const vec3& viewpoint) const
		{
			vec3 disp = viewpoint - center;
			disp.x = std::max<float>(0.0f, fabsf(disp.x) - extent.x);
			disp.y = std::max<float>(0.0f, fabsf(disp.y) - extent.y);
			disp.z = std::max<float>(0.0f, fabsf(disp.z) - extent.z);

			float d = disp.Length();

			return depth_ - 1 - int(log2(fmax(1, d / texture_distance_lod_max_)));
		}

	} // namespace geo
} // namespace sht