#pragma once
#ifndef __SHT_GRAPHICS_MODEL_VERTEX_H__
#define __SHT_GRAPHICS_MODEL_VERTEX_H__

#include "../../../math/vector.h"

namespace sht {
    namespace graphics {
        
        struct Vertex {
            vec3 position;
            vec3 normal;
            vec2 texcoord;
            vec3 tangent;
            vec3 binormal;
        };
        
    } // namespace graphics
} // namespace sht

#endif