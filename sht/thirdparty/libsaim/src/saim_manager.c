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

#include "saim_manager.h"

#include "saim_key_region.h"
#include "saim_provider.h"
#include "rasterizer/saim_rasterizer.h"

#include "util/saim_memory.h"
#include "util/saim_string.h"

bool s_viewport_clipping_enabled = false;
saim_cache * s_cache = NULL;
saim_provider * s_provider = NULL;
saim_rasterizer * s_rasterizer = NULL;

bool saim_manager__initialize_cache(const char* hash_string)
{
	if (!saim_memory_init())
		return false;

	s_provider = (saim_provider *) SAIM_MALLOC(sizeof(saim_provider));
	if (!saim_provider__create(s_provider))
		return false;

	s_cache = (saim_cache *) SAIM_MALLOC(sizeof(saim_cache));
	if (!saim_cache__create(s_cache, hash_string))
		return false;
	if (!saim_cache__initialize(s_cache))
		return false;

	s_rasterizer = (saim_rasterizer *) SAIM_MALLOC(sizeof(saim_rasterizer));
	if (!saim_rasterizer__create(s_rasterizer))
		return false;

	return true;
}
void saim_manager__deinitialize_cache()
{
	if (s_rasterizer)
	{
		saim_rasterizer__destroy(s_rasterizer);
		SAIM_FREE(s_rasterizer);
		s_rasterizer = NULL;
	}
	if (s_cache)
	{
		saim_cache__destroy(s_cache);
		SAIM_FREE(s_cache);
		s_cache = NULL;
	}
	if (s_provider)
	{
		saim_provider__destroy(s_provider);
		SAIM_FREE(s_provider);
		s_provider = NULL;
	}
	saim_memory_cleanup();
}
bool saim_manager__set_provider(saim_provider_info * provider_info, int flags)
{
	const char * kDefaultUrlMask = "http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}";
	saim_provider_info default_provider_info;
	if (provider_info) // user defined provider info
		memcpy(&default_provider_info, provider_info, sizeof(saim_provider_info));
	else // default provider info
	{
		strcpy(default_provider_info.string, kDefaultUrlMask);
		default_provider_info.string[strlen(kDefaultUrlMask)] = '\0';
	}
	return saim_provider__init(s_provider, &default_provider_info, flags);
}
saim_cache * saim_manager__get_cache()
{
	return s_cache;
}
void saim_manager__enable_viewport_clipping()
{
	s_viewport_clipping_enabled = true;
}
void saim_manager__disable_viewport_clipping()
{
	s_viewport_clipping_enabled = false;
}
bool saim_manager__is_viewport_clipping_enabled()
{
	return s_viewport_clipping_enabled;
}
bool saim_manager__is_in_viewport(const saim_data_key * key)
{
	if (!s_viewport_clipping_enabled)
		return true;
	// TODO: add regions check
	return false;
}