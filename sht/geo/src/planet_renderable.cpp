#include "planet_renderable.h"
#include "planet_tree.h"
#include "../include/planet_cube.h"
#include "planet_map_tile.h"

#include "../../math/sht_math.h"

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

			// Bounding box clipping.
			mIsClipped = false;//!lod.mCameraFrustum.isVisible(mBox);

										   // Get vector from center to camera and normalize it.
			math::Vector3 positionOffset = params.camera_position - mCenter;
			math::Vector3 viewDirection = positionOffset;

			// Find the offset between the center of the grid and the grid point closest to the camera (rough approx).
			math::Vector3 referenceOffset = 0.5f * (viewDirection - (mSurfaceNormal & viewDirection) * mSurfaceNormal);
			float tileRadius = planet_radius / (float)(1 << mNode->lod_);
			if (referenceOffset.Length() > tileRadius)
			{
				referenceOffset.Normalize();
				referenceOffset = referenceOffset * tileRadius;
			}

			// Spherical distance map clipping.
			math::Vector3 referenceCoordinate = mCenter + referenceOffset;
			referenceCoordinate.Normalize();
			mIsFarAway = ((params.sphere_plane & referenceCoordinate) < params.sphere_clip);
			mIsClipped = mIsClipped || mIsFarAway;

			// Find the position offset to the nearest point to the camera (approx).
			math::Vector3 nearPositionOffset = positionOffset + referenceOffset;

			// Determine LOD priority.
			math::Vector3 priorityAngle = nearPositionOffset;
			priorityAngle.Normalize();
			mLODPriority = -(priorityAngle & params.camera_front) / nearPositionOffset.Length();

			// Determine factor to shrink LOD by due to perspective foreshortening.
			// Pad shortening factor based on LOD level to compensate for grid curving.
			float lodSpan = tileRadius * nearPositionOffset.Length();
			viewDirection.Normalize();
			float lodShorten = std::min(1.0f, (mSurfaceNormal ^ viewDirection).Length() + lodSpan);
			float lodShortenSquared = lodShorten * lodShorten;
			mIsInLODRange = nearPositionOffset.Sqr() >
				std::max(mDistanceSquared, mChildDistanceSquared) * params.geo_factor_squared * lodShortenSquared;

			// Calculate texel resolution relative to near grid-point (approx).
			//float distance = nearPositionOffset.Length(); // Distance to point
			//nearPositionOffset.Normalize();
			//float isotropy = (mSurfaceNormal & positionOffset); // Perspective texture foreshortening along long axis
			//float isolimit = 1.0;//minf(1.0, (.5 + isotropy) * 8); // Beyond the limit of anisotropic filtering, no point in applying high res
			//float faceSize = mScaleFactor * (planet_radius * math::kPi); // Curved width/height of texture cube face on the sphere
			//float res = faceSize / (1 << mMapTile->getNode()->mLOD) / mMap->getWidth(); // Size of a single texel in world units

			mIsInMIPRange = true;//res * params.tex_factor * isolimit < distance;
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

			// Set bounding box/radius.
			mBoundingRadius = (max - min).Length() * 0.5f;
			mBoxCenter = (max + min) * 0.5;
		}
		void PlanetRenderable::InitDisplacementMapping()
		{
			// Calculate scales, offsets for tile position on cube face.
			const float invScale = 2.0f / (1 << mNode->lod_);
			const float positionX = -1.f + invScale * mNode->x_;
			const float positionY = -1.f + invScale * mNode->y_;

			// Correct for GL texture mapping at borders.
			const float uvCorrection = 0.0f; //.05f / (mMap->getWidth() + 1);

			// Calculate scales, offset for tile position in map tile.
			assert(mMapTile->GetNode()->lod_ == 0);
			int relativeLOD = mNode->lod_ - mMapTile->GetNode()->lod_;
			const float invTexScale = 1.0f / (1 << relativeLOD) * (1.0f - uvCorrection);
			const float textureX = invTexScale * (mNode->x_ - (mMapTile->GetNode()->x_ << relativeLOD)) + uvCorrection;
			const float textureY = invTexScale * (mNode->y_ - (mMapTile->GetNode()->y_ << relativeLOD)) + uvCorrection;

			// Set shader parameters
			stuv_scale_.x = invScale;
			stuv_scale_.y = invScale;
			stuv_scale_.z = invTexScale;
			stuv_scale_.w = invTexScale;
			stuv_position_.x = positionX;
			stuv_position_.y = positionY;
			stuv_position_.z = textureX;
			stuv_position_.w = textureY;

			// Set color/tint
			color_.x = cosf(mMapTile->GetNode()->lod_ * 0.70f) * .35f + .85f;
			color_.y = cosf(mMapTile->GetNode()->lod_ * 1.71f) * .35f + .85f;
			color_.z = cosf(mMapTile->GetNode()->lod_ * 2.64f) * .35f + .85f;
			color_.w = 1.0f;

			// Calculate area of tile relative to even face division.
			mScaleFactor = sqrt(1.0f / (positionX * positionX + 1.0f) / (positionY * positionY + 1.0f));
		}

	} // namespace geo
} // namespace sht