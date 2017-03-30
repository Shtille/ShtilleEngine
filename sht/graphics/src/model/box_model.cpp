#include "../../include/model/box_model.h"

namespace sht {
    namespace graphics {
        
        BoxModel::BoxModel(Renderer * renderer, float size_x, float size_y, float size_z)
        : Model(renderer)
        , sizes_(size_x, size_y, size_z)
        {
        }
        BoxModel::~BoxModel()
        {
            
        }
        void BoxModel::Create()
        {
            vertices_.resize(24);
            
            // +X side
            vertices_[0].position.Set(1.0f,  1.0f,  1.0f);
            vertices_[0].normal.Set(1.0f,  0.0f,  0.0f);
            vertices_[0].texcoord.Set(0.0f,  1.0f);
            vertices_[1].position.Set(1.0f, -1.0f,  1.0f);
            vertices_[1].normal.Set(1.0f,  0.0f,  0.0f);
            vertices_[1].texcoord.Set(0.0f,  0.0f);
            vertices_[2].position.Set(1.0f,  1.0f, -1.0f);
            vertices_[2].normal.Set(1.0f,  0.0f,  0.0f);
            vertices_[2].texcoord.Set(1.0f,  1.0f);
            vertices_[3].position.Set(1.0f, -1.0f, -1.0f);
            vertices_[3].normal.Set(1.0f,  0.0f,  0.0f);
            vertices_[3].texcoord.Set(1.0f,  0.0f);
            
            // -Z side
            vertices_[4].position.Set( 1.0f,  1.0f, -1.0f);
            vertices_[4].normal.Set(0.0f,  0.0f, -1.0f);
            vertices_[4].texcoord.Set(0.0f,  1.0f);
            vertices_[5].position.Set( 1.0f, -1.0f, -1.0f);
            vertices_[5].normal.Set(0.0f,  0.0f, -1.0f);
            vertices_[5].texcoord.Set(0.0f,  0.0f);
            vertices_[6].position.Set(-1.0f,  1.0f, -1.0f);
            vertices_[6].normal.Set(0.0f,  0.0f, -1.0f);
            vertices_[6].texcoord.Set(1.0f,  1.0f);
            vertices_[7].position.Set(-1.0f, -1.0f, -1.0f);
            vertices_[7].normal.Set(0.0f,  0.0f, -1.0f);
            vertices_[7].texcoord.Set(1.0f,  0.0f);
            
            // -X side
            vertices_[8 ].position.Set(-1.0f,  1.0f, -1.0f);
            vertices_[8 ].normal.Set(-1.0f,  0.0f,  0.0f);
            vertices_[8 ].texcoord.Set(0.0f,  1.0f);
            vertices_[9 ].position.Set(-1.0f, -1.0f, -1.0f);
            vertices_[9 ].normal.Set(-1.0f,  0.0f,  0.0f);
            vertices_[9 ].texcoord.Set(0.0f,  0.0f);
            vertices_[10].position.Set(-1.0f,  1.0f,  1.0f);
            vertices_[10].normal.Set(-1.0f,  0.0f,  0.0f);
            vertices_[10].texcoord.Set(1.0f,  1.0f);
            vertices_[11].position.Set(-1.0f, -1.0f,  1.0f);
            vertices_[11].normal.Set(-1.0f,  0.0f,  0.0f);
            vertices_[11].texcoord.Set(1.0f,  0.0f);
            
            // +Z side
            vertices_[12].position.Set(-1.0f,  1.0f,  1.0f);
            vertices_[12].normal.Set(0.0f,  0.0f,  1.0f);
            vertices_[12].texcoord.Set(0.0f,  1.0f);
            vertices_[13].position.Set(-1.0f, -1.0f,  1.0f);
            vertices_[13].normal.Set(0.0f,  0.0f,  1.0f);
            vertices_[13].texcoord.Set(0.0f,  0.0f);
            vertices_[14].position.Set( 1.0f,  1.0f,  1.0f);
            vertices_[14].normal.Set(0.0f,  0.0f,  1.0f);
            vertices_[14].texcoord.Set(1.0f,  1.0f);
            vertices_[15].position.Set( 1.0f, -1.0f,  1.0f);
            vertices_[15].normal.Set(0.0f,  0.0f,  1.0f);
            vertices_[15].texcoord.Set(1.0f,  0.0f);
            
            // +Y side
            vertices_[16].position.Set( 1.0f,  1.0f,  1.0f);
            vertices_[16].normal.Set(0.0f,  1.0f,  0.0f);
            vertices_[16].texcoord.Set(1.0f,  1.0f);
            vertices_[17].position.Set( 1.0f,  1.0f, -1.0f);
            vertices_[17].normal.Set(0.0f,  1.0f,  0.0f);
            vertices_[17].texcoord.Set(0.0f,  1.0f);
            vertices_[18].position.Set(-1.0f,  1.0f,  1.0f);
            vertices_[18].normal.Set(0.0f,  1.0f,  0.0f);
            vertices_[18].texcoord.Set(1.0f,  0.0f);
            vertices_[19].position.Set(-1.0f,  1.0f, -1.0f);
            vertices_[19].normal.Set(0.0f,  1.0f,  0.0f);
            vertices_[19].texcoord.Set(0.0f,  0.0f);
            
            // -Y side
            vertices_[20].position.Set( 1.0f, -1.0f, -1.0f);
            vertices_[20].normal.Set(0.0f, -1.0f,  0.0f);
            vertices_[20].texcoord.Set(1.0f,  1.0f);
            vertices_[21].position.Set( 1.0f, -1.0f,  1.0f);
            vertices_[21].normal.Set(0.0f, -1.0f,  0.0f);
            vertices_[21].texcoord.Set(0.0f,  1.0f);
            vertices_[22].position.Set(-1.0f, -1.0f, -1.0f);
            vertices_[22].normal.Set(0.0f, -1.0f,  0.0f);
            vertices_[22].texcoord.Set(1.0f,  0.0f);
            vertices_[23].position.Set(-1.0f, -1.0f,  1.0f);
            vertices_[23].normal.Set(0.0f, -1.0f,  0.0f);
            vertices_[23].texcoord.Set(0.0f,  0.0f);

            // Scale box
            ScaleVertices(sizes_);
            
            indices_.resize(34);
            indices_ = {
                0,1,2,3, 3,4,
                4,5,6,7, 7,8,
                8,9,10,11, 11,12,
                12,13,14,15, 15,16,
                16,17,18,19, 19,20,
                20,21,22,23
            };
        }
        const math::Vector3& BoxModel::sizes() const
        {
            return sizes_;
        }
        
    } // namespace graphics
} // namespace sht