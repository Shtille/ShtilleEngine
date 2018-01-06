#include "../../include/model/screen_quad_model.h"

namespace sht {
    namespace graphics {
        
        ScreenQuadModel::ScreenQuadModel(Renderer * renderer)
        : Model(renderer)
        {
        }
        ScreenQuadModel::~ScreenQuadModel()
        {
            
        }
        void ScreenQuadModel::Create()
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