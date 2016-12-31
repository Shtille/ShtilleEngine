/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2016 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#include "saim_key_region.h"

#include <string.h>

void make_key_region(key_region_t * region, int min_x, int max_x, int min_y, int max_y, int level, bool is_break)
{
	region->min_x = min_x;
	region->max_x = max_x;
	region->min_y = min_y;
	region->max_y = max_y;
	region->level = level;
	region->is_break = is_break;
}
void low_detailed_region(key_region_t * region, const key_region_t * other)
{
	if (other->level > 0)
	{
		region->min_x = other->min_x >> 1;
		region->max_x = other->max_x >> 1;
		region->min_y = other->min_y >> 1;
		region->max_y = other->max_y >> 1;
		region->level = other->level - 1;
		region->is_break = other->is_break;
	}
	else
	{
		memcpy(region, other, sizeof(key_region_t));
	}
}
void high_detailed_region(key_region_t * region, const key_region_t * other)
{
	region->min_x = other->min_x << 1;
	region->max_x = other->max_x << 1;
	region->min_y = other->min_y << 1;
	region->max_y = other->max_y << 1;
	region->level = other->level + 1;
	region->is_break = other->is_break;
}
bool is_key_inside_region(const key_region_t * region, const saim_data_key * key)
{
	int key_x, key_y, key_z;
	bool in_x_range;

	key_x = saim_data_key__get_x(key);
	key_y = saim_data_key__get_y(key);
	key_z = saim_data_key__get_z(key);
	in_x_range = (region->is_break) ? (key_x >= region->min_x || key_x <= region->max_x)
	                         		: (key_x >= region->min_x && key_x <= region->max_x);
	return (key_z == region->level)
		&& (key_y >= region->min_y) && (key_y <= region->max_y)
		&& in_x_range;
}
bool key_region_operator_equal(const key_region_t * lhs, const key_region_t * rhs)
{
	return memcmp(lhs, rhs, sizeof(key_region_t)) == 0;
}
bool key_region_operator_less(const key_region_t * lhs, const key_region_t * rhs)
{
	return memcmp(lhs, rhs, sizeof(key_region_t)) < 0;
}