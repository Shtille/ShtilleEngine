#pragma once
#ifndef __SHT_SYSTEM_COLOR_H__
#define __SHT_SYSTEM_COLOR_H__

#include "../../common/types.h"

namespace sht {
    namespace system {
        
        // Forward declarations
        class RgbColor;
        class RgbaColor;
        class PackedRgbColor;
        class PackedRgbaColor;
        
        //! Single precision floating-point component RGB storage
        class RgbColor {
            friend class RgbaColor;
            
        public:
            RgbColor() = default;
            RgbColor(const RgbColor& rgb_color);
            RgbColor(const RgbaColor& rgba_color);
            RgbColor(f32 red, f32 green, f32 blue);
            
            f32 red() const;
            f32 green() const;
            f32 blue() const;
            
            const float* AsArray() const;
            
        private:
            f32 red_;
            f32 green_;
            f32 blue_;
        };
        
        //! Single precision floating-point component RGBA storage
        class RgbaColor {
            friend class RgbColor;
            
        public:
            RgbaColor() = default;
            RgbaColor(const RgbaColor& rgba_color);
            RgbaColor(const RgbColor& rgb_color, f32 alpha);
            RgbaColor(f32 red, f32 green, f32 blue, f32 alpha);
            
            f32 red() const;
            f32 green() const;
            f32 blue() const;
            
            const float* AsArray() const;
            
        private:
            f32 red_;
            f32 green_;
            f32 blue_;
            f32 alpha_;
        };
        
        //! An RGB color packing into 32 bit value, alpha byte is always 0xFF
        class PackedRgbColor {
        public:
            PackedRgbColor() = default;
            PackedRgbColor(const PackedRgbColor&);
            PackedRgbColor(const PackedRgbaColor&);
            PackedRgbColor(const RgbColor&);
            PackedRgbColor(const RgbaColor&);
            
        private:
            u32 data_;
        };
        
        //! An RGBA color packing into 32 bit value
        class PackedRgbaColor {
        public:
            PackedRgbaColor() = default;
            PackedRgbaColor(const PackedRgbaColor&);
            PackedRgbaColor(const PackedRgbColor&);
            PackedRgbaColor(const RgbColor&);
            PackedRgbaColor(const RgbaColor&);
            
        private:
            u32 data_;
        };
        
    } // namespace system
} // namespace sht

#endif