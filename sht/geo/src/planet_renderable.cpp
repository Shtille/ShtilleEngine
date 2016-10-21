#include "planet_renderable.h"
#include "planet_tree.h"
#include "../include/planet_cube.h"
#include "planet_map_tile.h"

#include "../../math/sht_math.h"
#include "../../math/frustum.h"

#include <algorithm>
#include <assert.h>

namespace sht {
	namespace geo {

		PlanetRenderable::PlanetRenderable(PlanetTreeNode * node, PlanetMapTile * map_tile)
			: mNode(node)
			, mMapTile(map_tile)
			, mChildDistance(0.0f)
			, mChildDistanceSquared(0.0f)
		{
			AnalyzeTerrain();
			InitDisplacementMapping();
		}
		PlanetRenderable::~PlanetRenderable()
		{

		}
		void PlanetRenderable::SetFrameOfReference()
		{
			const float planet_radius = mNode->owner_->cube_->radius();
			LodParams& params = mNode->owner_->cube_->lod_params_;
			math::Frustum * frustum = mNode->owner_->cube_->frustum_;

			// Bounding box clipping.
			mIsClipped = !frustum->IsBoxIn(bounding_box_);

			// Get vector from center to camera and normalize it.
			math::Vector3 position_offset = params.camera_position - mCenter;
			math::Vector3 viewDirection = position_offset;

			// Find the offset between the center of the grid and the grid point closest to the camera (rough approx).
			//math::Vector3 reference_offset = 0.5f * (viewDirection - (mSurfaceNormal & viewDirection) * mSurfaceNormal);
			//float tile_radius = planet_radius / (float)(1 << mNode->lod_);
			//if (reference_offset.Length() > tile_radius)
			//{
			//	reference_offset.Normalize();
			//	reference_offset = reference_offset * tile_radius;
			//}

			const float tile_radius = planet_radius / (float)(1 << mNode->lod_);
			const float reference_length = math::kPi * 0.375f * planet_radius / (float)(1 << mNode->lod_);
			math::Vector3 reference_offset = viewDirection - ((viewDirection & mSurfaceNormal) * mSurfaceNormal);
			if (reference_offset.Sqr() > reference_length * reference_length)
			{
				reference_offset.Normalize();
				reference_offset *= reference_length;
			}

			// Spherical distance map clipping via five points check.
			math::Matrix3 face_transform = PlanetCube::GetFaceTransform(mNode->owner_->face_);
			const float inv_scale = 2.0f / (float)(1 << mNode->lod_);
			const float position_x = -1.f + inv_scale * mNode->x_;
			const float position_y = -1.f + inv_scale * mNode->y_;
			math::Vector3 tile_points[5];
			tile_points[0].Set(position_x, position_y, 1.0f); // (0,0)
			tile_points[1].Set(position_x, inv_scale + position_y, 1.0f); // (0,1)
			tile_points[2].Set(inv_scale + position_x, position_y, 1.0f); // (1,0)
			tile_points[3].Set(inv_scale + position_x, inv_scale + position_y, 1.0f); // (1,1)
			tile_points[4].Set(0.5f*inv_scale + position_x, 0.5f*inv_scale + position_y, 1.0f); // (0.5,0.5)
			bool is_visible = false;
			for (int i = 0; i < _countof(tile_points); ++i)
			{
				math::Vector3& tile_point = tile_points[i];
				tile_point.Normalize();
				tile_point = face_transform * tile_point;
				is_visible = ((params.camera_position & tile_point) > planet_radius);
				if (is_visible)
					break;
			}
			mIsFarAway = !is_visible;
			mIsClipped = mIsClipped || mIsFarAway;

			// Find the position offset to the nearest point to the camera (approx).
			math::Vector3 nearPositionOffset = position_offset + reference_offset;

			// Determine LOD priority.
			math::Vector3 priorityAngle = nearPositionOffset;
			priorityAngle.Normalize();
			mLODPriority = -(priorityAngle & params.camera_front) / nearPositionOffset.Length();

			// Determine factor to shrink LOD by due to perspective foreshortening.
			// Pad shortening factor based on LOD level to compensate for grid curving.
			float lodSpan = tile_radius * nearPositionOffset.Length();
			viewDirection.Normalize();
			float lodShorten = std::min(1.0f, (mSurfaceNormal ^ viewDirection).Length() + lodSpan);
			float lodShortenSquared = lodShorten * lodShorten;
			mIsInLODRange = nearPositionOffset.Sqr() >
				std::max(mDistanceSquared, mChildDistanceSquared) * params.geo_factor_squared * lodShortenSquared;

			// Calculate texel resolution relative to near grid-point (approx).
			float distance = nearPositionOffset.Length(); // Distance to point
			nearPositionOffset.Normalize();
			float isotropy = (mSurfaceNormal & position_offset); // Perspective texture foreshortening along long axis
			float isolimit = 1.0;//minf(1.0, (.5 + isotropy) * 8); // Beyond the limit of anisotropic filtering, no point in applying high res
			float faceSize = mScaleFactor * (planet_radius * math::kPi); // Curved width/height of texture cube face on the sphere
			float res = faceSize / (1 << mMapTile->GetNode()->lod_) / 256.0f/*mMap->getWidth()*/; // Size of a single texel in world units

			mIsInMIPRange = res * params.tex_factor * isolimit < distance;
		}
		const bool PlanetRenderable::IsInLODRange() const
		{
			return mIsInLODRange;
		}
		const bool PlanetRenderable::IsClipped() const
		{
			return mIsClipped;
		}
		const bool PlanetRenderable::IsInMIPRange() const
		{
			return mIsInMIPRange;
		}
		const bool PlanetRenderable::IsFarAway() const
		{
			return mIsFarAway;
		}
		float PlanetRenderable::GetLodPriority() const
		{
			return mLODPriority;
		}
		float PlanetRenderable::GetLodDistance()
		{
			if (mDistance > mChildDistance)
				return mDistance;
			else
				return mChildDistance;
		}
		void PlanetRenderable::SetChildLodDistance(float lod_distance)
		{
			mChildDistance = lod_distance;
			mChildDistanceSquared = mChildDistance * mChildDistance;
		}
		PlanetMapTile * PlanetRenderable::GetMapTile()
		{
			return mMapTile;
		}
		void PlanetRenderable::AnalyzeTerrain()
		{
			const int sGridSize = mNode->owner_->cube_->grid_size();
			const float planet_radius = mNode->owner_->cube_->radius();

			// Calculate scales, offsets for tile position on cube face.
			const float invScale = 2.0f / (float)(1 << mNode->lod_);
			const float positionX = -1.f + invScale * mNode->x_;
			const float positionY = -1.f + invScale * mNode->y_;

			// Keep track of extents.
			math::Vector3 min = math::Vector3(1e8), max = math::Vector3(-1e8);
			mCenter = math::Vector3(0, 0, 0);

			math::Matrix3 faceTransform = PlanetCube::GetFaceTransform(mNode->owner_->face_);

			// Lossy representation of heightmap
			mLODDifference = 0.0f;

			// Calculate LOD error of sphere.
			float angle = math::kPi / (sGridSize << std::max<int>(0, mNode->lod_ - 1));
			float sphereError = (1 - cos(angle)) * 1.4f * planet_radius;
			mDistance = sphereError;

			// Cache square.
			mDistanceSquared = mDistance * mDistance;

			// Process vertex data for regular grid.
			for (int j = 0; j < sGridSize; j++)
			{
				for (int i = 0; i < sGridSize; i++)
				{
					float x = (float)i / (float)(sGridSize - 1);
					float y = (float)j / (float)(sGridSize - 1);

					math::Vector3 spherePoint(x * invScale + positionX, y * invScale + positionY, 1);
					spherePoint.Normalize();
					spherePoint = faceTransform * spherePoint;
					spherePoint *= planet_radius;

					mCenter += spherePoint;

					min.MakeFloor(spherePoint);
					max.MakeCeil(spherePoint);
				}
			}

			// Calculate center.
			mCenter /= (float)(sGridSize * sGridSize);
			mSurfaceNormal = mCenter;
			mSurfaceNormal.Normalize();

			// Set bounding box
			bounding_box_.center = 0.5f * (max + min);
			bounding_box_.extent = 0.5f * (max - min);
		}
		void PlanetRenderable::InitDisplacementMapping()
		{
			// Calculate scales, offsets for tile position on cube face.
			const float inv_scale = 2.0f / (1 << mNode->lod_);
			const float position_x = -1.f + inv_scale * mNode->x_;
			const float position_y = -1.f + inv_scale * mNode->y_;

			// Correct for GL texture mapping at borders.
			const float uv_correction = 0.0f; //.05f / (mMap->getWidth() + 1);

			// Calculate scales, offset for tile position in map tile.
			int relative_lod = mNode->lod_ - mMapTile->GetNode()->lod_;
			const float inv_tex_scale = 1.0f / (1 << relative_lod) * (1.0f - uv_correction);
			const float texture_x = inv_tex_scale * (mNode->x_ - (mMapTile->GetNode()->x_ << relative_lod)) + uv_correction;
			const float texture_y = inv_tex_scale * (mNode->y_ - (mMapTile->GetNode()->y_ << relative_lod)) + uv_correction;

			// Set shader parameters
			stuv_scale_.x = inv_scale;
			stuv_scale_.y = inv_scale;
			stuv_scale_.z = inv_tex_scale;
			stuv_scale_.w = inv_tex_scale;
			stuv_position_.x = position_x;
			stuv_position_.y = position_y;
			stuv_position_.z = texture_x;
			stuv_position_.w = texture_y;

			// Set color/tint
			//color_.x = cosf(mMapTile->GetNode()->lod_ * 0.70f) * .35f + .85f;
			//color_.y = cosf(mMapTile->GetNode()->lod_ * 1.71f) * .35f + .85f;
			//color_.z = cosf(mMapTile->GetNode()->lod_ * 2.64f) * .35f + .85f;
			//color_.w = 1.0f;
			color_ = math::Vector4(1.0f);

			// Calculate area of tile relative to even face division.
			mScaleFactor = sqrt(1.0f / (position_x * position_x + 1.0f) / (position_y * position_y + 1.0f));
		}

	} // namespace geo
} // namespace sht