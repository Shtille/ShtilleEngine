#pragma once
#ifndef __SHT_PHYSICS_SURFACE_DATABASE_H__
#define __SHT_PHYSICS_SURFACE_DATABASE_H__

#include "physics_surface.h"

#include "utility/include/string_id.h"

#include <map>

namespace sht {
	namespace physics {

		class SurfaceDatabase {
			typedef std::map<utility::StringId, Surface> Container;
		public:
			SurfaceDatabase();
			~SurfaceDatabase();

			const Surface * Get(utility::StringId string_id) const;

		private:
			void Fill();

			Container container_;
		};

	} // namespace physics
} // namespace sht

#endif