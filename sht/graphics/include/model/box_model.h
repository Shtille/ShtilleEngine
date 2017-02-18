#pragma once
#ifndef __SHT_GRAPHICS_BOX_MODEL_H__
#define __SHT_GRAPHICS_BOX_MODEL_H__

#include "model.h"

namespace sht {
    namespace graphics {
        
        class BoxModel final : public Model {
        public:
            BoxModel(Renderer * renderer, float size_x, float size_y, float size_z);
            ~BoxModel();
            
            void Create();

            const math::Vector3& sizes() const;
            
        protected:
        	vec3 sizes_;
        };
        
    } // namespace graphics
} // namespace sht

#endif