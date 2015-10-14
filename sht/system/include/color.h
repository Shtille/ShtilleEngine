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
            
            static RgbColor Mix(const RgbColor& color1, const RgbColor& color2, f32 t);
            
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
            f32 alpha() const;
            
            const float* AsArray() const;
            
            static RgbaColor Mix(const RgbaColor& color1, const RgbaColor& color2, f32 t);
            
        private:
            f32 red_;
            f32 green_;
            f32 blue_;
            f32 alpha_;
        };
        
        //! An RGB color packed into a 32 bit value, alpha byte is always 0xFF
        class PackedRgbColor {
            friend class PackedRgbaColor;
            
        public:
            PackedRgbColor() = default;
            PackedRgbColor(const PackedRgbColor& rgb_color);
            PackedRgbColor(const PackedRgbaColor& rgba_color);
            PackedRgbColor(const RgbColor& rgb_color);
            PackedRgbColor(const RgbaColor& rgba_color);
            
            f32 red() const;
            f32 green() const;
            f32 blue() const;
            
            RgbColor Unpack();
            
            static PackedRgbColor Mix(const PackedRgbColor& color1, const PackedRgbColor& color2, f32 t);
            
        private:
            u32 data_;
        };
        
        //! An RGBA color packed into a 32 bit value
        class PackedRgbaColor {
            friend class PackedRgbColor;
            
        public:
            PackedRgbaColor() = default;
            PackedRgbaColor(const PackedRgbaColor& rgba_color);
            PackedRgbaColor(const PackedRgbColor& rgb_color);
            PackedRgbaColor(const RgbColor& rgb_color);
            PackedRgbaColor(const RgbaColor& rgba_color);
            
            f32 red() const;
            f32 green() const;
            f32 blue() const;
            f32 alpha() const;
            
            RgbaColor Unpack();
            
            static PackedRgbaColor Mix(const PackedRgbaColor& color1, const PackedRgbaColor& color2, f32 t);
            
        private:
            u32 data_;
        };
        
    } // namespace system
} // namespace sht

#endif