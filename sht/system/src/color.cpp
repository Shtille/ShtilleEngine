#include "../include/color.h"

namespace sht {
    namespace system {
        
        RgbColor::RgbColor(const RgbColor& rgb_color)
        {
            red_ = rgb_color.red_;
            green_ = rgb_color.green_;
            blue_ = rgb_color.blue_;
        }
        RgbColor::RgbColor(const RgbaColor& rgba_color)
        {
            red_ = rgba_color.red_;
            green_ = rgba_color.green_;
            blue_ = rgba_color.blue_;
        }
        RgbColor::RgbColor(f32 red, f32 green, f32 blue)
        {
            red_ = red;
            green_ = green;
            blue_ = blue;
        }
        f32 RgbColor::red() const
        {
            return red_;
        }
        f32 RgbColor::green() const
        {
            return green_;
        }
        f32 RgbColor::blue() const
        {
            return blue_;
        }
        const float* RgbColor::AsArray() const
        {
            return &red_;
        }
        RgbaColor::RgbaColor(const RgbaColor& rgba_color)
        {
            red_ = rgba_color.red_;
            green_ = rgba_color.green_;
            blue_ = rgba_color.blue_;
            alpha_ = rgba_color.alpha_;
        }
        RgbaColor::RgbaColor(const RgbColor& rgb_color, f32 alpha)
        {
            red_ = rgb_color.red_;
            green_ = rgb_color.green_;
            blue_ = rgb_color.blue_;
            alpha_ = alpha;
        }
        RgbaColor::RgbaColor(f32 red, f32 green, f32 blue, f32 alpha)
        {
            red_ = red;
            green_ = green;
            blue_ = blue;
            alpha_ = alpha;
        }
        f32 RgbaColor::red() const
        {
            return red_;
        }
        f32 RgbaColor::green() const
        {
            return green_;
        }
        f32 RgbaColor::blue() const
        {
            return blue_;
        }
        const float* RgbaColor::AsArray() const
        {
            return &red_;
        }

    } // namespace system
} // namespace sht