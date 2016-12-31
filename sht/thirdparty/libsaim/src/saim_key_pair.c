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

#include "saim_key_pair.h"

void make_key_pair(key_pair_t * dst, const stored_key_pair_t * src, file_offset_t key_offset)
{
	dst->key = src->key;
	dst->info.key_offset = key_offset;
	dst->info.data_offset = src->info.offset;
	dst->info.size = src->info.size;
	dst->info.counter = src->info.counter;
}
void make_stored_key_pair(stored_key_pair_t * dst, const key_pair_t * src)
{
	dst->key = src->key;
	dst->info.offset = src->info.data_offset;
	dst->info.size = src->info.size;
	dst->info.counter = src->info.counter;
}
int key_pair_compare(const key_pair_t * a, const key_pair_t * b)
{
	// Use key compare function
	return saim_data_key__compare(&a->key, &b->key);
}