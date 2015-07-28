#pragma once
#ifndef __SHT_GRAPHICS_CUBE_MODEL_H__
#define __SHT_GRAPHICS_CUBE_MODEL_H__

#include "model.h"

namespace sht {
    namespace graphics {
        
        class CubeModel final : public Model {
        public:
            CubeModel(Renderer * renderer);
            ~CubeModel();
            
            void Create();
            
        protected:
        };
        
    } // namespace graphics
} // namespace sht

#endif