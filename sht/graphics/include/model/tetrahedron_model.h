#pragma once
#ifndef __SHT_GRAPHICS_TETRAHEDRON_MODEL_H__
#define __SHT_GRAPHICS_TETRAHEDRON_MODEL_H__

#include "model.h"

namespace sht {
    namespace graphics {
        
        class TetrahedronModel final : public Model {
        public:
            TetrahedronModel(Renderer * renderer);
            ~TetrahedronModel();
            
            void Create();
            
        protected:
        };
        
    } // namespace graphics
} // namespace sht

#endif