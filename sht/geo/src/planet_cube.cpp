#include "planet_cube.h"
#include "planet_tree.h"
#include "planet_tile_mesh.h"

namespace sht {
	namespace geo {

		PlanetCube::PlanetCube(graphics::Renderer * renderer, utility::CameraManager * camera)
			: camera_(camera)
			, grid_size_(17)
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
		void PlanetCube::Update()
		{
			for (int i = 0; i < kNumFaces; ++i)
				faces_[i]->Update();
		}
		void PlanetCube::Render()
		{
			for (int i = 0; i < kNumFaces; ++i)
				faces_[i]->Render();
		}
		math::Matrix3 PlanetCube::GetFaceTransform(int face)
		{
			switch (face)
			{
	        default:
	        case 0:
	            return math::Matrix3( 0, 0, 1,
	                            0, 1, 0,
	                            1, 0, 0
	                           );
	        case 1:
	            return math::Matrix3( 0, 0,-1,
	                            0, 1, 0,
	                           -1, 0, 0
	                           );
	        case 2:
	            return math::Matrix3( 1, 0, 0,
	                            0, 0, 1,
	                            0, 1, 0
	                           );
	        case 3:
	            return math::Matrix3( 1, 0, 0,
	                            0, 0,-1,
	                            0,-1, 0
	                           );
	        case 4:
	            return math::Matrix3(-1, 0, 0,
	                            0, 1, 0,
	                            0, 0, 1
	                           );
	        case 5:
	            return math::Matrix3( 1, 0, 0,
	                            0, 1, 0,
	                            0, 0,-1
	                           );
    		}
		}

	} // namespace geo
} // namespace sht