#include "../../include/model/skybox_quad_model.h"

namespace sht {
    namespace graphics {
        
        SkyboxQuadModel::SkyboxQuadModel(Renderer * renderer)
        : Model(renderer)
        {
        }
        SkyboxQuadModel::~SkyboxQuadModel()
        {
            
        }
        void SkyboxQuadModel::Create()
        {
            vertices_.resize(4);
            vertices_[0].position.Set(-1.0f,  -1.0f,  0.0f);
            vertices_[1].position.Set( 1.0f,  -1.0f,  0.0f);
            vertices_[2].position.Set(-1.0f,   1.0f,  0.0f);
            vertices_[3].position.Set( 1.0f,   1.0f,  0.0f);
            
            indices_.resize(4);
            indices_ = {
                0,1,2,3
            };
        }
        
    } // namespace graphics
} // namespace sht