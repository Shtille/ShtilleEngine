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

#include "saim_memory.h"

/*
static unsigned int make_hash(const char* filename, int line)
{
	unsigned int hash = 5381;
	unsigned char * p;
	int c;

	c = sizeof(line);
	p = (unsigned char *)&line;
	while (c > 0)
	{
		hash = ((hash << 5) + hash) ^ *p;
		++p;
		--c;
	}

	while ((c = *filename++))
		hash = ((hash << 5) + hash) ^ c;

	return hash;
}
*/

#ifdef SAIM_MEMORY_DEBUG

#include "../../deps/tinycthread.h"

#include <stdio.h>

static mtx_t s_mutex;
static int s_number_allocated = 0;
static size_t s_allocated_bytes = 0;

#endif

#ifdef SAIM_USING_USER_MEMORY_FUNCTIONS

static saim_user_memory_functions s_user_functions;

#endif

bool saim_memory_init()
{
#ifdef SAIM_MEMORY_DEBUG
	if (mtx_init(&s_mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	s_number_allocated = 0;
	s_allocated_bytes = 0;
#endif
#ifdef SAIM_USING_USER_MEMORY_FUNCTIONS
	memset(&s_user_functions, 0, sizeof(saim_user_memory_functions));
#endif
	return true;
}
void saim_memory_cleanup()
{
#ifdef SAIM_MEMORY_DEBUG
	saim_memory_check();
	mtx_destroy(&s_mutex);
#endif
#ifdef SAIM_VISUAL_STUDIO_MEMORY_DEBUG
	// Get current flag
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// Turn on leak-checking bit.
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	// Turn off CRT block checking bit.
	tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

	// Set flag to the new value.
	_CrtSetDbgFlag(tmpFlag);
#endif
}
void saim_memory_check()
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	if (s_number_allocated != 0)
	{
		fprintf(stderr, "Still there are %i objects that haven't been deallocated\r\n", s_number_allocated);
	}
	mtx_unlock(&s_mutex);
#endif
}
int saim_memory_num_allocations()
{
#ifdef SAIM_MEMORY_DEBUG
	volatile int num_allocated;
	mtx_lock(&s_mutex);
	num_allocated = s_number_allocated;
	mtx_unlock(&s_mutex);
	return num_allocated;
#else
	return 0;
#endif
}
void * saim_malloc(size_t size, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	++s_number_allocated;
	//s_allocated_bytes += size;
	mtx_unlock(&s_mutex);
#endif
	return malloc(size);
}
void * saim_calloc(size_t num, size_t size, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	++s_number_allocated;
	//s_allocated_bytes += num * size;
	mtx_unlock(&s_mutex);
#endif
	return calloc(num ,size);
}
void * saim_realloc(void * what, size_t size, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	// num allocated stays the same
	//s_allocated_bytes -= old_size;
	//s_allocated_bytes += size;
	mtx_unlock(&s_mutex);
#endif
	return realloc(what, size);
}
void saim_free(void * what)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	--s_number_allocated;
	//s_allocated_bytes -= old_size;
	mtx_unlock(&s_mutex);
#endif
	free(what);
}
char * saim_strdup(const char* str, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	++s_number_allocated;
	//s_allocated_bytes += strlen(str);
	mtx_unlock(&s_mutex);
#endif
	return strdup(str);
}

#ifdef SAIM_USING_USER_MEMORY_FUNCTIONS

saim_user_memory_functions * saim_memory_get_functions_ptr()
{
	return &s_user_functions;
}
void * saim_user_malloc(size_t size)
{
	return s_user_functions->user_malloc(size);
}
void * saim_user_calloc(size_t num, size_t size)
{
	return s_user_functions->user_calloc(num, size);
}
void * saim_user_realloc(void * what, size_t size)
{
	return s_user_functions->user_realloc(what, size);
}
void saim_user_free(void * what)
{
	s_user_functions->user_free(what);
}
char * saim_user_strdup(const char* str)
{
	return s_user_functions->user_strdup(str);
}

#endif