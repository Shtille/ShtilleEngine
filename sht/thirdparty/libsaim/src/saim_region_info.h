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

#ifndef __SAIM_REGION_INFO_H__
#define __SAIM_REGION_INFO_H__

#include "util/saim_string.h"

#include <time.h>

#pragma pack(push, 1)

//! Region status
enum {
	kRegion_Stored = 0, // OK
	kRegion_Downloading = 1,
	kRegion_Invalid = 2
};

//! Region information struct
typedef struct {
	double upper_latitude;	//!< upper latitude of bounding box
	double left_longitude;	//!< left longitude of bounding box
	double lower_latitude;	//!< lower latitude of bounding box
	double right_longitude;	//!< right longitude of bounding box
	saim_string name;		//!< name of a region
} saim_region_info;

//! Stored region information
typedef struct {
	saim_region_info info;		//!< region information
	unsigned int status;	//!< region status
	time_t time;			//!< region data time
	saim_string filename;	//!< region storage filename
} saim_stored_region_info;

#pragma pack(pop)

void saim_region_info__create(saim_region_info * info);
void saim_region_info__destroy(saim_region_info * info);
void saim_region_info__copy(saim_region_info * dest, const saim_region_info * src);

void saim_stored_region_info__create(saim_stored_region_info * info);
void saim_stored_region_info__destroy(saim_stored_region_info * info);
void saim_stored_region_info__copy(saim_stored_region_info * dst, const saim_stored_region_info * src);

#endif