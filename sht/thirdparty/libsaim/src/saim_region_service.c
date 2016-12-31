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

#include "saim_region_service.h"

#include "util/saim_memory.h"
#include "saim_manager.h"

#include <stdio.h>

static int thread_func(void * arg)
{
	volatile bool finishing = false;
	saim_region_service * service = (saim_region_service *)arg;
	saim_region_service_task *task;

	while (!finishing)
	{
		mtx_lock(&service->critical_section);
		finishing = service->finishing;
		// We don't care about tasks loading order
		task = (saim_region_service_task *)saim_list_front(&service->tasks);
		if (task)
		{
			(void)saim_list_pop_front(&service->tasks);
		}
		mtx_unlock(&service->critical_section);

		if (task == NULL)
		{
			// There is no tasks to process, sleep a bit
			thrd_yield();
			continue;
		}

		// Check if our key in viewport
		if (!saim_manager__is_in_viewport(&task->key))
		{
			// We don't need this task anymore
			saim_region_service_task__notify(task, false);
			saim_region_service_task__destroy(task);
			SAIM_FREE(task);
			continue;
		}

		// Start downloading
		if (saim_region_service_task__execute(task))
		{
			// There is only observer for the task, so we just notify it about event
			saim_region_service_task__notify(task, true);
		}
		else
		{
			// We need to remove responding key from requested ones
			saim_region_service_task__notify(task, false);
		}
		saim_region_service_task__destroy(task);
		SAIM_FREE(task);
	}
	return 0;
}
static void task_destroy_func(void* task)
{
	SAIM_FREE((saim_region_service_task *)task);
}

bool saim_region_service__create(saim_region_service * service)
{
	if (mtx_init(&service->critical_section, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	if (!saim_curl_wrapper__create(&service->curl_wrapper))
		return false;
	// Init task list
	saim_list_create(&service->tasks, task_destroy_func);
	return true;
}
void saim_region_service__destroy(saim_region_service * service)
{
	// Some tasks still may exist
	saim_region_service__clear_tasks(service);
	// Destroy task list
	saim_list_destroy(&service->tasks);

	saim_curl_wrapper__destroy(&service->curl_wrapper);
	mtx_destroy(&service->critical_section);
}
void saim_region_service__run_service(saim_region_service * service)
{
	if (thrd_create(&service->thread, thread_func, service) != thrd_success)
	{
		fprintf(stderr, "saim: thread create failed\n");
	}
}
void saim_region_service__stop_service(saim_region_service * service)
{
	mtx_lock(&service->critical_section);
	service->finishing = true;
	mtx_unlock(&service->critical_section);
	thrd_join(service->thread, NULL);
}
void saim_region_service__clear_tasks(saim_region_service * service)
{
	mtx_lock(&service->critical_section);
	saim_list_clear(&service->tasks);
	mtx_unlock(&service->critical_section);
}
void saim_region_service__add_task(saim_region_service * service, saim_region_service_task * task)
{
	mtx_lock(&service->critical_section);
	saim_list_push_back(&service->tasks, task);
	mtx_unlock(&service->critical_section);
}
unsigned int saim_region_service__get_pending_count(saim_region_service * service)
{
	unsigned int task_count;
	mtx_lock(&service->critical_section);
	task_count = service->tasks.length;
	mtx_unlock(&service->critical_section);
	return task_count;
}