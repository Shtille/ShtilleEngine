#pragma once
#ifndef __SHT_MATH_FRUSTUM_H__
#define __SHT_MATH_FRUSTUM_H__

#include "Vector.h"

namespace sht {
namespace math {

    /** Culling information definition */
    struct CullInfo {
        bool culled;
        char active_planes; // flags

        CullInfo(bool cull = false, char active = 0x3f) : culled(cull), active_planes(active) {}
    };

    /** Standard mathematical plane definition */
    struct Plane {
        Plane(){}
        Plane(float x, float y, float z, float w)
        {
            normal = vec3(x,y,z);
            float invLen = 1.0f / normal.Length();
            normal *= invLen;
            offset = w * invLen;
        }

        vec3 normal;
        float offset;
    };

    /** Bounding box definition */
    struct BoundingBox {
        vec3 center;
        vec3 extent;
    };

    /** 6-planes view frustum class */
    class Frustum {
    public:
        //! initializes frustum with matrix
        // Note: mvp should be column-major matrix
        void Load(const float *mvp);

        //! initializes frustum with matrix
        // Note: mvp should be row-major matrix (column-major is a normal OpenGL orientation)
        void LoadTransposed(const float *mvp);

        bool IsPointIn(const vec3& p) const;
        bool IsSphereIn(const vec3& p, float r) const;
        bool IsPolygonIn(int num_points, vec3 *p) const;
        bool IsBoxIn(const vec2& p, float min_h, float max_h, float size) const;
        bool IsBoxIn(const vec3& p, float sx, float sy, float sz) const;
        bool IsBoxIn(const BoundingBox& bb) const;
        bool IsPlaneIn(float sx, float sz, float ex, float ez, float h) const;

        //! optimized algorithm for terrain quadtree
        /* Usage:
        int TerrainChunk::Render(CullInfo cull_info)
        {
            if (cull_info.active_planes)
            {
                cull_info = frustum.ComputeBoxVisibility(box_center, box_extent, cull_info);
                if (cull_info.culled)
                {
                    // Bounding box is not visible; no need to draw this node or its children.
                    return 0;
                }    
            }
            int rendered = 0;
            if (has_children)
            {
                for (auto &c : children)
                    rendered += c.Render(cull_info);
            }
            else
            {
                rendered += data.Render();
            }
            return rendered;
        }
        And the top chunk should be called by:
        top_chunk.Render(CullInfo());
        */
        CullInfo ComputeBoxVisibility(const vec3& center, const vec3& extent, CullInfo in) const;

    protected:
        Plane planes_[6];
    };

} // namespace math
} // namespace sht

#endif