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

			// Spherical distance map clipping via five points check.
			bool is_visible = false;
			for (int i = 0; i < _countof(corner_points_); ++i)
			{
				math::Vector3& corner_point = corner_points_[i];
				is_visible = ((params.camera_position & corner_point) > planet_radius);
				if (is_visible)
					break;
			}
			mIsFarAway = !is_visible;
			mIsClipped = mIsClipped || mIsFarAway;

			//math::Vector3 view_directions[_countof(tile_points)];
			//float min_distance_squared = view_directions[0].Sqr();
			//int min_index = 0;
			//for (int i = 1; i < _countof(view_directions); ++i)
			//{
			//	math::Vector3& view_direction = view_directions[i];
			//	view_direction = params.camera_position - tile_points[i] * planet_radius;
			//	float distance_squared = view_direction.Sqr();
			//	if (distance_squared < min_distance_squared)
			//	{
			//		min_distance_squared = distance_squared;
			//		min_index = i;
			//	}
			//}
			//math::Vector3 near_position_offset = view_directions[min_index];

			// Get vector from center to camera and normalize it.
			math::Vector3 position_offset = params.camera_position - mCenter;
			math::Vector3 view_direction = position_offset;

			// Find the offset between the center of the grid and the grid point closest to the camera (rough approx).
			const float reference_length = math::kPi * 0.375f * planet_radius / (float)(1 << mNode->lod_);
			math::Vector3 reference_offset = view_direction - ((view_direction & mSurfaceNormal) * mSurfaceNormal);
			if (reference_offset.Sqr() > reference_length * reference_length)
			{
				reference_offset.Normalize();
				reference_offset *= reference_length;
			}

			// Find the position offset to the nearest point to the camera (approx).
			math::Vector3 near_position_offset = position_offset - reference_offset;
			float near_position_distance = near_position_offset.Length();

			// Determine LOD priority.
			mLODPriority = -(near_position_offset & params.camera_front) / near_position_distance;

			mIsInLODRange = true; //GetLodDistance() * params.geo_factor < near_position_distance;

			// Calculate texel resolution relative to near grid-point (approx).
			float face_size = scale_factor_ * (planet_radius * math::kPi); // Curved width/height of texture cube face on the sphere
			float cube_side_pixels = static_cast<float>(256 << mMapTile->GetNode()->lod_);
			float texel_size = face_size / cube_side_pixels; // Size of a single texel in world units

			mIsInMIPRange = texel_size * params.tex_factor < near_position_distance;
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
			math::Matrix3 face_transform = PlanetCube::GetFaceTransform(mNode->owner_->face_);

			// Calculate scales, offsets for tile position on cube face.
			const float inv_scale = 2.0f / (float)(1 << mNode->lod_);
			const float position_x = -1.f + inv_scale * mNode->x_;
			const float position_y = -1.f + inv_scale * mNode->y_;

			// Calculate corner point normals
			corner_points_[0].Set(position_x, position_y, 1.0f); // (0,0)
			corner_points_[1].Set(position_x, inv_scale + position_y, 1.0f); // (0,1)
			corner_points_[2].Set(inv_scale + position_x, position_y, 1.0f); // (1,0)
			corner_points_[3].Set(inv_scale + position_x, inv_scale + position_y, 1.0f); // (1,1)
			corner_points_[4].Set(0.5f*inv_scale + position_x, 0.5f*inv_scale + position_y, 1.0f); // (0.5,0.5)
			bool is_visible = false;
			for (int i = 0; i < _countof(corner_points_); ++i)
			{
				math::Vector3& corner_point = corner_points_[i];
				corner_point.Normalize();
				corner_point = face_transform * corner_point;
			}

			// Keep track of extents.
			math::Vector3 min = math::Vector3(1e8), max = math::Vector3(-1e8);
			mCenter = math::Vector3(0, 0, 0);

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

					math::Vector3 sphere_point(x * inv_scale + position_x, y * inv_scale + position_y, 1.0f);
					sphere_point.Normalize();
					sphere_point = face_transform * sphere_point;
					sphere_point *= planet_radius;

					mCenter += sphere_point;

					min.MakeFloor(sphere_point);
					max.MakeCeil(sphere_point);
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
			color_.x = cosf(mMapTile->GetNode()->lod_ * 0.70f) * .35f + .85f;
			color_.y = cosf(mMapTile->GetNode()->lod_ * 1.71f) * .35f + .85f;
			color_.z = cosf(mMapTile->GetNode()->lod_ * 2.64f) * .35f + .85f;
			color_.w = 1.0f;
			//color_ = math::Vector4(1.0f);

			// Calculate area of tile relative to even face division.
			scale_factor_ = sqrt(1.0f / (position_x * position_x + 1.0f) / (position_y * position_y + 1.0f));
			//scale_factor_ = 1.0f / static_cast<float>(2 << mNode->lod_);
		}

	} // namespace geo
} // namespace sht