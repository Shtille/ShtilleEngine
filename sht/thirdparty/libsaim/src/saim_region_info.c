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

#include "saim_region_info.h"

void saim_region_info__create(saim_region_info * info)
{
	saim_string_create(&info->name);
}
void saim_region_info__destroy(saim_region_info * info)
{
	saim_string_destroy(&info->name);
}
void saim_region_info__copy(saim_region_info * dest, const saim_region_info * src)
{
	dest->upper_latitude = src->upper_latitude;
	dest->left_longitude = src->left_longitude;
	dest->lower_latitude = src->lower_latitude;
	dest->right_longitude = src->right_longitude;
	saim_string_copy(&dest->name, &src->name);
}
void saim_stored_region_info__create(saim_stored_region_info * info)
{
	saim_region_info__create(&info->info);
	saim_string_create(&info->filename);
}
void saim_stored_region_info__destroy(saim_stored_region_info * info)
{
	saim_region_info__destroy(&info->info);
	saim_string_destroy(&info->filename);
}
void saim_stored_region_info__copy(saim_stored_region_info * dst, const saim_stored_region_info * src)
{
	saim_region_info__copy(&dst->info, &src->info);
	dst->status = src->status;
	dst->time = src->time;
	saim_string_copy(&dst->filename, &src->filename);
}