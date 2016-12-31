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

#ifndef __SAIM_STRING_H__
#define __SAIM_STRING_H__

typedef struct {
	unsigned int length;
	char * data;
} saim_string;

//! Function to calculate hash from string buffer
unsigned int saim_hash(const char* buffer);
unsigned int saim_string_hash(const saim_string * str);

void saim_string_create(saim_string * str);
void saim_string_destroy(saim_string * str);

void saim_string_set_text(saim_string * str, const char* text);
void saim_string_set(saim_string * str, const saim_string * other);
void saim_string_resize(saim_string * str, unsigned int new_length);
void saim_string_append(saim_string * str, const char* buffer, unsigned int length);
void saim_string_swap(saim_string * dst, saim_string * src);

int saim_string_compare(const saim_string * str1, const saim_string * str2);
void saim_string_copy(saim_string * dest, const saim_string * src);

#endif