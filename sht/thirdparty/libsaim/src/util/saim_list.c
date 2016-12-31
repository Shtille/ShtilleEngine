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

#include "saim_list.h"

#include "saim_memory.h"

void saim_list_create(saim_list * list, void (*destroy_func)(void*))
{
	list->destroy_func = destroy_func;
	list->head = NULL;
	list->tail = NULL;
	list->length = 0U;
}
void saim_list_destroy(saim_list * list)
{
	void * data;
	while (list->length != 0U)
	{
		data = saim_list_pop_front(list);
		if (list->destroy_func)
			list->destroy_func(data);
	}
}
void * saim_list_front(saim_list * list)
{
	saim_list_node * node;

	node = list->head;
	if (node)
		return node->data;
	else
		return NULL;
}
void * saim_list_back(saim_list * list)
{
	saim_list_node * node;

	node = list->tail;
	if (node)
		return node->data;
	else
		return NULL;
}
bool saim_list_empty(saim_list * list)
{
	return list->length == 0U;
}
void saim_list_clear(saim_list * list)
{
	saim_list_destroy(list);
	saim_list_create(list, list->destroy_func);
}
void saim_list_push_front(saim_list * list, void * data)
{
	saim_list_node * node;
	node = (saim_list_node *)SAIM_MALLOC(sizeof(saim_list_node));
	node->data = data;
	node->prev = NULL;
	node->next = list->head;
	if (node->next)
		node->next->prev = node;
	list->head = node;
	if (list->tail == NULL)
		list->tail = node;
	++list->length;
}
void saim_list_push_back(saim_list * list, void * data)
{
	saim_list_node * node;
	node = (saim_list_node *)SAIM_MALLOC(sizeof(saim_list_node));
	node->data = data;
	node->prev = list->tail;
	node->next = NULL;
	if (node->prev)
		node->prev->next = node;
	if (list->head == NULL)
		list->head = node;
	list->tail = node;
	++list->length;
}
void * saim_list_pop_front(saim_list * list)
{
	saim_list_node * node;
	void * data;
	node = list->head;
	if (node)
	{
		list->head = node->next;
		if (list->head)
			list->head->prev = NULL;
		if (list->tail == node)
			list->tail = NULL;
		--list->length;
		data = node->data;
		SAIM_FREE(node);
		return data;
	}
	return NULL;
}
void * saim_list_pop_back(saim_list * list)
{
	saim_list_node * node;
	void * data;
	node = list->tail;
	if (node)
	{
		list->tail = node->prev;
		if (list->tail)
			list->tail->next = NULL;
		if (list->head == node)
			list->head = NULL;
		--list->length;
		data = node->data;
		SAIM_FREE(node);
		return data;
	}
	return NULL;
}
void * saim_list_delete(saim_list * list, saim_list_node * node)
{
	void * data;
	if (node == NULL)
		return NULL;

	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;
	if (list->head == node)
		list->head = node->next;
	if (list->tail == node)
		list->tail = node->prev;
	--list->length;
	data = node->data;
	SAIM_FREE(node);
	return data;
}
void saim_list_sort(saim_list * list, bool (*sort_func)(const void*, const void*))
{
	// Using merge sort algorithm
	saim_list_node *p, *q, *e, *head, *tail;
	int insize, nmerges, psize, qsize, i;

	if (list->head == NULL || list->head->next == NULL)
		return;

	head = list->head;
	insize = 1;

	while (1)
	{
		p = head;
		head = NULL;
		tail = NULL;

		nmerges = 0;  /* count number of merges we do in this pass */

		while (p)
		{
            ++nmerges;  /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for (i = 0; i < insize; ++i)
            {
                ++psize;
				q = q->next;
                if (!q) break;
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q))
            {

                /* decide whether next element of merge comes from p or q */
                if (psize == 0) {
					/* p is empty; e must come from q. */
					e = q; q = q->next; --qsize;
				} else if (qsize == 0 || !q) {
					/* q is empty; e must come from p. */
					e = p; p = p->next; --psize;
				} else if (sort_func(p->data, q->data)) {
					/* First element of p is lower (or same);
					 * e must come from p. */
					e = p; p = p->next; --psize;
				} else {
					/* First element of q is lower; e must come from q. */
					e = q; q = q->next; --qsize;
				}

                /* add the next element to the merged list */
				if (tail) {
					tail->next = e;
				} else {
					head = e;
				}
				/* Maintain reverse pointers in a doubly linked list. */
				e->prev = tail;
				tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }
		tail->next = NULL;

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
        {
        	list->head = head;
        	list->tail = tail;
            return;
        }

        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }
}
saim_list_node * saim_list_find(saim_list * list, const void * data, bool (*cmp_func)(const void*, const void*))
{
	saim_list_node * node;
	node = list->head;
	while (node != NULL)
	{
		if (cmp_func(data, node->data))
			return node;
		node = node->next;
	}
	return NULL;
}