#include "planet_tree.h"
#include "planet_tile_mesh.h"
#include "planet_cube.h"

namespace sht {
	namespace geo {

		PlanetTreeNode::PlanetTreeNode(PlanetTree * tree)
			: owner_(tree)
			, lod_(0)
			, x_(0)
			, y_(0)
		{

		}
		PlanetTreeNode::~PlanetTreeNode()
		{

		}
		void PlanetTreeNode::Render()
		{
			// Update LOD params (renderable->setFrameOfReference)

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
			// Bind face_transform here
			owner_->cube_->tile_->Render();
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
		void PlanetTree::Update()
		{
			// TODO
		}
		void PlanetTree::Render()
		{
			root_->Render();
		}

	} // namespace geo
} // namespace sht