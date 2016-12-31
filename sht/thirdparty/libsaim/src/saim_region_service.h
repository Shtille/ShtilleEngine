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

#ifndef __SAIM_REGION_SERVICE_H__
#define __SAIM_REGION_SERVICE_H__

#include "saim_region_service_task.h"
#include "saim_curl_wrapper.h"
#include "util/saim_list.h"

#include "../deps/tinycthread.h"

typedef struct saim_region_service saim_region_service;

struct saim_region_service {
	mtx_t critical_section;				//!< critical section handle
	thrd_t thread;						//!< thread handle
	saim_curl_wrapper curl_wrapper;	//!< each service has its own instance of curl wrapper
	saim_list tasks;					//!< tasks list
	bool finishing;						//!< finishing flag
};

bool saim_region_service__create(saim_region_service * service);
void saim_region_service__destroy(saim_region_service * service);

void saim_region_service__run_service(saim_region_service * service);
void saim_region_service__stop_service(saim_region_service * service);

void saim_region_service__clear_tasks(saim_region_service * service);
void saim_region_service__add_task(saim_region_service * service, saim_region_service_task * task);

unsigned int saim_region_service__get_pending_count(saim_region_service * service);

#endif