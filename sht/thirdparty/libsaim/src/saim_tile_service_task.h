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

#ifndef __SAIM_TILE_SERVICE_TASK_H__
#define __SAIM_TILE_SERVICE_TASK_H__

#include "saim_data_key.h"

#include "util/saim_string.h"

#include <stdbool.h>

typedef void (*saim_tile_notification_function)(const saim_data_key * key, saim_string * data, bool success);

typedef struct saim_tile_service_task saim_tile_service_task;

struct saim_tile_service_task {
	saim_string data;
	struct saim_storage * storage;					//!< storage pointer
	struct saim_curl_wrapper * curl_wrapper;	//!< curl wrapper pointer
	saim_tile_notification_function function;	//!< notification function
	saim_data_key key;								//!< data key
	bool is_load;								//!< load task or download
};

void saim_tile_service_task__create(saim_tile_service_task * task, 
	struct saim_storage * storage,
	struct saim_curl_wrapper * curl_wrapper,
	saim_tile_notification_function function,
	const saim_data_key* key);
void saim_tile_service_task__destroy(saim_tile_service_task * task);

bool saim_tile_service_task__execute(saim_tile_service_task * task);
void saim_tile_service_task__notify(saim_tile_service_task * task, bool success);

#endif