#include "../../include/renderer/font.h"
#include "../../include/renderer/texture.h"
#include "../../include/image/image.h"
#include "../../../system/include/stream/log_stream.h"

#include "../../../thirdparty/freetype/include/ft2build.h"
#include FT_FREETYPE_H

#include <algorithm>

namespace sht {
    namespace graphics {
        
        Font::Font()
        : texture_(nullptr)
        {
            
        }
        Font::~Font()
        {
        }
        const FontCharInfo* Font::info(u32 charcode) const
        {
            auto it = info_map_.find(charcode);
            if (it != info_map_.end())
                return &(it->second);
            else
                return nullptr;
        }
        const int Font::atlas_width() const
        {
            return texture_->width_;
        }
        const int Font::atlas_height() const
        {
            return texture_->height_;
        }
        Texture * Font::texture() const
        {
            return texture_;
        }
        const float Font::font_height() const
        {
            return font_height_;
        }
        bool Font::MakeAtlas(const char* filename, int font_height, Image * image)
        {
            font_height_ = static_cast<float>(font_height);
            
            FT_Library ft;
            FT_Face face;
            
            // Initialize library
            if (FT_Init_FreeType(&ft))
            {
                fprintf(stderr, "FreeType library initialization failed");
                return false;
            }
            
            // Load a font
            if (FT_New_Face(ft, filename, 0, &face))
            {
                fprintf(stderr, "Failed to load a font %s!\n", filename);
				FT_Done_FreeType(ft);
                return false;
            }
            
            // Set encoding
            if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
            {
                fprintf(stderr, "Failed to set encoding\n");
				FT_Done_FreeType(ft);
                return false;
            }
            
            // Set font height in pixels
            FT_Set_Pixel_Sizes(face, 0, font_height);
            
            FT_GlyphSlot g = face->glyph;
            
            const unsigned int kMaxTextureWidth = 1024U;
            
            unsigned int w,h;
            unsigned int roww = 0;
            unsigned int rowh = 0;
            w = 0;
            h = 0;

            FT_ULong charcode;
            FT_UInt gindex;
            
            // Find minimum size for a texture holding all visible ASCII characters
            charcode = FT_Get_First_Char(face, &gindex);
            while (gindex != 0)
            {
                // Load glyph
                if (FT_Load_Char(face, charcode, FT_LOAD_RENDER)) {
                    fprintf(stderr, "Loading character %lu failed!\n", charcode);
                    continue;
                }
                if (roww + g->bitmap.width + 1 >= kMaxTextureWidth) {
                    w = std::max(w, roww);
                    h += rowh;
                    roww = 0;
                    rowh = 0;
                }
                roww += g->bitmap.width + 1;
                rowh = std::max(rowh, g->bitmap.rows);
                // Goto next charcode
                charcode = FT_Get_Next_Char(face, charcode, &gindex);
            }
            
            w = std::max(w, roww);
            h += rowh;
            
            if (w == 0 || h == 0)
            {
				FT_Done_FreeType(ft);
                return false;
            }
            
            // Create an image that will be used to hold all ASCII glyphs
            image->Allocate(w, h, Image::Format::kR8);
            image->FillWithZeroes();
            
            // Paste all glyph bitmaps into the image, remembering the offset
            int ox = 0;
            int oy = 0;
            
            rowh = 0;
            
            charcode = FT_Get_First_Char(face, &gindex);
            while (gindex != 0)
            {
                if (FT_Load_Char(face, charcode, FT_LOAD_RENDER)) {
                    fprintf(stderr, "Loading character %lu failed!\n", charcode);
                    continue;
                }
                
                if (ox + g->bitmap.width + 1 >= kMaxTextureWidth) {
                    oy += rowh;
                    rowh = 0;
                    ox = 0;
                }
                
                image->SubData(ox, oy, g->bitmap.width, g->bitmap.rows, g->bitmap.buffer);
                
                u32 i = static_cast<u32>(charcode);
                
                FontCharInfo & info = info_map_[i];

                info.advance_x = g->advance.x >> 6;
                info.advance_y = g->advance.y >> 6;
                
                info.bitmap_width = g->bitmap.width;
                info.bitmap_height = g->bitmap.rows;
                
                info.bitmap_left = g->bitmap_left;
                info.bitmap_top = g->bitmap_top;
                
                info.texcoord_x = ox / (float)w;
                info.texcoord_y = oy / (float)h;
                
                rowh = std::max(rowh, g->bitmap.rows);
                ox += g->bitmap.width + 1;
                
                // Goto next charcode
                charcode = FT_Get_Next_Char(face, charcode, &gindex);
            }
            
            FT_Done_Face(face);
            
            FT_Done_FreeType(ft);
            
            return true;
        }
        
    }
}