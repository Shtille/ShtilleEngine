#pragma once
#ifndef __SHT_GRAPHICS_MODEL_H__
#define __SHT_GRAPHICS_MODEL_H__

#include "vertex.h"
#include "../renderer/vertex_format.h"
#include "../renderer/renderer.h"
#include <vector>

namespace sht {
    namespace graphics {
        
        //! Standart model class
        class Model {
        public:
            Model(Renderer * renderer);
            virtual ~Model();
            
            virtual void Create() = 0;
            void AddFormat(const VertexAttribute& attrib);
            bool MakeRenderable();
            
            void Render();
            
        protected:            
            std::vector<Vertex> vertices_;
            std::vector<u32> indices_;
            
            PrimitiveType primitive_mode_;
            
        private:
            void FreeArrays();
            void TransformVertices();
            
            Renderer * renderer_;
            VertexFormat * vertex_format_;
            VertexBuffer * vertex_buffer_;
            IndexBuffer * index_buffer_;
            u32 vertex_array_object_;
            
            u32 num_vertices_;
            u8 * vertices_array_;
            u32 num_indices_;
            u32 index_size_;
            u8 * indices_array_;
            
            std::vector<VertexAttribute> attribs_;
        };
        
    }
}

#endif