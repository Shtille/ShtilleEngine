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

#ifndef __SAIM_MERCATOR_H__
#define __SAIM_MERCATOR_H__

//! Transforms input longitude to [-180; +180] range
// @param[in] longitude Longitude of the point, in degrees.
// @return Normalized longitude.
double saim_normalized_longitude(double longitude);

//! Determines the map width and height (in pixels) at a specified level of detail
// @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
// @return The map width and height in pixels.
int saim_map_size(int level_of_detail);

//! Converts a point from latitude/longitude WGS-84 coordinates (in degrees)
// into pixel XY at a specified level of detail.
// @param[in] latitude Latitude of the point, in degrees.
// @param[in] longitude Longitude of the point, in degrees.
// @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
// @param[out] pixel_x Output parameter receiving the X coordinate of pixel.
// @param[out] pixel_y Output parameter receiving the Y coordinate of pixel.
void saim_lat_long_to_pixel_xy(double latitude, double longitude, int level_of_detail, 
	int* pixel_x, int* pixel_y);

//! Converts a point longitude WGS-84 coordinates (in degrees)
// into pixel X at a specified level of detail.
// @param[in] longitude Longitude of the point, in degrees.
// @param[in] map_size The map width and height in pixels.
// @param[out] pixel_x Output parameter receiving the X coordinate of pixel.
void saim_longitude_to_pixel_x(double longitude, int map_size, int* pixel_x);

//! Converts a point from latitude WGS-84 coordinates (in degrees)
// into pixel Y at a specified level of detail.
// @param[in] latitude Latitude of the point, in degrees.
// @param[in] map_size The map width and height in pixels.
// @param[out] pixel_y Output parameter receiving the Y coordinate of pixel.
void saim_latitude_to_pixel_y(double latitude, int map_size, int* pixel_y);

//! Converts a point from latitude/longitude WGS-84 coordinates (in degrees)
// into tile XY at a specified level of detail.
// @param[in] latitude Latitude of the point, in degrees.
// @param[in] longitude Longitude of the point, in degrees.
// @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
// @param[out] tile_x Output parameter receiving the X coordinate of tile.
// @param[out] tile_y Output parameter receiving the Y coordinate of tile.
void saim_lat_long_to_tile_xy(double latitude, double longitude, int level_of_detail, 
	int* tile_x, int* tile_y);

//! Computes optimal level of detail (logically correct method)
// @param[in] screen_pixel_size_x Pixel size in degrees of one pixel in x direction.
// @return Optimal level of detail
int saim_get_optimal_level_of_detail(double screen_pixel_size_x);

#endif