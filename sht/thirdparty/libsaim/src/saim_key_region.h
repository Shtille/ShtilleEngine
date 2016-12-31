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

#ifndef __SAIM_KEY_REGION_H__
#define __SAIM_KEY_REGION_H__

#include "saim_data_key.h"

#include <stdbool.h>

typedef struct {
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int level;
	bool is_break;
} key_region_t;

void make_key_region(key_region_t * region, int min_x, int max_x, int min_y, int max_y, int level, bool is_break);

void low_detailed_region(key_region_t * region, const key_region_t * other);
void high_detailed_region(key_region_t * region, const key_region_t * other);

bool is_key_inside_region(const key_region_t * region, const saim_data_key * key);

bool key_region_operator_equal(const key_region_t * lhs, const key_region_t * rhs);
bool key_region_operator_less(const key_region_t * lhs, const key_region_t * rhs);

#endif