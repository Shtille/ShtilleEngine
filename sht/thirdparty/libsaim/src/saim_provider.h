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

#ifndef __SAIM_PROVIDER_H__
#define __SAIM_PROVIDER_H__

#include "saim_provider_info.h"
#include "saim_data_key.h"

#include "util/saim_string.h"

#include "../deps/tinycthread.h"

#include <stdbool.h>

typedef struct {
	mtx_t mutex;
	double min_latitude;
	double max_latitude;
	double min_longitude;
	double max_longitude;
	int min_lod;
	int max_lod;
	int bitmap_width;
	int bitmap_height;
	saim_string url_format;
	int passed_key_x;
	int passed_key_y;
	int passed_key_z;
	int * first_argument;
	int * second_argument;
	int * third_argument;
} saim_provider;

bool saim_provider__create(saim_provider * provider);
void saim_provider__destroy(saim_provider * provider);

bool saim_provider__init(saim_provider * provider, saim_provider_info * provider_info, int flags);

void saim_provider__fill_buffer_for_key(saim_provider * provider, const saim_data_key * key, char* buffer);

#endif