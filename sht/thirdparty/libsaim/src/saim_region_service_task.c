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

#include "saim_region_service_task.h"

#include "saim_storage.h"
#include "saim_curl_wrapper.h"
#include "saim_provider.h"

extern saim_provider * s_provider;

static size_t on_data_received(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	saim_string * str = (saim_string *)(userdata);
	const char* data_buffer = (const char*)(buffer);
	saim_string_append(str, data_buffer, (unsigned int)(nmemb * size));
	return nmemb * size;
}

void saim_region_service_task__create(saim_region_service_task * task, 
	struct saim_storage * storage,
	struct saim_curl_wrapper * curl_wrapper,
	saim_region_notification_function function,
	const saim_data_key * key,
	const saim_string * region_name)
{
	saim_string_create(&task->data);
	task->storage = storage;
	task->curl_wrapper = curl_wrapper;
	task->function = function;
	task->key = *key;
	saim_string_create(&task->region_name);
	saim_string_set(&task->region_name, region_name);
}
void saim_region_service_task__destroy(saim_region_service_task * task)
{
	saim_string_destroy(&task->data);
	saim_string_destroy(&task->region_name);
}
bool saim_region_service_task__execute(saim_region_service_task * task)
{
	char url_buffer[260];
	// Setup working key to make buffer be filled properly
	saim_provider__fill_buffer_for_key(s_provider, &task->key, url_buffer);
	return saim_curl_wrapper__download(task->curl_wrapper,
		url_buffer, (void*)&task->data, on_data_received);
}
void saim_region_service_task__notify(saim_region_service_task * task, bool success)
{
	if (success)
	{
		// Save data to the separate storage
		if (saim_storage__save_separate(task->storage, &task->key, &task->data, &task->region_name))
		{
			// Then notify main observer (to let copy or move data)
			if (task->function)
				task->function(&task->key, &task->data, &task->region_name, true);
		}
		else
		{
			fprintf(stderr, "saim: failed to save to separate storage\n");
		}
	}
	else // skip notification
	{
		if (task->function)
			task->function(&task->key, &task->data, &task->region_name, false);
	}
}