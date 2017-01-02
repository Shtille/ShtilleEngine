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

#ifndef __SAIM_H__
#define __SAIM_H__

#include "saim_provider_info.h"

#ifndef SAIM_VERSION
#define SAIM_VERSION 102
#endif

#ifdef _WIN32

#ifdef BUILDING_LIBSAIM

#ifndef SAIM_STATICLIB
#define SAIM_EXPORT __declspec(dllexport)
#endif /* SAIM_STATICLIB */

#else /* BUILDING_LIBSAIM */

#ifndef SAIM_STATICLIB
#define SAIM_EXPORT __declspec(dllimport)
#endif /* SAIM_STATICLIB */

#endif /* BUILDING_LIBSAIM */

#else /* _WIN32 */

#define SAIM_EXPORT extern

#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! @brief Library initialization function.
 *  @param[in] path          A path there tiles storage is located.
 *  @param[in] provider_info Pointer to provider info that discribes tiles services parameters.
 *  @param[in] flags         Some flags that discribe which parameters from provider info is to use.
 *  @return                  returns 0 on success and not 0 in case of error
*/
int saim_init(const char* path, saim_provider_info * provider_info, int flags);

/*! @brief Library cleanup function.
*/
void saim_cleanup();

/*! @brief Setups target bitmap parameters.
 *  @param[in] buffer          Pointer to the target buffer.
 *  @param[in] width           Width of the target buffer.
 *  @param[in] height          Height of the buffer.
 *  @param[in] bytes_per_pixel Number of bytes per pixel of the target buffer.
*/
void saim_set_target(unsigned char * buffer, int width, int height, int bytes_per_pixel);

/*! @brief Setups maximum bitmap cache size (capacity).
 *  @param[in] size            Size of bitmap.
*/
void saim_set_bitmap_cache_size(unsigned int size);

/*! @brief Defines user functions to allocate and deallocate memory.
 *  @param[in] user_malloc     User malloc function pointer.
 *  @param[in] user_calloc     User calloc function pointer.
 *  @param[in] user_realloc    User realloc function pointer.
 *  @param[in] user_free       User free function pointer.
 *  @param[in] user_strdup     User strdup function pointer.
*/
void saim_set_memory_functions(
	void* (*user_malloc)(size_t /*size*/),
	void* (*user_calloc)(size_t /*num*/, size_t /*size*/),
	void* (*user_realloc)(void* /*what*/, size_t /*size*/),
	void(*user_free)(void* /*what*/),
	char* (*user_strdup)(const char* /*str*/)
);

/*! @brief Renders saved bitmaps onto aligned target surface.
 *  @param[in] upper_latitude  Latitude of the upper left point of rendered rectangle, degrees.
 *  @param[in] left_longitude  Longitude of the upper left point of rendered rectangle, degrees.
 *  @param[in] lower_latitude  Latitude of the lower right point of rendered rectangle, degrees.
 *  @param[in] right_longitude Longitude of the lower right point of rendered rectangle, degrees.
 *  @return                    Returns number of tiles that are left to load (0 if buffer has been filled)
 *  					       and -1 in case of error.
*/
int saim_render_aligned(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude);

/*! @brief Renders saved bitmaps onto target surface.
 *  @param[in] upper_latitude  Latitude of the upper left point of rendered rectangle, degrees.
 *  @param[in] left_longitude  Longitude of the upper left point of rendered rectangle, degrees.
 *  @param[in] lower_latitude  Latitude of the lower right point of rendered rectangle, degrees.
 *  @param[in] right_longitude Longitude of the lower right point of rendered rectangle, degrees.
 *  @param[in] angle           Rotation angle of rectangle, clock wise, degrees.
 *  @return                    Returns number of tiles that are left to load (0 if buffer has been filled) and
 *			   				   -1 in case of error.
*/
int saim_render_common(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif