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

#include "saim_stack.h"

#include "saim_memory.h"

void saim_stack_create(saim_stack * stack)
{
	stack->head = NULL;
	stack->length = 0U;
}
void saim_stack_destroy(saim_stack * stack)
{
	void * data;
	while (stack->length != 0)
	{
		data = saim_stack_pop(stack);
		SAIM_FREE(data);
	}
}
void saim_stack_push(saim_stack * stack, void * data)
{
	saim_stack_node * node;
	node = (saim_stack_node *)SAIM_MALLOC(sizeof(saim_stack_node));
	node->data = data;
	node->next = stack->head;
	stack->head = node;
	++stack->length;
}
void * saim_stack_pop(saim_stack * stack)
{
	saim_stack_node * node;
	void * data;
	node = stack->head;
	if (node)
	{
		stack->head = node->next;
		--stack->length;
		data = node->data;
		SAIM_FREE(node);
		return data;
	}
	return NULL;
}