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

#ifndef __SAIM_DATA_INFO_H__
#define __SAIM_DATA_INFO_H__

#include "saim_defines.h"

#pragma pack(push, 1)

//! Data information to store in file
typedef struct {
    file_offset_t offset;   //!< offset from the beginning of file
    file_size_t size;       //!< size of stored data
    counter_t counter;      //!< usage counter for key replacement
} stored_data_info_t;

//! Data information struct for inner use
typedef struct {
    file_offset_t key_offset;   //!< key offset from the beginning of file
    file_offset_t data_offset;  //!< data offset from the beginning of file
    file_size_t size;           //!< size of stored data
    counter_t counter;          //!< usage counter for key replacement
} data_info_t;

#pragma pack(pop)

#endif