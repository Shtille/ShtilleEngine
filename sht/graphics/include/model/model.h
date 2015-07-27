#pragma once
#ifndef __SHT_GRAPHICS_MODEL_H__
#define __SHT_GRAPHICS_MODEL_H__

#include "../renderer/vertex_format.h"

namespace sht {
    namespace graphics {
        
        // Forward declarations
        class Renderer;
        class VertexFormat;
        class VertexBuffer;
        class IndexBuffer;
        
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
        };
        
    }
}

#endif