#include "../../include/model/sphere_model.h"
#include "../../../math/sht_math.h"

namespace sht {
    namespace graphics {
        
        SphereModel::SphereModel(Renderer * renderer, u32 slices, u32 loops)
        : Model(renderer)
        , slices_(slices)
        , loops_(loops)
        {
            primitive_mode_ = PrimitiveType::kTriangles;
        }
        SphereModel::~SphereModel()
        {
            
        }
        void SphereModel::Create()
        {
            const float kRadius = 1.0f;
            
            vertices_.resize(2 + slices_*(loops_ - 2));
            
            volatile u32 ind = 0;
            
            vertices_[ind].position.Set(0.0f, -kRadius, 0.0f);
            vertices_[ind].normal.Set(0.0f, -1.0f, 0.0f);
            ++ind;
            vertices_[ind].position.Set(0.0f, kRadius, 0.0f);
            vertices_[ind].normal.Set(0.0f, 1.0f, 0.0f);
            ++ind;
            
            for (u32 j = 0; j < loops_-1; ++j)
            {
                float aj = math::kPi / (float)(loops_-1) * (float)j;
                for (u32 i = 0; i < slices_; ++i)
                {
                    float ai = math::kTwoPi / (float)slices_ * (float)i;
                    
                    float sin_aj = sinf(aj);
                    vertices_[ind].normal.Set(sin_aj*cosf(ai), -cosf(aj), sin_aj*sinf(ai));
                    vertices_[ind].position = vertices_[ind].normal * kRadius;
                    ++ind;
                }
            }
            
            indices_.resize((6*slices_)*(loops_ - 1));
            ind = 0;
            for (u32 j = 0; j < loops_-1; ++j)
            {
                for (u32 i = 0; i < slices_; ++i)
                {
                    u32 next_i = (i + 1 == slices_) ? (0) : (i+1);
                    u32 ind0 = (j+2 == loops_) ? (1) : (2 + (i     ) + (j  )*slices_);
                    u32 ind1 = (j == 0       ) ? (0) : (2 + (i     ) + (j-1)*slices_);
                    u32 ind2 = (j+2 == loops_) ? (1) : (2 + (next_i) + (j  )*slices_);
                    u32 ind3 = (j == 0       ) ? (0) : (2 + (next_i) + (j-1)*slices_);
                    indices_[ind++] = ind0;
                    indices_[ind++] = ind1;
                    indices_[ind++] = ind2;
                    indices_[ind++] = ind2;
                    indices_[ind++] = ind1;
                    indices_[ind++] = ind3;
                }
            }
        }
        
    } // namespace graphics
} // namespace sht