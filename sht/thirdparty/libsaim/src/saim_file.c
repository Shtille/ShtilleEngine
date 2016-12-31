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

#include "saim_file.h"

#include "util/saim_memory.h"

#include <string.h>

saim_file * saim_file__alloc(const char* filename)
{
	saim_file * file;
	file = (saim_file *)SAIM_MALLOC(sizeof(saim_file));
	saim_file__create(file, filename);
	return file;
}
void saim_file__free(saim_file * file)
{
	saim_file__destroy(file);
	SAIM_FREE(file);
}
void saim_file__create(saim_file * file, const char* filename)
{
	file->file = 0;
	file->filename = SAIM_STRDUP(filename);
	file->operation_successful = false;
	file->was_open_for_write = false;
}
void saim_file__destroy(saim_file * file)
{
	SAIM_FREE(file->filename);
	if (file->file)
	{
		fclose(file->file);
		file->file = 0;
	}
}
bool saim_file__open_for_write(saim_file * file)
{
	if (file->file) // opened
    {
        // Our main mode is read, so we gonna close the file
        fclose(file->file);
        file->was_open_for_write = false;
    }
    if ((file->file = fopen(file->filename, "wb")))
    {
        file->was_open_for_write = true;
        file->operation_successful = true;
        return true;
    }
    else
    {
        fprintf(stderr, "saim: fopen failed: %s\r\n", file->filename);
        return false;
    }
}
bool saim_file__open_for_read(saim_file * file)
{
	if (!file->file)
	{
	    if ((file->file = fopen(file->filename, "rb+"))) // we will also use this option to update the file
	    {
	        file->operation_successful = true;
	        return true;
	    }
	    else
	    {
	        fprintf(stderr, "saim: fopen failed: %s\r\n", file->filename);
	        return false;
	    }
	}
	else
	{
	    // File is already open for reading
	    saim_file__rewind(file);
	    return true;
	}
}
void saim_file__rewind(saim_file * file)
{
	rewind(file->file);
}
void saim_file__close(saim_file * file)
{
	if (file->file)
	{
		if (file->was_open_for_write)
		{
			fclose(file->file);
			file->file = NULL;
		}
	}
	file->was_open_for_write = false;
}
void saim_file__offset_from_beginning(saim_file * file, long offset)
{
	if (0 != fseek(file->file, offset, SEEK_SET))
	{
		fprintf(stderr, "saim: fseek failed\n");
		file->operation_successful = false;
	}
}
void saim_file__offset_from_current(saim_file * file, long offset)
{
	if (0 != fseek(file->file, offset, SEEK_CUR))
	{
		fprintf(stderr, "saim: fseek failed\n");
		file->operation_successful = false;
	}
}
void saim_file__offset_from_end(saim_file * file, long offset)
{
	if (0 != fseek(file->file, offset, SEEK_END))
	{
		fprintf(stderr, "saim: fseek failed\n");
		file->operation_successful = false;
	}
}
file_offset_t saim_file__tell(saim_file * file)
{
	return (file_offset_t)ftell(file->file);
}
void saim_file__write_byte(saim_file * file, unsigned char x)
{
	if (0 == fwrite(&x, sizeof(x), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->operation_successful = false;
	}
}
void saim_file__write_int(saim_file * file, int x)
{
	if (0 == fwrite(&x, sizeof(x), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->operation_successful = false;
	}
}
void saim_file__write_uint(saim_file * file, unsigned int x)
{
	if (0 == fwrite(&x, sizeof(x), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->operation_successful = false;
	}
}
void saim_file__write_array(saim_file * file, const void *data, unsigned int size)
{
	if (0 == fwrite(data, size, 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->operation_successful = false;
	}
}
void saim_file__write_string(saim_file * file, const saim_string* str)
{
	if (0 == fwrite(&str->length, sizeof(unsigned int), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->operation_successful = false;
	}
	if (0 == fwrite(str->data, str->length * sizeof(char), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->operation_successful = false;
	}
}
void saim_file__read_byte(saim_file * file, unsigned char *x)
{
	if (0 == fread(x, sizeof(unsigned char), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->operation_successful = false;
	}
}
void saim_file__read_int(saim_file * file, int *x)
{
	if (0 == fread(x, sizeof(int), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->operation_successful = false;
	}
}
void saim_file__read_uint(saim_file * file, unsigned int *x)
{
	if (0 == fread(x, sizeof(unsigned int), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->operation_successful = false;
	}
}
void saim_file__read_array(saim_file * file, void *data, unsigned int size)
{
	if (0 == fread(data, size, 1, file->file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->operation_successful = false;
	}
}
void saim_file__read_string(saim_file * file, saim_string *str)
{
	unsigned int length;
	if (0 == fread(&length, sizeof(unsigned int), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->operation_successful = false;
	}
	saim_string_resize(str, length);
	if (0 == fread(str->data, str->length * sizeof(char), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->operation_successful = false;
	}
}