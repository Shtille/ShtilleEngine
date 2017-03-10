#pragma once
#ifndef __SURFACE_H__
#define __SURFACE_H__

struct Surface {
	Surface() {}
	Surface(float x1, float x2, float x3, float x4)
		: linear_friction(x1)
		, rolling_friction(x2)
		, spinning_friction(x3)
		, restitution(x4)
	{

	}

	float linear_friction;
	float rolling_friction;
	float spinning_friction;
	float restitution;
};

#endif