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

#include "saim_storage_info_pair.h"

void saim_storage_info_pair__create(saim_storage_info_pair * pair)
{
	saim_string_create(&pair->name);
	saim_storage_info__create(&pair->info);
}
void saim_storage_info_pair__destroy(saim_storage_info_pair * pair)
{
	saim_string_destroy(&pair->name);
	saim_storage_info__destroy(&pair->info);
}
int saim_storage_info_pair__compare(const saim_storage_info_pair * a, const saim_storage_info_pair * b)
{
	return saim_string_compare(&a->name, &b->name);
}