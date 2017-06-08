#pragma once
#ifndef __SHT_GRAPHICS_MATERIAL_H__
#define __SHT_GRAPHICS_MATERIAL_H__

namespace sht {
    namespace graphics {
        
        //! Base material class
        class Material {
        public:
            Material();
            virtual ~Material();

            virtual void Bind() = 0;
            virtual void Unbind() = 0;
        };
        
    }
}

#endif