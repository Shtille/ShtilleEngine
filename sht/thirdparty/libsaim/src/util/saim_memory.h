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

#ifndef __SAIM_MEMORY_H__
#define __SAIM_MEMORY_H__

/*
* Debug versions of memory allocation and freeing functions
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool saim_memory_init();
void saim_memory_cleanup();

void saim_memory_check();
int saim_memory_num_allocations();

void * saim_malloc(size_t size, const char* filename, int line);
void * saim_calloc(size_t num, size_t size, const char* filename, int line);
void * saim_realloc(void * what, size_t size, const char* filename, int line);
void saim_free(void * what);
char * saim_strdup(const char* str, const char* filename, int line);

#ifndef NDEBUG // debug

#ifdef _MSC_VER // Visual C++ compiler

#include <crtdbg.h>

// Visual C++ has its own memory functions
#define SAIM_VISUAL_STUDIO_MEMORY_DEBUG

#define SAIM_MALLOC(size)		_malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__)
#define SAIM_CALLOC(num,size)	_calloc_dbg(num, size, _NORMAL_BLOCK, __FILE__, __LINE__)
#define SAIM_REALLOC(what,size)	_realloc_dbg(what, size, _NORMAL_BLOCK, __FILE__, __LINE__)
#define SAIM_FREE(what)			_free_dbg(what, _NORMAL_BLOCK)

#define SAIM_STRDUP(str)		_strdup_dbg(str, _NORMAL_BLOCK, __FILE__, __LINE__)

#else // other compiler

// Use our functions
#define SAIM_MEMORY_DEBUG

#define SAIM_MALLOC(size)		saim_malloc(size, __FILE__, __LINE__)
#define SAIM_CALLOC(num,size)	saim_calloc(num, size, __FILE__, __LINE__)
#define SAIM_REALLOC(what,size)	saim_realloc(what, size, __FILE__, __LINE__)
#define SAIM_FREE(what)			saim_free(what)

#define SAIM_STRDUP(str)		saim_strdup(str, __FILE__, __LINE__)

#endif // _MSC_VER

#else // release

#define SAIM_MALLOC(size)		malloc(size)
#define SAIM_CALLOC(num,size)	calloc(num,size)
#define SAIM_REALLOC(what,size)	realloc(what,size)
#define SAIM_FREE(what)			free(what)

#define SAIM_STRDUP(str)		strdup(str)

#endif // NDEBUG

#endif