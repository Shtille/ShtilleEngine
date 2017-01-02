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

#include "saim.h"

#include "saim_manager.h"
#include "rasterizer/saim_rasterizer.h"
#include "util/saim_memory.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

char s_path[260];

extern saim_rasterizer * s_rasterizer;

int saim_init(const char* path, saim_provider_info * provider_info, int flags)
{
	size_t length;
	char* hash_string;
	// Append path delimeter here
	length = strlen(path);
	if (length > 259)
	{
		fprintf(stderr, "path parameter is too long\n");
		return 1;
	}
	strncpy(s_path, path, length);
	if (length > 0)
	{
		if (path[length-1] != '/')
		{
			s_path[length] = '/';
			s_path[length+1] = '\0';
		}
		else
			s_path[length] = '\0';
	}
	else
		s_path[length] = '\0';

	// Initialize cache
	if (provider_info != NULL)
		hash_string = provider_info->string;
	else
		hash_string = "default"; // it's normal to pass temporary object!
	if (!saim_manager__initialize_cache(hash_string))
	{
		fprintf(stderr, "cache initialization failed\n");
		return 2;
	}

	// Set provider parameters
	if (!saim_manager__set_provider(provider_info, flags))
	{
		fprintf(stderr, "provider setup failed\n");
		return 3;
	}

	return 0;
}
void saim_cleanup()
{
	// Deinitialize cache
	saim_manager__deinitialize_cache();
}
void saim_set_target(unsigned char * buffer, int width, int height, int bytes_per_pixel)
{
	s_rasterizer->target_buffer = buffer;
	s_rasterizer->target_width = width;
	s_rasterizer->target_height = height;
	s_rasterizer->target_bpp = bytes_per_pixel;
}
void saim_set_bitmap_cache_size(unsigned int size)
{
	s_rasterizer->max_bitmap_cache_size = size;
}
void saim_set_memory_functions(
	void* (*user_malloc)(size_t /*size*/),
	void* (*user_calloc)(size_t /*num*/, size_t /*size*/),
	void* (*user_realloc)(void* /*what*/, size_t /*size*/),
	void(*user_free)(void* /*what*/),
	char* (*user_strdup)(const char* /*str*/)
)
{
#ifdef SAIM_USING_USER_MEMORY_FUNCTIONS
	saim_user_memory_functions * funcs = saim_memory_get_functions_ptr();
	funcs->user_malloc = user_malloc;
	funcs->user_calloc = user_calloc;
	funcs->user_realloc = user_realloc;
	funcs->user_free = user_free;
	funcs->user_strdup = user_strdup;
#else
	assert(!"You should define SAIM_USING_USER_MEMORY_FUNCTIONS to use this function");
#endif
}
int saim_render_aligned(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude)
{
	return saim_rasterizer__render_aligned(s_rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude);
}
int saim_render_common(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle)
{
	return saim_rasterizer__render_common(s_rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude, angle);
}