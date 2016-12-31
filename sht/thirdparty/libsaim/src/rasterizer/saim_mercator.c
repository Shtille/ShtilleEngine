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

#include "saim_mercator.h"

#include "../saim_provider.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

extern saim_provider * s_provider;

static int clip_int(int value, int min_value, int max_value)
{
	return (value < min_value) ? min_value : (value > max_value) ? max_value : value;
}
static double clip_double(double value, double min_value, double max_value)
{
	return (value < min_value) ? min_value : (value > max_value) ? max_value : value;
}
double saim_normalized_longitude(double longitude)
{
	double clipped_longitude = longitude;
	while (clipped_longitude < -180.0)
		clipped_longitude += 360.0;
	while (clipped_longitude >= 180.0)
		clipped_longitude -= 360.0;
	return clipped_longitude;
}
int saim_map_size(int level_of_detail)
{
	return s_provider->bitmap_width << level_of_detail;
}
void saim_lat_long_to_pixel_xy(double latitude, double longitude, int level_of_detail, 
	int* pixel_x, int* pixel_y)
{
	latitude = clip_double(latitude, s_provider->min_latitude, s_provider->max_latitude);
	longitude = clip_double(longitude, s_provider->min_longitude, s_provider->max_longitude);

	double x = (longitude + 180.0) / 360.0;
	double sin_latitude = sin(latitude * M_PI / 180.0);
	double y = 0.5 - log((1.0 + sin_latitude) / (1.0 - sin_latitude)) / (4.0 * M_PI);

	int map_size = saim_map_size(level_of_detail);
	*pixel_x = clip_int((int)(x * (double)map_size + 0.5), 0, map_size - 1);
	*pixel_y = clip_int((int)(y * (double)map_size + 0.5), 0, map_size - 1);
}
void saim_longitude_to_pixel_x(double longitude, int map_size, int* pixel_x)
{
	longitude = clip_double(longitude, s_provider->min_longitude, s_provider->max_longitude);
	double x = (longitude + 180.0) / 360.0;
	*pixel_x = clip_int((int)(x * (double)map_size + 0.5), 0, map_size - 1);
}
void saim_latitude_to_pixel_y(double latitude, int map_size, int* pixel_y)
{
	latitude = clip_double(latitude, s_provider->min_latitude, s_provider->max_latitude);
	const double kOneOverFourPi = 1.0  / (4.0 * M_PI);
	double sin_latitude = sin(latitude * M_PI / 180.0);
	double y = 0.5 - log((1.0 + sin_latitude) / (1.0 - sin_latitude)) * kOneOverFourPi;
	*pixel_y = clip_int((int)(y * (double)map_size + 0.5), 0, map_size - 1);
}
void saim_lat_long_to_tile_xy(double latitude, double longitude, int level_of_detail, 
	int* tile_x, int* tile_y)
{
	saim_lat_long_to_pixel_xy(latitude, longitude, level_of_detail, tile_x, tile_y);
	*tile_x /= s_provider->bitmap_width;
	*tile_y /= s_provider->bitmap_height;
}
int saim_get_optimal_level_of_detail(double screen_pixel_size_x)
{
	const double kInvLog2 = 1.442695040888963; // 1/ln(2)
	// To not do ceil we just use +1
	return clip_int((int)(log((360.0/(double)s_provider->bitmap_width)/screen_pixel_size_x) * kInvLog2) + 1,
		s_provider->min_lod, s_provider->max_lod);
}