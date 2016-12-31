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

#include "saim_storage_info_map.h"

#include "util/saim_memory.h"

static int compare_func(const void * p1, const void * p2)
{
	return saim_storage_info_pair__compare((const saim_storage_info_pair *)p1, (const saim_storage_info_pair *)p2);
}
static void destroy_func(void * p)
{
	saim_storage_info_pair * pair = (saim_storage_info_pair *)p;
	saim_storage_info_pair__destroy(pair);
	SAIM_FREE(pair);
}

void saim_storage_info_map__create(saim_storage_info_map * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void saim_storage_info_map__destroy(saim_storage_info_map * map)
{
	saim_set_destroy(map->set);
}
void saim_storage_info_map__clear(saim_storage_info_map * map)
{
	saim_set_clear(map->set);
}
saim_set_node * saim_storage_info_map__insert(saim_storage_info_map * map, saim_storage_info_pair * pair)
{
	return saim_set_insert(map->set, pair);
}
void saim_storage_info_map__erase(saim_storage_info_map * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
void saim_storage_info_map__cut(saim_storage_info_map * map, saim_set_node * node)
{
	saim_set_cut(map->set, node);
}
saim_set_node * saim_storage_info_map__search(saim_storage_info_map * map, const saim_string * name)
{
	saim_set_node * node;
	saim_storage_info_pair pair;

	saim_string_create(&pair.name);
	saim_string_set(&pair.name, name);
	node = saim_set_search(map->set, &pair);
	saim_string_destroy(&pair.name);
	return node;
}