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

#ifndef __SAIM_PROVIDER_INFO_H__
#define __SAIM_PROVIDER_INFO_H__

/*! @brief Flags that are used in provider info structure.
*          - SAIM_BOUNDING_BOX is used for bounding box range.
*          - SAIM_LEVEL_OF_DETAIL is used for setting the minimum and the maximum lod
             that tile service provides.
           - SAIM_BITMAP_SIZE is used for setting bitmap size of a single tile.
*/
enum {
	SAIM_BOUNDING_BOX		= 1,
	SAIM_LEVEL_OF_DETAIL	= 2,
	SAIM_BITMAP_SIZE		= 4
};

/*! @brief A provider info structure.
*/
typedef struct {
	double min_latitude;	//!< minimum latitude in tile service source
	double max_latitude;	//!< maximum latitude in tile service source
	double min_longitude;	//!< minimum longitude in tile service source
	double max_longitude;	//!< maximum longitude in tile service source
	int min_lod;			//!< minimum LOD of the tile service
	int max_lod;			//!< maximum LOD of the tile service
	int bitmap_width;		//!< a single tile width
	int bitmap_height;		//!< a single tile height
	char string[260];		//!< URL format to download tiles
} saim_provider_info;

#endif