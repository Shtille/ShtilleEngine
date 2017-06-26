#include "../include/physics_surface_database.h"

namespace sht {
	namespace physics {

		SurfaceDatabase::SurfaceDatabase()
		{
			Fill();
		}
		SurfaceDatabase::~SurfaceDatabase()
		{

		}
		const Surface * SurfaceDatabase::Get(utility::StringId string_id) const
		{
			const auto it = container_.find(string_id);
			if (it != container_.end())
				return &it->second;
			else
				return nullptr;
		}
		void SurfaceDatabase::Fill()
		{
			// Surface(friction, restitution)
			container_.insert(std::make_pair(ConstexprStringId("brick"),	Surface(0.90f, 0.0f)));
			container_.insert(std::make_pair(ConstexprStringId("metal"),	Surface(0.84f, 0.0f)));
			container_.insert(std::make_pair(ConstexprStringId("leather"),	Surface(0.80f, 0.0f)));
			container_.insert(std::make_pair(ConstexprStringId("stone"),	Surface(0.75f, 0.0f)));
			container_.insert(std::make_pair(ConstexprStringId("wood"),		Surface(0.70f, 0.0f)));
		}

	} // namespace physics
} // namespace sht