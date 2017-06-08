#pragma once
#ifndef __SHT_GRAPHICS_MESH_H__
#define __SHT_GRAPHICS_MESH_H__

#include "vertex.h"
#include "../renderer/renderer.h"
#include <vector>

namespace sht {
    namespace graphics {

        class Material;
        
        //! Standart mesh class
        class Mesh {
        public:
            Mesh(Renderer * renderer);
            virtual ~Mesh();

            void SetMaterial(Material * material);
            
            virtual void Create() = 0;
            bool MakeRenderable(VertexFormat * vertex_format);
            
            void Render();

            void ScaleVertices(const math::Vector3& scale);
            void ScaleTexcoord(const math::Vector2& scale);
            
        protected:            
            std::vector<Vertex> vertices_;
            std::vector<u32> indices_;
            
            PrimitiveType primitive_mode_;
            
        private:
            void FreeArrays();
            void TransformVertices(VertexFormat * vertex_format);
            
            Renderer * renderer_;
            Material * material_;
            VertexBuffer * vertex_buffer_;
            IndexBuffer * index_buffer_;
            u32 vertex_array_object_;
            
            u32 num_vertices_;
            u8 * vertices_array_;
            u32 num_indices_;
            u32 index_size_;
            u8 * indices_array_;
            DataType index_data_type_;
        };
        
    }
}

#endif