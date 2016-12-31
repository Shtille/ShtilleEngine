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

#include "saim_key_set.h"

#include "util/saim_memory.h"

static int compare_func(const void * key1, const void * key2)
{
	return saim_data_key__compare((const saim_data_key *)key1, (const saim_data_key *)key2);
}
static void destroy_func(void * victim)
{
	saim_data_key * key = (saim_data_key *)victim;
	saim_data_key__destroy(key);
	SAIM_FREE(key);
}

void saim_key_set__create(saim_key_set * set)
{
	set->set = saim_set_create(compare_func, destroy_func);
}
void saim_key_set__destroy(saim_key_set * set)
{
	saim_set_destroy(set->set);
}
void saim_key_set__clear(saim_key_set * set)
{
	saim_set_clear(set->set);
}
void saim_key_set__insert(saim_key_set * set, const saim_data_key * key)
{
	saim_set_node * node;
	node = saim_set_search(set->set, key);
	if (node == set->set->nil)
		(void)saim_set_insert(set->set, (void*)key);
}
void saim_key_set__erase(saim_key_set * set, saim_set_node * node)
{
	saim_set_delete(set->set, node);
}
saim_set_node * saim_key_set__search(saim_key_set * set, const saim_data_key * key)
{
	return saim_set_search(set->set, key);
}
unsigned int saim_key_set__size(saim_key_set * set)
{
	return saim_set_size(set->set);
}