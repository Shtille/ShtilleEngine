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
            
        protected:
            bool SetTextInternal(Font * font, float x, float y, float scale);
            bool MakeRenderable();
            const size_t GetVerticesPerPrimitive() const;
            virtual void AllocateVertexBuffer() = 0;
            virtual void AllocateBuffer() = 0;
            virtual void UpdateBuffer() = 0;
            
            Renderer * renderer_;
            Font * font_;
            VertexFormat * vertex_format_;
            VertexBuffer * vertex_buffer_;
            u32 vertex_array_object_;
            
            u32 num_vertices_;
            u8 * vertices_array_;
            
            wchar_t * text_buffer_;
            size_t text_buffer_size_;
            
        private:
            void FreeArrays();
            
            // Store some positioning parameters to obtain text size
            f32 scale_;         //!< text scale
            f32 reference_x_;   //!< text reference point x (in screen coordinates)
            f32 reference_y_;   //!< text reference point y (in screen coordinates)
        };
        
        //! Static text class
        class StaticText final : private Text {
        public:
            ~StaticText();
            
            static StaticText * Create(Renderer * renderer, Font * font, float scale,
                                       float x, float y, const wchar_t* str, ...);
            
            using Text::Render;
            using Text::GetTextBoundingBox;
            
        private:
            StaticText(Renderer * renderer);
            void AllocateVertexBuffer();
            void AllocateBuffer() final;
            void UpdateBuffer() final;
        };
        
        //! Dynamic text class
        class DynamicText final : private Text {
        public:
            ~DynamicText();
            
            static DynamicText * Create(Renderer * renderer, u32 buffer_size);
            
            bool SetText(Font * font, float x, float y, float scale, const wchar_t* str, ...);
            
            using Text::Render;
            using Text::GetTextBoundingBox;
            
        private:
            DynamicText(Renderer * renderer, u32 buffer_size);
            void AllocateVertexBuffer();
            void AllocateBuffer() final;
            void UpdateBuffer() final;
        };
        
    } // namespace graphics
} // namespace sht

#endif