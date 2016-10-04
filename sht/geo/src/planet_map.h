#pragma once
#ifndef __SHT_GEO_PLANET_MAP_H__
#define __SHT_GEO_PLANET_MAP_H__

namespace sht {
	namespace geo {

		class PlanetTreeNode;
		class PlanetMapTile;

		class PlanetMap {
		public:
			PlanetMap();
			~PlanetMap();

			void ResetTile();
			bool PrepareTile(PlanetTreeNode * node);
			PlanetMapTile * FinalizeTile(PlanetTreeNode * node);

		private:
			int step_;
		};

	} // namespace geo
} // namespace sht

#endif