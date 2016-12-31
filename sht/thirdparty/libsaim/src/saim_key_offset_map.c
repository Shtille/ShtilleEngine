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

#include "saim_key_offset_map.h"

#include "util/saim_memory.h"

static int compare_func(const void * p1, const void * p2)
{
	return key_pair_compare((const key_pair_t *)p1, (const key_pair_t *)p2);
}
static void destroy_func(void * p)
{
	// key_pair_t is a simple struct, so do not need to deinitialize
	SAIM_FREE((key_pair_t *)p);
}

void saim_key_offset_map__create(saim_key_offset_map * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void saim_key_offset_map__destroy(saim_key_offset_map * map)
{
	saim_set_destroy(map->set);
}
void saim_key_offset_map__clear(saim_key_offset_map * map)
{
	saim_set_clear(map->set);
}
saim_set_node * saim_key_offset_map__insert(saim_key_offset_map * map, key_pair_t * pair)
{
	return saim_set_insert(map->set, pair);
}
void saim_key_offset_map__erase(saim_key_offset_map * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
saim_set_node * saim_key_offset_map__search(saim_key_offset_map * map, const saim_data_key * key)
{
	key_pair_t pair;
	pair.key = *key;
	return saim_set_search(map->set, &pair);
}