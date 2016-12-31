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

#ifndef __SAIM_KEY_SET_H__
#define __SAIM_KEY_SET_H__

#include "util/saim_set.h"
#include "saim_data_key.h"

typedef struct {
	saim_set * set;
} saim_key_set;

void saim_key_set__create(saim_key_set * set);
void saim_key_set__destroy(saim_key_set * set);

void saim_key_set__clear(saim_key_set * set);
void saim_key_set__insert(saim_key_set * set, const saim_data_key * key);
void saim_key_set__erase(saim_key_set * set, saim_set_node * node);
saim_set_node * saim_key_set__search(saim_key_set * set, const saim_data_key * key);

unsigned int saim_key_set__size(saim_key_set * set);

#endif