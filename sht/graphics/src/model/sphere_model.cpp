#include "../../include/model/sphere_model.h"
#include "../../../math/sht_math.h"

namespace sht {
    namespace graphics {
        
        SphereModel::SphereModel(Renderer * renderer, u32 slices, u32 loops)
        : Model(renderer)
        , slices_(slices)
        , loops_(loops)
        {
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
            
            for (u32 j = 1; j < loops_-1; ++j)
            {
                float aj = math::kPi / (float)(loops_-1) * (float)j;
                float sin_aj = sinf(aj);
                float cos_aj = cosf(aj);
                for (u32 i = 0; i < slices_; ++i)
                {
                    float ai = math::kTwoPi / (float)slices_ * (float)i;
                    
                    vertices_[ind].normal.Set(sin_aj*cosf(ai), -cos_aj, -sin_aj*sinf(ai));
                    vertices_[ind].position = vertices_[ind].normal * kRadius;
                    ++ind;
                }
            }
            
            assert(loops_ > 3);
            indices_.resize((2 + 2*slices_)*(loops_ - 1) + 2*(loops_ - 2));
            ind = 0;
            for (u32 j = 0; j < loops_-1; ++j)
            {
                bool lat_end = (j+2 == loops_);
                bool lat_beg = (j == 0);
                indices_[ind++] = lat_end ? (1) : (2 + (j  )*slices_);
                indices_[ind++] = lat_beg ? (0) : (2 + (j-1)*slices_);
                for (u32 i = 0; i < slices_; ++i)
                {
                    u32 next_i = (i + 1 == slices_) ? (0) : (i+1);
                    
                    indices_[ind++] = lat_end ? (1) : (2 + (next_i) + (j  )*slices_);
                    indices_[ind++] = lat_beg ? (0) : (2 + (next_i) + (j-1)*slices_);
                }
                // Degenerates
                if (!lat_end)
                {
                    if (j+3 == loops_)
                    {
                        indices_[ind] = indices_[ind - 1], ++ind;
                        indices_[ind++] = 1;
                    }
                    else
                    {
                        indices_[ind] = indices_[ind - 1], ++ind;
                        indices_[ind++] = 2 + (j+1)*slices_;
                    }
                }
            }
        }
        
    } // namespace graphics
} // namespace sht