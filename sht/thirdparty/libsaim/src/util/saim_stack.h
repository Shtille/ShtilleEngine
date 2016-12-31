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

#ifndef __SAIM_STACK_H__
#define __SAIM_STACK_H__

/*
* Class that implements FIFO logics
*/

typedef struct saim_stack_node {
	void * data;
	struct saim_stack_node * next;
} saim_stack_node;

typedef struct {
	saim_stack_node * head;
	unsigned int length;
} saim_stack;

void saim_stack_create(saim_stack * stack);
void saim_stack_destroy(saim_stack * stack);

void saim_stack_push(saim_stack * stack, void * data);
void * saim_stack_pop(saim_stack * stack);

#endif