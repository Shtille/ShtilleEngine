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

#ifndef __SAIM_LIST_H__
#define __SAIM_LIST_H__

#include <stdbool.h>

typedef struct saim_list_node {
	void * data;
	struct saim_list_node * prev;
	struct saim_list_node * next;
} saim_list_node;

typedef struct {
	void (*destroy_func)(void*);
	saim_list_node * head; //!< pointer to the first element
	saim_list_node * tail; //!< pointer to the last element
	unsigned int length;
} saim_list;

void saim_list_create(saim_list * list, void (*destroy_func)(void*));
void saim_list_destroy(saim_list * list); //! deletes all the data

void * saim_list_front(saim_list * list);
void * saim_list_back(saim_list * list);

bool saim_list_empty(saim_list * list);
void saim_list_clear(saim_list * list);
void saim_list_push_front(saim_list * list, void * data);
void saim_list_push_back(saim_list * list, void * data);
void * saim_list_pop_front(saim_list * list);
void * saim_list_pop_back(saim_list * list);
void * saim_list_delete(saim_list * list, saim_list_node * node);
void saim_list_sort(saim_list * list, bool (*sort_func)(const void*, const void*));
saim_list_node * saim_list_find(saim_list * list, const void * data, bool (*cmp_func)(const void*, const void*));

#endif