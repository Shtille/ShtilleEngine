#pragma once
#ifndef __SHT_GRAPHICS_TEXT_H__
#define __SHT_GRAPHICS_TEXT_H__

#include "renderer.h"
#include "font.h"

namespace sht {
    namespace graphics {
        
        //! Standart text class
        class Text {
        public:
            Text(Renderer * renderer, u32 buffer_size);
            virtual ~Text();

            void Render();
            
            //! Returns text bounding box in screen coordinates ([0..a, 0..1])
            void GetTextBoundingBox(float* min_x, float* min_y, float* max_x, float* max_y);

            void SetPosition(const vec2& position);
            
        protected:
            bool SetTextInternal(Font * font, float x, float y, float scale);
            bool MakeRenderable();
            const size_t GetVerticesPerPrimitive() const;
            virtual void AllocateVertexBuffer() = 0;
            virtual void AllocateBuffer() = 0;
            virtual void* LockBuffer() = 0;
            virtual void UnlockBuffer() = 0;
            
            Renderer * renderer_;
            Font * font_;
            VertexFormat * vertex_format_;
            VertexBuffer * vertex_buffer_;
            u32 vertex_array_object_;
            
            u32 num_vertices_;
            u8 * vertices_array_;
            
            wchar_t * text_buffer_;
            size_t text_buffer_size_;
            
            // Store some positioning parameters to obtain text size
            f32 scale_;         //!< text scale (at 1 it has height as window)
            f32 reference_x_;   //!< text reference point x (in screen coordinates)
            f32 reference_y_;   //!< text reference point y (in screen coordinates)
            
        private:
            void FreeArrays();
        };
        
        //! Static text class
        class StaticText final : private Text {
        public:
            ~StaticText();
            
            static StaticText * Create(Renderer * renderer, Font * font, float scale,
                                       float x, float y, const wchar_t* str, ...);
            
            using Text::Render;
            using Text::GetTextBoundingBox;
            using Text::SetPosition;
            
        private:
            StaticText(Renderer * renderer);
            void AllocateVertexBuffer();
            void AllocateBuffer() final;
            void* LockBuffer() final;
            void UnlockBuffer() final;
        };
        
        //! Dynamic text class
        class DynamicText final : private Text {
        public:
            ~DynamicText();
            
            static DynamicText * Create(Renderer * renderer, u32 buffer_size);
            
            void AppendSymbol(wchar_t symbol);
            bool SetText(Font * font, float x, float y, float scale, const wchar_t* str, ...);
            bool SetTextSimple(Font * font, float x, float y, float scale, const wchar_t* str);
            
            using Text::Render;
            using Text::GetTextBoundingBox;
            using Text::SetPosition;
            
        private:
            DynamicText(Renderer * renderer, u32 buffer_size);
            void AllocateVertexBuffer();
            void AllocateBuffer() final;
            void* LockBuffer() final;
            void UnlockBuffer() final;
        };
        
    } // namespace graphics
} // namespace sht

#endif