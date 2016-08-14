#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_DATA_PROVIDER_INTERFACE_H__
#define __SHT_SATELLITE_IMAGERY_DATA_PROVIDER_INTERFACE_H__

#include "../../../common/non_copyable.h"
#include "../key.h"

#include <cmath>
#include <string>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

namespace sht {
    namespace satellite_imagery {
    
        //! Data provider class interface
        class DataProviderInterface : public NonCopyable {
        public:
            //! Converts data key to URL
            // @param[in] key data key
            // @return URL
            virtual std::string KeyToUrl(const DataKey& key) = 0;

            //! String used to make an unique hash value for storage file
            virtual std::string HashName() = 0;

            virtual const double GetMaxLatitude() { return  85.05112878; }
            virtual const double GetMinLatitude() { return -85.05112878; }
            virtual const double GetMaxLongitude() { return  180.0; }
            virtual const double GetMinLongitude() { return -180.0; }

            virtual const int GetMinLod() { return 0; }
            virtual const int GetMaxLod() { return 15; }

            virtual const int GetBitmapWidth() { return 256; }
            virtual const int GetBitmapHeight() { return 256; }

            //! Determines the map width and height (in pixels) at a specified level of detail
            // @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
            // @return The map width and height in pixels.
            int MapSize(int level_of_detail)
            {
                return GetBitmapWidth() << level_of_detail;
            }
            //! Converts a point from latitude/longitude WGS-84 coordinates (in degrees)
            // into tile XY at a specified level of detail.
            // @param[in] latitude Latitude of the point, in degrees.
            // @param[in] longitude Longitude of the point, in degrees.
            // @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
            // @param[out] tile_x Output parameter receiving the X coordinate of tile.
            // @param[out] tile_y Output parameter receiving the Y coordinate of tile.
            void LatLongToTileXY(double latitude, double longitude, int level_of_detail, 
                int& tile_x, int& tile_y)
            {
                LatLongToPixelXY(latitude, longitude, level_of_detail, tile_x, tile_y);
                tile_x /= GetBitmapWidth();
                tile_y /= GetBitmapHeight();
            }
            //! Converts a point from latitude/longitude WGS-84 coordinates (in degrees)
            // into pixel XY at a specified level of detail.
            // @param[in] latitude Latitude of the point, in degrees.
            // @param[in] longitude Longitude of the point, in degrees.
            // @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
            // @param[out] pixel_x Output parameter receiving the X coordinate of pixel.
            // @param[out] pixel_y Output parameter receiving the Y coordinate of pixel.
            void LatLongToPixelXY(double latitude, double longitude, int level_of_detail, 
                int& pixel_x, int& pixel_y)
            {
                latitude = Clip(latitude, GetMinLatitude(), GetMaxLatitude());
                longitude = Clip(longitude, GetMinLongitude(), GetMaxLongitude());

                double x = (longitude + 180.0) / 360.0;
                double sin_latitude = sin(latitude * M_PI / 180.0);
                double y = 0.5 - log((1.0 + sin_latitude) / (1.0 - sin_latitude)) / (4.0 * M_PI);

                int map_size = MapSize(level_of_detail);
                pixel_x = Clip((int)(x * (double)map_size + 0.5), 0, map_size - 1);
                pixel_y = Clip((int)(y * (double)map_size + 0.5), 0, map_size - 1);
            }
            void LongitudeToPixelX(double longitude, int map_size, int& pixel_x)
            {
                longitude = Clip(longitude, GetMinLongitude(), GetMaxLongitude());
                double x = (longitude + 180.0) / 360.0;
                pixel_x = Clip((int)(x * (double)map_size + 0.5), 0, map_size - 1);
            }
            void LatitudeToPixelY(double latitude, int map_size, int& pixel_y)
            {
                latitude = Clip(latitude, GetMinLatitude(), GetMaxLatitude());
                const double kOneOverFourPi = 1.0  / (4.0 * M_PI);
                double sin_latitude = sin(latitude * M_PI / 180.0);
                double y = 0.5 - log((1.0 + sin_latitude) / (1.0 - sin_latitude)) * kOneOverFourPi;
                pixel_y = Clip((int)(y * (double)map_size + 0.5), 0, map_size - 1);
            }
            void PixelXToLongitude(int pixel_x, int map_size, double& longitude)
            {
                double x = ((double)pixel_x / (double)map_size) - 0.5;
                longitude = 360.0 * x;
            }
            void PixelYToLatitude(int pixel_y, int map_size, double& latitude)
            {
                double y = 0.5 - ((double)pixel_y / (double)map_size);
                latitude = 90.0 - 360.0 * atan(exp(-y * 2.0 * M_PI)) / M_PI;
            }
            void LatitudeToPixelYApprox(double latitude, int map_size, int& pixel_y)
            {
                latitude = Clip(latitude, GetMinLatitude(), GetMaxLatitude()) * M_PI / 180.0;
                double x2 = latitude * latitude;
                double x3 = x2 * latitude;
                double log_x = 2.*latitude + (x3/3.) + (x2*x3/12.);
                double y = 0.5 - log_x / (4.0 * M_PI);
                pixel_y = Clip((int)(y * (double)map_size + 0.5), 0, map_size - 1);
            }
            void PixelXToTileX(int pixel_x, int& tile_x)
            {
                tile_x = pixel_x / GetBitmapWidth();
            }
            void PixelYToTileY(int pixel_y, int& tile_y)
            {
                tile_y = pixel_y / GetBitmapHeight();
            }
            void PixelXToTilePixelX(int pixel_x, int& tile_pixel_x)
            {
                tile_pixel_x = pixel_x % GetBitmapWidth();
            }
            void PixelYToTilePixelY(int pixel_y, int& tile_pixel_y)
            {
                tile_pixel_y = pixel_y % GetBitmapHeight();
            }
            //! Computes optimal level of detail (physically correct method)
            // @param[in] scale The map scale, expressed as the denominator N of the ratio 1 : N.
            // @param[in] latitude Latitude of the point, in degrees.
            // @param[in] pixels_per_cm Resolution of the screen, in pixels per centimeter
            // @return Optimal level of detail.
            int GetOptimalLevelOfDetail(double scale, double latitude, double pixels_per_cm)
            {
                const double kEarthRadius = 6378137.0;
                const double kInvLog2 = 1.442695040888963; // 1/ln(2)
                const double tile_size = static_cast<double>(GetBitmapWidth()); // pixels
                double ex = (cos(latitude * M_PI / 180.0) * (2.0 * M_PI * kEarthRadius) * 10.0) *
                    pixels_per_cm / (scale * tile_size);
                double log_value = log(ex) * kInvLog2;
                return Clip(static_cast<int>(log_value), GetMinLod(), GetMaxLod());
            }

