#pragma once
#ifndef __SHT_GRAPHICS_MATERIAL_H__
#define __SHT_GRAPHICS_MATERIAL_H__

#include "math/vector.h"

namespace sht {
    namespace graphics {
        
        //! Base material class
        struct Material
        {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 transmittance;
            vec3 emission;
            float shininess;
            float dissolve;  // 1 == opaque; 0 == fully transparent
        };

        //! Interface class for material binder
        class MaterialBinderInterface {
        public:
            virtual ~MaterialBinderInterface() = default;

            virtual void Bind(Material * material) = 0;
        };
        
    }
}

#endif