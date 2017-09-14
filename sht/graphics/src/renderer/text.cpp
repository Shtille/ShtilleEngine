#include "../../include/renderer/text.h"
#include <cstdarg>
#include <cwchar>

#ifdef TARGET_MAC
#define my_vswprintf vswprintf
#else
#define my_vswprintf _vsnwprintf
#endif

namespace sht {
    namespace graphics {
        
        Text::Text(Renderer * renderer, u32 buffer_size)
        : renderer_(renderer)
        , font_(nullptr)
        , vertex_format_(nullptr)
        , vertex_buffer_(nullptr)
        , vertex_array_object_(0)
        , num_vertices_(0)
        , vertices_array_(nullptr)
        {
            text_buffer_size_ = buffer_size;
            text_buffer_ = new wchar_t[text_buffer_size_];
        }
        Text::~Text()
        {
            if (vertex_format_)
                renderer_->DeleteVertexFormat(vertex_format_);
            if (vertex_buffer_)
                renderer_->DeleteVertexBuffer(vertex_buffer_);
            if (vertex_array_object_)
                renderer_->context()->DeleteVertexArrayObject(vertex_array_object_);
            if (text_buffer_)
                delete[] text_buffer_;
            FreeArrays();
        }
        void Text::FreeArrays()
        {
            if (vertices_array_)
            {
                delete [] vertices_array_;
                vertices_array_ = nullptr;
            }
        }
        const size_t Text::GetVerticesPerPrimitive() const
        {
            return 6;
        }
        bool Text::SetTextInternal(Font * font, float x, float y, float scale)
        {
            // Store text parameters (need it for bounding box)
            font_ = font;
            scale_ = scale;
            reference_x_ = x;
            reference_y_ = y;
            
            // Make sure that text is not tend to overflow
            text_buffer_[text_buffer_size_-1] = L'\0';
            
            AllocateBuffer();
            
            FontGlyphPoint * vertex_buffer = reinterpret_cast<FontGlyphPoint*>(LockBuffer());
            
            // Coefficient to transform from bitmap space to screen space
            const float kB2S = (scale/font->font_height());
            
            // Coefficients to transform from screen space to clip space
            const float kS2CX = (2.0f/renderer_->aspect_ratio());
            const float kS2CY = (2.0f);
            
            // Transform offset coordinates x and y to clip space
            float offset_x = -1.0f + x * kS2CX;
            float offset_y = -1.0f + y * kS2CY;
            
            int index = 0;
            for (const wchar_t* p = text_buffer_; *p != L'\0'; ++p)
            {
                if (*p == L'\n') // new line character
                {
                    offset_x = -1.0f + x * kS2CX;
                    offset_y -= kS2CY * scale;
                    continue;
                }
                
                // Character is already in UTF, so we dont need any traslation
                const FontCharInfo* info = font->info(*p);
                
                // Our font doesnt present this character
                if (info == nullptr)
                    continue;
                
                // Calculate the vertex and texture coordinates
                float glyph_size_x = kS2CX * kB2S * info->bitmap_width;
                float glyph_size_y = kS2CY * kB2S * info->bitmap_height;
                
                float left = kS2CX * kB2S * info->bitmap_left;
                float top  = kS2CY * kB2S * info->bitmap_top;
                
                float glyph_x = offset_x + left;
                float glyph_y = offset_y + top - glyph_size_y;
                
                float texcoord_right = info->texcoord_x + info->bitmap_width/font->atlas_width();
                float texcoord_top = info->texcoord_y + info->bitmap_height/font->atlas_height();
                
                // Advance the cursor to the start of the next character
                offset_x += kS2CX * kB2S * info->advance_x;
                offset_y += kS2CY * kB2S * info->advance_y;
                
                // Lower left
                vertex_buffer[index].position_x = glyph_x;
                vertex_buffer[index].position_y = glyph_y;
                vertex_buffer[index].texcoord_x = info->texcoord_x;
                vertex_buffer[index].texcoord_y = texcoord_top;
                ++index;
                
                // Lower right
                vertex_buffer[index].position_x = glyph_x + glyph_size_x;
                vertex_buffer[index].position_y = glyph_y;
                vertex_buffer[index].texcoord_x = texcoord_right;
                vertex_buffer[index].texcoord_y = texcoord_top;
                ++index;
                
                // Upper left
                vertex_buffer[index].position_x = glyph_x;
                vertex_buffer[index].position_y = glyph_y + glyph_size_y;
                vertex_buffer[index].texcoord_x = info->texcoord_x;
                vertex_buffer[index].texcoord_y = info->texcoord_y;
                ++index;
                
                // Upper left (same as previous)
                vertex_buffer[index] = vertex_buffer[index-1];
                ++index;
                
                // Lower right
                vertex_buffer[index] = vertex_buffer[index-3];
                ++index;
                
                // Upper right
                vertex_buffer[index].position_x = glyph_x + glyph_size_x;
                vertex_buffer[index].position_y = glyph_y + glyph_size_y;
                vertex_buffer[index].texcoord_x = texcoord_right;
                vertex_buffer[index].texcoord_y = info->texcoord_y;
                ++index;
            }
            
            UnlockBuffer();
            renderer_->context()->CheckForErrors();
            
            return true;
        }
        bool Text::MakeRenderable()
        {
            VertexAttribute attribs[] = { {VertexAttribute::kVertex, 4} };
            renderer_->AddVertexFormat(vertex_format_, &attribs[0], _countof(attribs));
            renderer_->context()->CheckForErrors();
            
            renderer_->context()->GenVertexArrayObject(vertex_array_object_);
            renderer_->context()->BindVertexArrayObject(vertex_array_object_);
            renderer_->context()->CheckForErrors();
            
            AllocateVertexBuffer();
            renderer_->context()->CheckForErrors();
            if (vertex_buffer_ == nullptr) return false;
            
            // Free all data in memory
            FreeArrays();
            
            // There is only one attribute
            const char* base = (char*)0;
            {
            renderer_->context()->VertexAttribPointer(0, vertex_format_->generic_[0].size, DataType::kFloat, vertex_format_->vertex_size(), base + vertex_format_->generic_[0].offset);
                renderer_->context()->CheckForErrors();
            renderer_->context()->EnableVertexAttribArray(0);
                renderer_->context()->CheckForErrors();
            }
            
            renderer_->context()->BindVertexArrayObject(0);
            renderer_->context()->CheckForErrors();
            
            return true;
        }
        void Text::Render()
        {
            renderer_->ChangeTexture(font_->texture());
            renderer_->context()->BindVertexArrayObject(vertex_array_object_);
            renderer_->context()->DrawArrays(PrimitiveType::kTriangles, 0, num_vertices_);
            renderer_->context()->BindVertexArrayObject(0);
        }
        void Text::GetTextBoundingBox(float* min_x, float* min_y, float* max_x, float* max_y)
        {
            assert(font_);
            
            // Some start values
            *min_x = 1e7;
            *min_y = 1e7;
            *max_x = -1e7;
            *max_y = -1e7;
            
            // Coefficient to transform from bitmap space to screen space
            const float kB2S = (scale_/font_->font_height());
            
            float offset_x = reference_x_;
            float offset_y = reference_y_;
            
            for (const wchar_t* p = text_buffer_; *p != L'\0'; ++p)
            {
                if (*p == L'\n') // new line character
                {
                    offset_x = reference_x_;
                    offset_y -= scale_;
                    continue;
                }
                
                const FontCharInfo* info = font_->info(*p);
                
                if (info == nullptr)
                    continue;
                
                float glyph_size_x = kB2S * info->bitmap_width;
                float glyph_size_y = kB2S * info->bitmap_height;
                
                float left = kB2S * info->bitmap_left;
                float top  = kB2S * info->bitmap_top;
                
                float glyph_x = offset_x + left;
                float glyph_y = offset_y + top - glyph_size_y;

                // Advance the cursor to the start of the next character
                offset_x += kB2S * info->advance_x;
                offset_y += kB2S * info->advance_y;
                
                if (glyph_x < *min_x)
                    *min_x = glyph_x;
                
                if (glyph_y < *min_y)
                    *min_y = glyph_y;
                
                if (glyph_x + glyph_size_x > *max_x)
                    *max_x = glyph_x + glyph_size_x;
                
                if (glyph_y + glyph_size_y > *max_y)
                    *max_y = glyph_y + glyph_size_y;
            }
        }
        void Text::SetPosition(const vec2& position)
        {
            SetTextInternal(font_, position.x, position.y, scale_);
        }
        StaticText::StaticText(Renderer * renderer)
        : Text(renderer, 256)
        {
            
        }
        StaticText::~StaticText()
        {
            
        }
        StaticText * StaticText::Create(Renderer * renderer, Font * font, float scale,
                                   float x, float y, const wchar_t* str, ...)
        {
            StaticText * text = new StaticText(renderer);
            
            // Transform arguments
            va_list ap;
            va_start(ap, str);
            int written = my_vswprintf(text->text_buffer_, text->text_buffer_size_, str, ap);
            va_end(ap);
            if (written == -1) // error
            {
                delete text;
                return nullptr;
            }
            
            if (text->SetTextInternal(font, x, y, scale) &&
                text->MakeRenderable())
            {
                return text;
            }
            else
            {
                delete text;
                return nullptr;
            }
        }
        void StaticText::AllocateVertexBuffer()
        {
            renderer_->AddVertexBuffer(vertex_buffer_, num_vertices_ * vertex_format_->vertex_size(), vertices_array_, BufferUsage::kStaticDraw);
        }
        void StaticText::AllocateBuffer()
        {
            assert(vertices_array_ == nullptr);
            // We wont modify text, so use minimum memory for data
            size_t text_length = wcslen(text_buffer_);
            vertices_array_ = new u8[text_length * GetVerticesPerPrimitive() * sizeof(FontGlyphPoint)];
            num_vertices_ = static_cast<u32>(GetVerticesPerPrimitive() * text_length);
        }
        void* StaticText::LockBuffer()
        {
            return vertices_array_;
        }
        void StaticText::UnlockBuffer()
        {
            
        }
        DynamicText::DynamicText(Renderer * renderer, u32 buffer_size)
        : Text(renderer, buffer_size)
        {
            num_vertices_ = static_cast<u32>(GetVerticesPerPrimitive() * text_buffer_size_);
        }
        DynamicText::~DynamicText()
        {
            
        }
        DynamicText * DynamicText::Create(Renderer * renderer, u32 buffer_size)
        {
            DynamicText * text = new DynamicText(renderer, buffer_size);
            
            if (text->MakeRenderable())
            {
                return text;
            }
            else
            {
                delete text;
                return nullptr;
            }
        }
        void DynamicText::AppendSymbol(wchar_t symbol)
        {
            size_t len = wcslen(text_buffer_);
            if (len + 1 < text_buffer_size_)
            {
                text_buffer_[len++] = symbol;
                text_buffer_[len] = L'\0';
                SetTextInternal(font_, reference_x_, reference_y_, scale_);
            }
        }
        bool DynamicText::SetText(Font * font, float x, float y, float scale, const wchar_t* str, ...)
        {
            // Transform arguments
            va_list ap;
            va_start(ap, str);
            int written = my_vswprintf(text_buffer_, text_buffer_size_, str, ap);
            va_end(ap);
            if (written == -1) // error
            {
                assert(!"Error during character conversion.");
                return false;
            }
            return SetTextInternal(font, x, y, scale);
        }
        bool DynamicText::SetTextSimple(Font * font, float x, float y, float scale, const wchar_t* str)
        {
            wcsncpy(text_buffer_, str, text_buffer_size_);
            return SetTextInternal(font, x, y, scale);
        }
        void DynamicText::AllocateVertexBuffer()
        {
            renderer_->AddVertexBuffer(vertex_buffer_, num_vertices_ * vertex_format_->vertex_size(), nullptr, BufferUsage::kDynamicDraw);
        }
        void DynamicText::AllocateBuffer()
        {
            assert(vertex_buffer_);
            size_t text_length = wcslen(text_buffer_);
            num_vertices_ = static_cast<u32>(GetVerticesPerPrimitive() * text_length);
        }
        void* DynamicText::LockBuffer()
        {
            if (vertex_buffer_)
            {
                renderer_->context()->BindVertexArrayObject(0);
                vertex_buffer_->Bind();
                return vertex_buffer_->Lock(DataAccessType::kWrite);
            }
            else
                return vertices_array_;
        }
        void DynamicText::UnlockBuffer()
        {
            if (vertex_buffer_)
            {
                vertex_buffer_->Unlock();
                renderer_->context()->CheckForErrors();
                vertex_buffer_->Unbind();
            }
        }
        
    } // namespace graphics
} // namespace sht

#undef my_vswprintf