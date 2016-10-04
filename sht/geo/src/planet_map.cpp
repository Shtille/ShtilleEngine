#include "planet_map.h"
#include "planet_map_tile.h"

namespace sht {
	namespace geo {

		PlanetMap::PlanetMap()
			: step_(0)
		{
		}
		PlanetMap::~PlanetMap()
		{
		}
		void PlanetMap::ResetTile()
		{
			step_ = 0;
		}
		bool PlanetMap::PrepareTile(PlanetTreeNode * node)
		{
			++step_;
			return true;
		}
		PlanetMapTile * PlanetMap::FinalizeTile(PlanetTreeNode * node)
		{
			step_ = 0;
			return new PlanetMapTile(node);
		}

	} // namespace geo
} // namespace sht