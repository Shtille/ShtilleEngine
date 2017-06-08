#pragma once
#ifndef __SHT_GRAPHICS_COMPLEX_MESH_H__
#define __SHT_GRAPHICS_COMPLEX_MESH_H__

#include "../renderer/vertex_format.h"

#include <vector>

namespace sht {
    namespace graphics {

        class Renderer;
        class Mesh;
        
        //! Complex mesh class
        class ComplexMesh {
        public:

            enum class FileFormat {
                kUnknown,
                kObj
            };

            ComplexMesh(Renderer * renderer);
            virtual ~ComplexMesh();

            bool LoadFromFile(const char* filename);
            
            virtual void Create() = 0;
            void AddFormat(const VertexAttribute& attrib);
            bool MakeRenderable();
            
            void Render();
            
        protected:
            // Save routines
            bool SaveToFileObj(const char *filename);

            // Load routines
            bool LoadFromFileObj(const char *filename);
            
        private:            
            Renderer * renderer_;
            VertexFormat * vertex_format_;

            std::vector<VertexAttribute> attribs_;
            std::vector<Mesh*> meshes_;
        };
        
    }
}

#endif