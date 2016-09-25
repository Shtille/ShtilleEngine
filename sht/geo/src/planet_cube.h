#pragma once
#ifndef __SHT_GEO_PLANET_CUBE_H__
#define __SHT_GEO_PLANET_CUBE_H__

#include "../../common/non_copyable.h"
#include "../../utility/include/camera.h"

namespace sht {
	namespace graphics {
		class Renderer;
		class Shader;
	}
}

namespace sht {
	namespace geo {

		// Forward declarations
		class PlanetTree;
		class PlanetTileMesh;

		//! Planet rendering class
		class PlanetCube final : public NonCopyable {
			friend class PlanetTreeNode;
		public:
			PlanetCube(graphics::Renderer * renderer, graphics::Shader * shader,
				utility::CameraManager * camera);
			~PlanetCube();

			//! Data video memory objects creation and other things that may fail
			bool Initialize();

			void Update();
			void Render();

			static math::Matrix3 GetFaceTransform(int face);

		private:
			graphics::Shader * shader_;
			utility::CameraManager * camera_;

			static constexpr int kNumFaces = 6;
			PlanetTree * faces_[kNumFaces];

			const int grid_size_;
			PlanetTileMesh * tile_;
		};

	} // namespace geo
} // namespace sht

#endif