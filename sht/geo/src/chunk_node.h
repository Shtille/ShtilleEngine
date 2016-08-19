#pragma once
#ifndef __SHT_GEO_PLANET_CHUNK_NODE_H__
#define __SHT_GEO_PLANET_CHUNK_NODE_H__

#include "../../common/types.h"
#include "../../common/non_copyable.h"

namespace sht {
	namespace geo {

		// Forward declarations
		class ChunkTree;

		//! Planet renderer class
		class ChunkNode : public NonCopyable {
			friend class ChunkTree;
		public:
			ChunkNode();
			~ChunkNode();

			void Clear();

			void Update(ChunkTree * tree, const vec3& viewpoint);
			void UpdateTexture(ChunkTree * tree, const vec3& viewpoint);
			void Render(const ChunkTree& tree);

			void DoSplit(ChunkTree * tree, const vec3& viewpoint);
			bool CanSplit(ChunkTree * tree);
			void UnloadData();

			void WarmUpData(ChunkTree * tree, float priority);
			void RequestUnloadSubtree(ChunkTree * tree);

			void RequestUnloadTextures(ChunkTree * tree);

			void LookupNeighbours(ChunkTree * tree);

			// Utilities.
			bool has_resident_data() const;
			bool has_children() const;
			int	level() const;

			void ComputeBoundingBox(const ChunkTree& tree, vec3* box_center, vec3* box_extent);

		private:
			static constexpr int kNumNodes = 4;
			ChunkNode * parent_;
			ChunkNode * children_[kNumNodes];
			ChunkNode * neighbour_[kNumNodes];
			u16 x_, z_;
			u8 level_;
			bool split_;
			u16 level_of_detail_;
			float min_y_, max_y_;
			void * data_; //!< TODO: data to render
			void * texture_; //!< TODO: texture, assigned with this node
		};

	} // namespace geo
} // namespace sht

#endif