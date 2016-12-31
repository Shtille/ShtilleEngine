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

#ifndef __SAIM_DATA_KEY_H__
#define __SAIM_DATA_KEY_H__

//! Define hash type for key struct
typedef unsigned long long key_hash_t;

//! Data key struct
typedef struct {
    key_hash_t hash;
} saim_data_key;

void saim_data_key__create(saim_data_key * key, int x, int y, int z);
void saim_data_key__destroy(saim_data_key * key);

void saim_data_key__set(saim_data_key * key, int x, int y, int z);
void saim_data_key__set_by_other(saim_data_key * key, const saim_data_key * other_key);
int saim_data_key__get_x(const saim_data_key * key);
int saim_data_key__get_y(const saim_data_key * key);
int saim_data_key__get_z(const saim_data_key * key);

int saim_data_key__compare(const saim_data_key * key1, const saim_data_key * key2);

#endif