#pragma once
#ifndef __SHT_GEO_PLANET_MAP_TILE_H__
#define __SHT_GEO_PLANET_MAP_TILE_H__

namespace sht {
	namespace geo {

		class PlanetTreeNode;

		class PlanetMapTile {
		public:
			PlanetMapTile(PlanetTreeNode * node);
			~PlanetMapTile();

			PlanetTreeNode * GetNode();

		private:
			PlanetTreeNode * node_;
		};

	} // namespace geo
} // namespace sht

#endif