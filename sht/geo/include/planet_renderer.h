#pragma once
#ifndef __SHT_GEO_PLANET_RENDERER_H__
#define __SHT_GEO_PLANET_RENDERER_H__

#include "../../common/non_copyable.h"

namespace sht {
	namespace geo {

		// Forward declarations
		class ChunkTree;
		class ChunkLoader;

		//! Planet renderer class
		class PlanetRenderer : public NonCopyable {
		public:
			PlanetRenderer();
			virtual ~PlanetRenderer();

			void Update();
			void Render();

		private:
			ChunkLoader * loader_;
			static constexpr int kNumSides = 6;
			ChunkTree * side_[kNumSides];
		};

	} // namespace geo
} // namespace sht

#endif