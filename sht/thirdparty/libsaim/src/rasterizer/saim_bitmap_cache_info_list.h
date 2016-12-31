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

#ifndef __SAIM_BITMAP_CACHE_INFO_LIST_H__
#define __SAIM_BITMAP_CACHE_INFO_LIST_H__

#include "saim_bitmap_cache_info.h"

#include "../util/saim_list.h"

typedef struct {
	saim_list list;
} saim_bitmap_cache_info_list;

void saim_bitmap_cache_info_list__create(saim_bitmap_cache_info_list * list);
void saim_bitmap_cache_info_list__destroy(saim_bitmap_cache_info_list * list);

void saim_bitmap_cache_info_list__clear(saim_bitmap_cache_info_list * list);
void saim_bitmap_cache_info_list__sort(saim_bitmap_cache_info_list * list);
saim_bitmap_cache_info * saim_bitmap_cache_info_list__front(saim_bitmap_cache_info_list * list);
saim_bitmap_cache_info * saim_bitmap_cache_info_list__pop_front(saim_bitmap_cache_info_list * list);
void saim_bitmap_cache_info_list__push_back(saim_bitmap_cache_info_list * list, saim_bitmap_cache_info * info);

#endif