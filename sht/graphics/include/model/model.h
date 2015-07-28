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
            
            virtual void Create() = 0;
            bool MakeRenderable();
            
            void Render();
            
        protected:
            void SetFormat(VertexAttribute *attribs, u32 num_attribs);
            u32 vertex_size() const;
            
            u32 num_vertices_;
            u8 * vertices_;
            u32 num_indices_;
            u8 * indices_;
            
            PrimitiveType primitive_mode_;
            
        private:
            void FreeArrays();
            
            Renderer * renderer_;
            VertexFormat * vertex_format_;
            VertexBuffer * vertex_buffer_;
            IndexBuffer * index_buffer_;
        };
        
    }
}

#endif