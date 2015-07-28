#pragma once
#ifndef __SHT_GRAPHICS_MODEL_H__
#define __SHT_GRAPHICS_MODEL_H__

#include "../renderer/vertex_format.h"
#include "../renderer/renderer.h"

namespace sht {
    namespace graphics {
        
        //! Standart model class
        class Model {
        public:
            Model(Renderer * renderer);
            virtual ~Model();
            
            void SetFormat(VertexAttribute *attribs, u32 num_attribs);
            
            void Render();
            
        protected:
            Renderer * renderer_;
            VertexFormat * vertex_format_;
            VertexBuffer * vertex_buffer_;
            IndexBuffer * index_buffer_;
            
            u32 num_vertices_;
            u32 vertex_size_;
            u8 * vertices_;
            u32 num_indices_;
            u32 index_size_;
            u8 * indices_;
            
            PrimitiveType primitive_mode_;
        };
        
    }
}

#endif