            //! Computes optimal level of detail (logically correct method)
            // @param[in] screen_pixel_size_x Pixel size in degrees of one pixel in x direction.
            // @return Optimal level of detail
            int GetOptimalLevelOfDetail(double screen_pixel_size_x)
            {
                const double kInvLog2 = 1.442695040888963; // 1/ln(2)
                const double tile_size = static_cast<double>(GetBitmapWidth()); // pixels
                double ex = (360.0/tile_size)/screen_pixel_size_x;
                double log_value = log(ex) * kInvLog2;
                // To not do ceil we just use +1
                return Clip(static_cast<int>(log_value)+1, GetMinLod(), GetMaxLod());
            }
            static double ClippedLongitude(double longitude)
            {
                double clipped_longitude = longitude;
                while (clipped_longitude < -180.0)
                    clipped_longitude += 360.0;
                while (clipped_longitude >= 180.0)
                    clipped_longitude -= 360.0;
                return clipped_longitude;
            }

        private:

            template <typename T>
            T Clip(T x, T min_x, T max_x)
            {
                return std::min(std::max(x, min_x), max_x);
            }
            //! Determines the ground resolution (in meters per pixel) at a specified
            // latitude and level of detail.
            // @param[in] latitude Latitude (in degrees) at which to measure the ground resolution.
            // @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
            // @return The ground resolution, in meters per pixel.
            double GroundResolution(double latitude, int level_of_detail)
            {
                const double kEarthRadius = 6378137.0;
                latitude = Clip(latitude, GetMinLatitude(), GetMaxLatitude());
                return cos(latitude * M_PI / 180.0) * 2.0 * M_PI * kEarthRadius / MapSize(level_of_detail);
            }
            //! Determines the map scale at a specified latitude, level of detail,
            // and screen resolution.
            // @param[in] latitude Latitude (in degrees) at which to measure the map scale
            // @param[in] level_of_detail Level of detail, from 0 (lowest detail) to 15 (highest detail)
            // @param[in] screen_dpi Resolution of the screen, in dots per inch.
            // @return The map scale, expressed as the denominator N of the ratio 1 : N.
            double MapScale(double latitude, int level_of_detail, int screen_dpi)
            {
                return GroundResolution(latitude, level_of_detail) * screen_dpi / 0.0254;
            }
        };

    } // namespace satellite_imagery
} // namespace sht

#endif