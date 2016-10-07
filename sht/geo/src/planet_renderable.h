#pragma once
#ifndef __SHT_GEO_PLANET_RENDERABLE_H__
#define __SHT_GEO_PLANET_RENDERABLE_H__

#include "../../math/vector.h"

namespace sht {
	namespace geo {

		class PlanetTreeNode;
		class PlanetMapTile;

		//!
		class PlanetRenderable {
		public:
			PlanetRenderable(PlanetTreeNode * node, PlanetMapTile * map_tile);
			~PlanetRenderable();

			void SetFrameOfReference();

			const bool IsInLODRange() const;
			const bool IsClipped() const;
			const bool IsInMIPRange() const;
			const bool IsFarAway() const;
			float GetLodPriority() const;
			float GetLodDistance();
			void SetChildLodDistance(float lod_distance);

			PlanetMapTile * GetMapTile();

			float mBoundingRadius;
			math::Vector3 mCenter;
			math::Vector3 mBoxCenter;
			math::Vector3 mSurfaceNormal;

			float mChildDistance;
			float mChildDistanceSquared;
			float mLODDifference;
			float mDistance;
			float mDistanceSquared;

			float mScaleFactor;

			PlanetMapTile* mMapTile;
			//Image* mMap;
			const PlanetTreeNode * mNode;

			bool mIsInLODRange;
			bool mIsClipped;
			bool mIsInMIPRange;
			bool mIsFarAway;
			float mLODPriority;

			// Additional params to map to shader
			math::Vector4 stuv_scale_;
			math::Vector4 stuv_position_;
			math::Vector4 color_;

		private:
			void AnalyzeTerrain();
			void InitDisplacementMapping();
		};

	} // namespace geo
} // namespace sht

#endif