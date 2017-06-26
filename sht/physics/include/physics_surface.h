#pragma once
#ifndef __SHT_PHYSICS_SURFACE_H__
#define __SHT_PHYSICS_SURFACE_H__

namespace sht {
	namespace physics {

		struct Surface {
			Surface(float friction, float restitution)
			: friction(friction)
			, restitution(restitution)
			{
			}
			
			float friction;
			float restitution;
		};

	} // namespace physics
} // namespace sht

#endif