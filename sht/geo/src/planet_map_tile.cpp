#include "planet_map_tile.h"

namespace sht {
	namespace geo {

		PlanetMapTile::PlanetMapTile(PlanetTreeNode * node)
			: node_(node)
		{
		}
		PlanetMapTile::~PlanetMapTile()
		{
		}
		PlanetTreeNode * PlanetMapTile::GetNode()
		{
			return node_;
		}

	} // namespace geo
} // namespace sht