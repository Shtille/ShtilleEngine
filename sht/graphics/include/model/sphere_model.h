#pragma once
#ifndef __SHT_GRAPHICS_SPHERE_MODEL_H__
#define __SHT_GRAPHICS_SPHERE_MODEL_H__

#include "model.h"

namespace sht {
    namespace graphics {
        
        class SphereModel final : public Model {
        public:
            SphereModel(Renderer * renderer, u32 slices = 20, u32 loops = 10);
            ~SphereModel();
            
            void Create();
            
        protected:
            u32 slices_;    //!< number of vertices in sphere's longitude direction
            u32 loops_;     //!< number of vertices in sphere's latitude direction
        };
        
    } // namespace graphics
} // namespace sht

#endif