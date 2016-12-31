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

#ifndef __SAIM_MANAGER_H__
#define __SAIM_MANAGER_H__

#include "saim_data_key.h"
#include "saim_cache.h"
#include "saim_provider_info.h"

#include <stdbool.h>

bool saim_manager__initialize_cache(const char* hash_string);
void saim_manager__deinitialize_cache();

bool saim_manager__set_provider(saim_provider_info * provider_info, int flags);

saim_cache * saim_manager__get_cache();

void saim_manager__enable_viewport_clipping();
void saim_manager__disable_viewport_clipping();
bool saim_manager__is_viewport_clipping_enabled();

bool saim_manager__is_in_viewport(const saim_data_key * key);

#endif