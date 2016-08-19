#pragma once
#ifndef __SHT_GEO_PLANET_CHUNK_TREE_H__
#define __SHT_GEO_PLANET_CHUNK_TREE_H__

#include "../../common/non_copyable.h"
#include "../../math/vector.h"

#include <vector>

namespace sht {
	namespace geo {

		// Forward declarations
		class ChunkNode;
		class ChunkLoader;

		//! Planet renderer class
		class ChunkTree : public NonCopyable {
		public:
			ChunkTree(int tree_id, ChunkLoader * loader, int tree_depth);
			~ChunkTree();

			void Update(const vec3& viewpoint);
			void Render();

			//! Returns the bounding box of the data in this chunk tree.
			void GetBoundingBox(vec3* box_center, vec3* box_extent);

			void SetParameters(float max_pixel_error, float max_texel_size,
				float screen_width_pixels, float horizontal_fov_degrees);
			u16	ComputeLod(const vec3& center, const vec3& extent, const vec3& viewpoint) const;
			int	ComputeTextureLod(const vec3& center, const vec3& extent, const vec3& viewpoint) const;

		private:
			int tree_id_;
			ChunkLoader * loader_;
			int depth_; //!< tree depth
			int node_count_;
			float base_chunk_dimension_;
			float error_lod_max_;
			float distance_lod_max_;
			float texture_distance_lod_max_;
			ChunkNode * chunks_;
		};

	} // namespace geo
} // namespace sht

#endif