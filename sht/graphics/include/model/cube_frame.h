#pragma once
#ifndef __SHT_GRAPHICS_CUBE_FRAME_H__
#define __SHT_GRAPHICS_CUBE_FRAME_H__

#include "model.h"

namespace sht {
    namespace graphics {
        
        class CubeFrameModel final : public Model {
        public:
            CubeFrameModel(Renderer * renderer);
            ~CubeFrameModel();
            
            void Create();
            
        protected:
        };
        
    } // namespace graphics
} // namespace sht

#endif