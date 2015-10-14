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
        RgbColor RgbColor::Mix(const RgbColor& color1, const RgbColor& color2, f32 t)
        {
            f32 u = 1.0f - t;
            RgbColor color(color1.red()   * u + color2.red()   * t,
                           color1.green() * u + color2.green() * t,
                           color1.blue()  * u + color2.blue()  * t);
            return color;
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
        f32 RgbaColor::alpha() const
        {
            return alpha_;
        }
        const float* RgbaColor::AsArray() const
        {
            return &red_;
        }
        RgbaColor RgbaColor::Mix(const RgbaColor& color1, const RgbaColor& color2, f32 t)
        {
            f32 u = 1.0f - t;
            RgbaColor color(color1.red()   * u + color2.red()   * t,
                            color1.green() * u + color2.green() * t,
                            color1.blue()  * u + color2.blue()  * t,
                            color1.alpha() * u + color2.alpha() * t);
            return color;
        }
        PackedRgbColor::PackedRgbColor(const PackedRgbColor& rgb_color)
        {
            data_ = rgb_color.data_;
        }
        PackedRgbColor::PackedRgbColor(const PackedRgbaColor& rgba_color)
        {
            data_ = rgba_color.data_;
            data_ |= 0xFF000000; // make color opaque
        }
        PackedRgbColor::PackedRgbColor(const RgbColor& rgb_color)
        {
            data_ = 0xFF000000;
            u32 red   = static_cast<u32>(rgb_color.red() * 255.0f);
            u32 green = static_cast<u32>(rgb_color.green() * 255.0f);
            u32 blue  = static_cast<u32>(rgb_color.blue() * 255.0f);
            data_ |= blue << 16;
            data_ |= green << 8;
            data_ |= red;
        }
        PackedRgbColor::PackedRgbColor(const RgbaColor& rgba_color)
        {
            data_ = 0xFF000000;
            u32 red   = static_cast<u32>(rgba_color.red() * 255.0f);
            u32 green = static_cast<u32>(rgba_color.green() * 255.0f);
            u32 blue  = static_cast<u32>(rgba_color.blue() * 255.0f);
            data_ |= blue << 16;
            data_ |= green << 8;
            data_ |= red;
        }
        f32 PackedRgbColor::red() const
        {
            return static_cast<f32>(data_ & 0x0000FF) / 255.0f;
        }
        f32 PackedRgbColor::green() const
        {
            return static_cast<f32>(data_ & 0x00FF00) / 255.0f;
        }
        f32 PackedRgbColor::blue() const
        {
            return static_cast<f32>(data_ & 0xFF0000) / 255.0f;
        }
        RgbColor PackedRgbColor::Unpack()
        {
            return RgbColor(red(), green(), blue());
        }
        PackedRgbColor PackedRgbColor::Mix(const PackedRgbColor& color1, const PackedRgbColor& color2, f32 t)
        {
            f32 u = 1.0f - t;
            RgbColor color(color1.red()   * u + color2.red()   * t,
                           color1.green() * u + color2.green() * t,
                           color1.blue()  * u + color2.blue()  * t);
            return color;
        }
        PackedRgbaColor::PackedRgbaColor(const PackedRgbaColor& rgba_color)
        {
            data_ = rgba_color.data_;
        }
        PackedRgbaColor::PackedRgbaColor(const PackedRgbColor& rgb_color)
        {
            data_ = rgb_color.data_;
        }
        PackedRgbaColor::PackedRgbaColor(const RgbColor& rgb_color)
        {
            data_ = 0xFF000000;
            u32 red   = static_cast<u32>(rgb_color.red() * 255.0f);
            u32 green = static_cast<u32>(rgb_color.green() * 255.0f);
            u32 blue  = static_cast<u32>(rgb_color.blue() * 255.0f);
            data_ |= blue << 16;
            data_ |= green << 8;
            data_ |= red;
        }
        PackedRgbaColor::PackedRgbaColor(const RgbaColor& rgba_color)
        {
            u32 red   = static_cast<u32>(rgba_color.red() * 255.0f);
            u32 green = static_cast<u32>(rgba_color.green() * 255.0f);
            u32 blue  = static_cast<u32>(rgba_color.blue() * 255.0f);
            u32 alpha = static_cast<u32>(rgba_color.alpha() * 255.0f);
            data_ = alpha << 24;
            data_ |= blue << 16;
            data_ |= green << 8;
            data_ |= red;
        }
        f32 PackedRgbaColor::red() const
        {
            return static_cast<f32>(data_ & 0x000000FF) / 255.0f;
        }
        f32 PackedRgbaColor::green() const
        {
            return static_cast<f32>(data_ & 0x0000FF00) / 255.0f;
        }
        f32 PackedRgbaColor::blue() const
        {
            return static_cast<f32>(data_ & 0x00FF0000) / 255.0f;
        }
        f32 PackedRgbaColor::alpha() const
        {
            return static_cast<f32>(data_ & 0xFF000000) / 255.0f;
        }
        RgbaColor PackedRgbaColor::Unpack()
        {
            return RgbaColor(red(), green(), blue(), alpha());
        }
        PackedRgbaColor PackedRgbaColor::Mix(const PackedRgbaColor& color1, const PackedRgbaColor& color2, f32 t)
        {
            f32 u = 1.0f - t;
            RgbaColor color(color1.red()   * u + color2.red()   * t,
                            color1.green() * u + color2.green() * t,
                            color1.blue()  * u + color2.blue()  * t,
                            color1.alpha() * u + color2.alpha() * t);
            return color;
        }

    } // namespace system
} // namespace sht