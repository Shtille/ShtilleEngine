#pragma once
#ifndef __SHT_GRAPHICS_SKYBOX_QUAD_MODEL_H__
#define __SHT_GRAPHICS_SKYBOX_QUAD_MODEL_H__

#include "model.h"

namespace sht {
    namespace graphics {
        
        class ScreenQuadModel final : public Model {
        public:
            ScreenQuadModel(Renderer * renderer);
            ~ScreenQuadModel();
            
            void Create();
            
        protected:
        };
        
    } // namespace graphics
} // namespace sht

#endif