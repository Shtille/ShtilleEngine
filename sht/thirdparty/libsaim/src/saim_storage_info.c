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

#include "saim_storage_info.h"

#include <stdlib.h>

void saim_storage_info__create(saim_storage_info * info)
{
	saim_key_offset_map__create(&info->offsets);
	saim_map_nodes_list__create(&info->list);
}
void saim_storage_info__destroy(saim_storage_info * info)
{
	saim_key_offset_map__destroy(&info->offsets);
	saim_map_nodes_list__destroy(&info->list);
}