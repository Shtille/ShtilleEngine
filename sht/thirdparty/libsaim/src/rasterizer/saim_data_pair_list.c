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

#include "saim_data_pair_list.h"

#include "../util/saim_memory.h"

static void data_pair_destroy_func(void* p)
{
	saim_data_pair * pair = (saim_data_pair *) p;
	saim_data_pair__destroy(pair);
	SAIM_FREE(pair);
}
void saim_data_pair_list__create(saim_data_pair_list * list)
{
	saim_list_create(&list->list, data_pair_destroy_func);
}
void saim_data_pair_list__destroy(saim_data_pair_list * list)
{
	saim_list_destroy(&list->list);
}
void saim_data_pair_list__clear(saim_data_pair_list * list)
{
	saim_list_clear(&list->list);
}
void saim_data_pair_list__push_back(saim_data_pair_list * list, saim_data_pair * pair)
{
	saim_list_push_back(&list->list, pair);
}
saim_data_pair * saim_data_pair_list__pop_front(saim_data_pair_list * list)
{
	return (saim_data_pair *) saim_list_pop_front(&list->list);
}