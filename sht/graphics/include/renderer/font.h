#pragma once
#ifndef __SHT_GRAPHICS_FONT_H__
#define __SHT_GRAPHICS_FONT_H__

#include "../../../common/types.h"

namespace sht {
    namespace graphics {
        
        class Image;
        class Texture;
        
        //! Struct containing all offsets data
        struct FontCharInfo {
            float advance_x;
            float advance_y;
            float bitmap_width;
            float bitmap_height;
            float bitmap_left;
            float bitmap_top;
            float texcoord_x;
            float texcoord_y;
        };
        
        struct FontGlyphPoint {
            float position_x;
            float position_y;
            float texcoord_x;
            float texcoord_y;
        };
        
        //! Font class
        class Font {
            friend class Renderer;
            friend class OpenGlRenderer;
            
        public:
            const FontCharInfo* info(u32 charcode) const;
            const int atlas_width() const;
            const int atlas_height() const;
            Texture * texture() const;
            const float font_height() const;
            
            bool MakeAtlas(const char* filename, int font_height, Image * image);
            
        protected:
            Font();
            ~Font();
            Font(const Font&) = delete;
            void operator = (const Font&) = delete;
            
            static const int GetBmpSize();
            
        private:
            Texture * texture_;
            FontCharInfo * info_;
            u32 info_size_;
            float font_height_; //!< for atlas
        };
        
    } // namespace graphics
} // namespace sht

#endif