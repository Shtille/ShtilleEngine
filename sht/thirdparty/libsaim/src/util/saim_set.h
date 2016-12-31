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

#ifndef __SAIM_SET_H__
#define __SAIM_SET_H__

/*
* saim_set is implemented as red-black tree
* NOTE: it allows data duplication, so if want standart C++ style-like functionality,
* you should implement it by your own using saim_set_search check before data insertion.
*/

typedef struct saim_set_node {
  void * data;
  int red; /* if red = 0 then the node is black */
  struct saim_set_node * left;
  struct saim_set_node * right;
  struct saim_set_node * parent;
} saim_set_node;


/* compare_func(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* destroy_func(a) takes a pointer to whatever data might be and frees it accordingly */
typedef struct {
	int  (*compare_func)(const void*, const void*);
	void (*destroy_func)(void*);
	unsigned int size;
	saim_set_node * root;
	saim_set_node * nil;
} saim_set;

saim_set * saim_set_create(int  (*compare_func)(const void*, const void*),
						   void (*destroy_func)(void*));
void saim_set_destroy(saim_set * set);

void saim_set_clear(saim_set * set);
saim_set_node * saim_set_insert(saim_set * set, void * data);
void saim_set_delete(saim_set * set, saim_set_node * node);

/*! Removes node from tree without any data deletion (but frees allocated node pointer,
** so node pointer is invalid anymore)
*/
void saim_set_cut(saim_set * set, saim_set_node * node);

saim_set_node * saim_set_search(saim_set * set, const void * data);

// Functions for tree enumeration:
saim_set_node * saim_set_get_first(saim_set * set);
saim_set_node * saim_set_get_next(saim_set * set, saim_set_node * prev);

unsigned int saim_set_size(saim_set * set);

#endif
