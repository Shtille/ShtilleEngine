#include "../../include/model/tetrahedron_model.h"

namespace sht {
    namespace graphics {
        
        TetrahedronModel::TetrahedronModel(Renderer * renderer)
        : Model(renderer)
        {
            primitive_mode_ = PrimitiveType::kTriangles;
        }
        TetrahedronModel::~TetrahedronModel()
        {
            
        }
        void TetrahedronModel::Create()
        {
            /* unique vertices
             (1, 0, 0)
             (0, 0, 1)
             (0, 0,-1)
             (0, 1, 0)
             */
            
            vertices_.resize(12);
            
            // +X side
            vertices_[0].position.Set(0.0f,  0.0f,  1.0f);
            vertices_[0].normal.Set(1.0f,  0.0f,  0.0f);
            vertices_[1].position.Set(1.0f,  0.0f,  0.0f);
            vertices_[1].normal.Set(1.0f,  0.0f,  0.0f);
            vertices_[2].position.Set(0.0f,  1.0f,  0.0f);
            vertices_[2].normal.Set(1.0f,  0.0f,  0.0f);
            
            // -Z side
            vertices_[3].position.Set( 1.0f,  0.0f,  0.0f);
            vertices_[3].normal.Set(0.0f,  0.0f, -1.0f);
            vertices_[4].position.Set( 0.0f,  0.0f, -1.0f);
            vertices_[4].normal.Set(0.0f,  0.0f, -1.0f);
            vertices_[5].position.Set( 0.0f,  1.0f,  0.0f);
            vertices_[5].normal.Set(0.0f,  0.0f, -1.0f);
            
            // -X side
            vertices_[6 ].position.Set( 0.0f,  0.0f, -1.0f);
            vertices_[6 ].normal.Set(-1.0f,  0.0f,  0.0f);
            vertices_[7 ].position.Set( 0.0f,  0.0f,  1.0f);
            vertices_[7 ].normal.Set(-1.0f,  0.0f,  0.0f);
            vertices_[8 ].position.Set( 0.0f,  1.0f,  0.0f);
            vertices_[8 ].normal.Set(-1.0f,  0.0f,  0.0f);
            
            // +Z side
            vertices_[9 ].position.Set( 0.0f,  0.0f,  1.0f);
            vertices_[9 ].normal.Set(0.0f,  0.0f,  1.0f);
            vertices_[10].position.Set( 0.0f,  0.0f, -1.0f);
            vertices_[10].normal.Set(0.0f,  0.0f,  1.0f);
            vertices_[11].position.Set( 1.0f,  0.0f,  0.0f);
            vertices_[11].normal.Set(0.0f,  0.0f,  1.0f);
            
            indices_.resize(12);
            indices_ = {
                0,1,2,
                3,4,5,
                6,7,8,
                9,10,11
            };
        }
        
    } // namespace graphics
} // namespace sht