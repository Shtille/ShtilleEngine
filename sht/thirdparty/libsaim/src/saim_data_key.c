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

#include "saim_data_key.h"

#include <string.h>

// Shifts
static const key_hash_t kYShift = 28ULL;
static const key_hash_t kZShift = 56ULL;
// Masks
static const key_hash_t kXMask = 0x000000000FFFFFFFULL;
static const key_hash_t kYMask = 0x00FFFFFFF0000000ULL;
static const key_hash_t kZMask = 0xFF00000000000000ULL;

static key_hash_t hash_function(int x, int y, int z)
{
	key_hash_t ux = (key_hash_t)(x+1);
	key_hash_t uy = (key_hash_t)(y+1);
	key_hash_t uz = (key_hash_t)(z+1);
	return (uz << kZShift) | (uy << kYShift) | (ux);
}

void saim_data_key__create(saim_data_key * key, int x, int y, int z)
{
	key->hash = hash_function(x, y, z);
}
void saim_data_key__destroy(saim_data_key * key)
{
	// Nothing to do
}
void saim_data_key__set(saim_data_key * key, int x, int y, int z)
{
	key->hash = hash_function(x, y, z);
}
void saim_data_key__set_by_other(saim_data_key * key, const saim_data_key * other_key)
{
	key->hash = other_key->hash;
}
int saim_data_key__get_x(const saim_data_key * key)
{
	return (int)((key->hash & kXMask)           )-1;
}
int saim_data_key__get_y(const saim_data_key * key)
{
	return (int)((key->hash & kYMask) >> kYShift)-1;
}
int saim_data_key__get_z(const saim_data_key * key)
{
	return (int)((key->hash & kZMask) >> kZShift)-1;
}
int saim_data_key__compare(const saim_data_key * key1, const saim_data_key * key2)
{
	if (key1->hash > key2->hash) return 1;
	if (key1->hash < key2->hash) return -1;
	return 0;
}