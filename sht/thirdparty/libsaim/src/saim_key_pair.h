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

#ifndef __SAIM_KEY_PAIR_H__
#define __SAIM_KEY_PAIR_H__

#include "saim_data_key.h"
#include "saim_data_info.h"

#pragma pack(push, 1)

typedef struct {
    saim_data_key key;
    data_info_t info;
} key_pair_t;

typedef struct {
    saim_data_key key;
    stored_data_info_t info;
} stored_key_pair_t;

#pragma pack(pop)

void make_key_pair(key_pair_t * dst, const stored_key_pair_t * src, file_offset_t key_offset);
void make_stored_key_pair(stored_key_pair_t * dst, const key_pair_t * src);

int key_pair_compare(const key_pair_t * a, const key_pair_t * b);

#endif