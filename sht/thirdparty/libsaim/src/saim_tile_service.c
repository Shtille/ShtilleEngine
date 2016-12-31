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

#include "saim_tile_service.h"

#include "util/saim_memory.h"
#include "saim_manager.h"

#include <stdio.h>

static int thread_func(void * arg)
{
	volatile bool finishing = false;
	saim_tile_service * service = (saim_tile_service *)arg;
	saim_tile_service_task *task, *cur_task;
	saim_list_node *node, *best_node;

	while (!finishing)
	{
        task = NULL;
		mtx_lock(&service->critical_section);
		finishing = service->finishing;
		// Find a task with lowest detail and remove it from list
		if (service->tasks.length != 0)
		{
			best_node = service->tasks.head;
            task = (saim_tile_service_task *)best_node->data;
			node = best_node->next;
			while (node != NULL)
			{
				cur_task = (saim_tile_service_task *)node->data;
				if (saim_data_key__get_z(&cur_task->key) < saim_data_key__get_z(&task->key)) // less detailed
				{
					task = cur_task;
					best_node = node;
				}
				node = node->next;
			}
			(void)saim_list_delete(&service->tasks, best_node);
		}
		mtx_unlock(&service->critical_section);

		if (task == NULL)
		{
			// Wait for any signal to come
			mtx_lock(&service->critical_section);
			while (service->tasks.length == 0 && !service->finishing)
				cnd_wait(&service->condition_variable, &service->critical_section);
			mtx_unlock(&service->critical_section);
			continue;
		}

		// Check if our key in viewport
		if (!saim_manager__is_in_viewport(&task->key))
		{
			// We don't need this task anymore
			saim_tile_service_task__notify(task, false);
			saim_tile_service_task__destroy(task);
			SAIM_FREE(task);
			continue;
		}

		// Start downloading
		if (saim_tile_service_task__execute(task))
		{
			// There is only observer for the task, so we just notify it about event
			saim_tile_service_task__notify(task, true);
		}
		else
		{
			// We need to remove responding key from requested ones
			saim_tile_service_task__notify(task, false);
		}
		saim_tile_service_task__destroy(task);
		SAIM_FREE(task);
	}
	return 0;
}
static void task_destroy_func(void* task)
{
	SAIM_FREE((saim_tile_service_task *)task);
}

bool saim_tile_service__create(saim_tile_service * service)
{
	if (mtx_init(&service->critical_section, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	if (cnd_init(&service->condition_variable) == thrd_error)
	{
		fprintf(stderr, "saim: condition variable init failed\n");
		return false;
	}
	if (!saim_curl_wrapper__create(&service->curl_wrapper))
		return false;
	// Init task list
	saim_list_create(&service->tasks, task_destroy_func);
	// Don't forget to initialize flag
	service->finishing = false;
	return true;
}
void saim_tile_service__destroy(saim_tile_service * service)
{
	// Some tasks still may exist
	saim_tile_service__clear_tasks(service);
	// Destroy task list
	saim_list_destroy(&service->tasks);

	saim_curl_wrapper__destroy(&service->curl_wrapper);
	cnd_destroy(&service->condition_variable);
	mtx_destroy(&service->critical_section);
}
void saim_tile_service__run_service(saim_tile_service * service)
{
	if (thrd_create(&service->thread, thread_func, service) != thrd_success)
	{
		fprintf(stderr, "saim: thread create failed\n");
	}
}
void saim_tile_service__stop_service(saim_tile_service * service)
{
	mtx_lock(&service->critical_section);
	service->finishing = true;
	cnd_signal(&service->condition_variable);
	mtx_unlock(&service->critical_section);
	thrd_join(service->thread, NULL);
}
void saim_tile_service__clear_tasks(saim_tile_service * service)
{
	mtx_lock(&service->critical_section);
	saim_list_clear(&service->tasks);
	mtx_unlock(&service->critical_section);
}
void saim_tile_service__add_task(saim_tile_service * service, saim_tile_service_task * task)
{
	mtx_lock(&service->critical_section);
	saim_list_push_back(&service->tasks, task);
	cnd_signal(&service->condition_variable);
	mtx_unlock(&service->critical_section);
}
unsigned int saim_tile_service__get_pending_count(saim_tile_service * service)
{
	unsigned int task_count;
	mtx_lock(&service->critical_section);
	task_count = service->tasks.length;
	mtx_unlock(&service->critical_section);
	return task_count;
}