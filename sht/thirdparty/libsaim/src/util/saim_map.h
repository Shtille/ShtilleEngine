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

#ifndef __SAIM_MAP_H__
#define __SAIM_MAP_H__

typedef struct saim_map_node {
  void * key;
  void * value;
  int red; /* if red = 0 then the node is black */
  struct saim_map_node * left;
  struct saim_map_node * right;
  struct saim_map_node * parent;
} saim_map_node;


/* compare_func(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* destroy_func(a) takes a pointer to whatever data might be and frees it accordingly */
typedef struct {
	int  (*compare_func)(const void*, const void*);
	void (*key_destroy_func)(void*);
	void (*value_destroy_func)(void*);
	saim_map_node * root;
	saim_map_node * nil;
} saim_map;

saim_map * saim_map_create(int  (*compare_func)(const void*, const void*),
						   void (*key_destroy_func)(void*),
						   void (*value_destroy_func)(void*));
void saim_map_destroy(saim_map * map);

void saim_map_clear(saim_map * map);
saim_map_node * saim_map_insert(saim_map * map, void * key, void * value);
void saim_map_delete(saim_map * map, saim_map_node * node);

saim_map_node * saim_map_search(saim_map * map, void * key);

// Functions for tree enumeration:
saim_map_node * saim_map_get_first(saim_map * map);
saim_map_node * saim_map_get_next(saim_map * map, saim_map_node * prev);

#endif
