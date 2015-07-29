#include "../../include/model/cube_model.h"

namespace sht {
    namespace graphics {
        
        CubeModel::CubeModel(Renderer * renderer)
        : Model(renderer)
        {
            VertexAttribute attribs[] = {
                {VertexAttribute::kVertex, 3}
            };
            SetFormat(attribs, _countof(attribs));
        }
        CubeModel::~CubeModel()
        {
            
        }
        void CubeModel::Create()
        {
            num_vertices_ = 8;
            // vertex size = 12
            vertices_ = new u8[num_vertices_ * vertex_size()];
            
            float * vertices = reinterpret_cast<float*>(vertices_);
            vertices[0] = 1.0f;
            vertices[1] = -1.0f;
            vertices[2] = 1.0f;
            
            vertices[3] = 1.0f;
            vertices[4] = -1.0f;
            vertices[5] = -1.0f;
            
            vertices[6] = -1.0f;
            vertices[7] = -1.0f;
            vertices[8] = -1.0f;
            
            vertices[9] = -1.0f;
            vertices[10] = -1.0f;
            vertices[11] = 1.0f;
            
            vertices[12] = 1.0f;
            vertices[13] = 1.0f;
            vertices[14] = 1.0f;
            
            vertices[15] = 1.0f;
            vertices[16] = 1.0f;
            vertices[17] = -1.0f;
            
            vertices[18] = -1.0f;
            vertices[19] = 1.0f;
            vertices[20] = -1.0f;
            
            vertices[21] = -1.0f;
            vertices[22] = 1.0f;
            vertices[23] = 1.0f;
            
            num_indices_ = 18;
            indices_ = new u8[num_indices_ * sizeof(u16)];
            u16 * indices = reinterpret_cast<u16*>(indices_);
            indices[0] = 4;
            indices[1] = 0;
            indices[2] = 5;
            indices[3] = 1;
            indices[4] = 6;
            indices[5] = 2;
            indices[6] = 7;
            indices[7] = 3;
            indices[8] = 4;
            indices[9] = 0;
            indices[10] = 4;
            indices[11] = 5;
            indices[12] = 7;
            indices[13] = 6;
            indices[14] = 1;
            indices[15] = 0;
            indices[16] = 2;
            indices[17] = 3;
        }
        
    } // namespace graphics
} // namespace sht