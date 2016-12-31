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

#include "saim_provider.h"

#include <stdio.h>
#include <string.h>

bool saim_provider__create(saim_provider * provider)
{
	saim_string_create(&provider->url_format);
	if (mtx_init(&provider->mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	return true;
}
void saim_provider__destroy(saim_provider * provider)
{
	mtx_destroy(&provider->mutex);
	saim_string_destroy(&provider->url_format);
}
bool saim_provider__init(saim_provider * provider, saim_provider_info * provider_info, int flags)
{
	/* We gonna exchange all {x},{y},{z} key masks on %i to easy make formatted url text */
	const char *src;
	char *dst;
	int ** argument_ptr;
	int num_arguments;
	unsigned int length, l;
	bool found_argument;

	src = provider_info->string;
	length = (unsigned int) strlen(src);
	l = 0;
	if (length <= 9)
	{
		fprintf(stderr, "saim: wrong URL format\n");
		return false;
	}
	saim_string_resize(&provider->url_format, length - 9 + 6 + 1);
	dst = provider->url_format.data;
	dst[length-3] = '\0';
	
	argument_ptr = & provider->first_argument;
	num_arguments = 0;
	do
	{
		if (l+3 <= length && *src == '{' && *(src+2) == '}')
		{
			if (num_arguments == 3)
			{
				fprintf(stderr, "saim: wrong URL format\n");
				return false;
			}
			found_argument = false;
			if (*(src+1) == 'x')
			{
				*argument_ptr = & provider->passed_key_x;
				found_argument = true;
			}
			else if (*(src+1) == 'y')
			{
				*argument_ptr = & provider->passed_key_y;
				found_argument = true;
			}
			else if (*(src+1) == 'z')
			{
				*argument_ptr = & provider->passed_key_z;
				found_argument = true;
			}
			if (found_argument)
			{
				++argument_ptr;
				++num_arguments;
				*dst++ = '%';
				*dst++ = 'i';
				src += 3;
				l += 3;
				continue;
			}
		}
		*dst++ = *src++;
		++l;
	}
	while (l < length);

	if (num_arguments == 3)
	{
		// Finally copy the rest parameters
		if (flags & SAIM_BOUNDING_BOX)
		{
			provider->min_latitude  = provider_info->min_latitude;
			provider->max_latitude  = provider_info->max_latitude;
			provider->min_longitude = provider_info->min_longitude;
			provider->max_longitude = provider_info->max_longitude;
		}
		else // default values
		{
			provider->min_latitude  = -85.05112878;
			provider->max_latitude  =  85.05112878;
			provider->min_longitude = -180.0;
			provider->max_longitude =  180.0;
		}
		if (flags & SAIM_LEVEL_OF_DETAIL)
		{
			provider->min_lod = provider_info->min_lod;
			provider->max_lod = provider_info->max_lod;
		}
		else // default values
		{
			provider->min_lod = 0;
			provider->max_lod = 15;
		}
		if (flags & SAIM_BITMAP_SIZE)
		{
			provider->bitmap_width  = provider_info->bitmap_width;
			provider->bitmap_height = provider_info->bitmap_height;
		}
		else // default values
		{
			provider->bitmap_width  = 256;
			provider->bitmap_height = 256;
		}
		return true;
	}
	else
		return false;
}
void saim_provider__fill_buffer_for_key(saim_provider * provider, const saim_data_key * key, char* buffer)
{
	// Make sure that this function is called only from one thread
	mtx_lock(&provider->mutex);
	provider->passed_key_x = saim_data_key__get_x(key);
	provider->passed_key_y = saim_data_key__get_y(key);
	provider->passed_key_z = saim_data_key__get_z(key);
	sprintf(buffer, provider->url_format.data,
		*provider->first_argument, *provider->second_argument, *provider->third_argument);
	mtx_unlock(&provider->mutex);
}