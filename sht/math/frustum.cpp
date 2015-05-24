#include "Frustum.h"
#include <cmath>
#define FRUSTUM_LEFT   0
#define FRUSTUM_RIGHT  1
#define FRUSTUM_TOP    2
#define FRUSTUM_BOTTOM 3
#define FRUSTUM_FAR    4
#define FRUSTUM_NEAR   5

namespace sht {
namespace math {

    void Frustum::Load(const float *mvp)
    {        
        // mvp is column-major matrix
        planes_[FRUSTUM_LEFT  ] = Plane(mvp[3] + mvp[0], mvp[7] + mvp[4], mvp[11] + mvp[8],  mvp[15] + mvp[12]);
        planes_[FRUSTUM_RIGHT ] = Plane(mvp[3] - mvp[0], mvp[7] - mvp[4], mvp[11] - mvp[8],  mvp[15] - mvp[12]);

        planes_[FRUSTUM_TOP   ] = Plane(mvp[3] - mvp[1], mvp[7] - mvp[5], mvp[11] - mvp[9],  mvp[15] - mvp[13]);
        planes_[FRUSTUM_BOTTOM] = Plane(mvp[3] + mvp[1], mvp[7] + mvp[5], mvp[11] + mvp[9],  mvp[15] + mvp[13]);

        planes_[FRUSTUM_FAR   ] = Plane(mvp[3] - mvp[2], mvp[7] - mvp[6], mvp[11] - mvp[10], mvp[15] - mvp[14]);
        planes_[FRUSTUM_NEAR  ] = Plane(mvp[3] + mvp[2], mvp[7] + mvp[6], mvp[11] + mvp[10], mvp[15] + mvp[14]);
    }
    void Frustum::LoadTransposed(const float *mvp)
    {
        planes_[FRUSTUM_LEFT  ] = Plane(mvp[12] + mvp[0], mvp[13] + mvp[1], mvp[14] + mvp[2],  mvp[15] + mvp[3]);
        planes_[FRUSTUM_RIGHT ] = Plane(mvp[12] - mvp[0], mvp[13] - mvp[1], mvp[14] - mvp[2],  mvp[15] - mvp[3]);

        planes_[FRUSTUM_TOP   ] = Plane(mvp[12] - mvp[4], mvp[13] - mvp[5], mvp[14] - mvp[6],  mvp[15] - mvp[7]);
        planes_[FRUSTUM_BOTTOM] = Plane(mvp[12] + mvp[4], mvp[13] + mvp[5], mvp[14] + mvp[6],  mvp[15] + mvp[7]);

        planes_[FRUSTUM_FAR   ] = Plane(mvp[12] - mvp[8], mvp[13] - mvp[9], mvp[14] - mvp[10], mvp[15] - mvp[11]);
        planes_[FRUSTUM_NEAR  ] = Plane(mvp[12] + mvp[8], mvp[13] + mvp[9], mvp[14] + mvp[10], mvp[15] + mvp[11]);
    }
    bool Frustum::IsPointIn(const vec3& p) const
    {
        for (int i = 0; i < 6; ++i)
        if ((p & planes_[i].normal) + planes_[i].offset <= 0.0f)
            return false;
        return true;
    }
    bool Frustum::IsSphereIn(const vec3& p, float r) const
    {
        for (int i = 0; i < 6; ++i)
        if ((p & planes_[i].normal) + planes_[i].offset <= -r)
            return false;
        return true;
    }
    bool Frustum::IsPolygonIn(int num_points, vec3 *p) const
    {
        for (int i = 0; i < 6; ++i)
        {
            bool skip = false;
            for (int j = 0; j < num_points; ++j)
            if (planes_[i].normal.x*p[j].x +
                planes_[i].normal.y*p[j].y +
                planes_[i].normal.z*p[j].z +
                planes_[i].offset >= 0.0f)
            {
                skip = true;
                break;
            }
            if (skip)
                continue;

            return false;
        }
        return true;
    }
    bool Frustum::IsBoxIn(const vec2& p, float min_h, float max_h, float size) const
    {
        for (int i = 0; i < 6; ++i)
        {
            if (planes_[i].normal.x*(p.x - size) +
                planes_[i].normal.y*min_h +
                planes_[i].normal.z*(p.y - size) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + size) +
                planes_[i].normal.y*min_h +
                planes_[i].normal.z*(p.y - size) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + size) +
                planes_[i].normal.y*min_h +
                planes_[i].normal.z*(p.y + size) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x - size) +
                planes_[i].normal.y*min_h +
                planes_[i].normal.z*(p.y + size) +
                planes_[i].offset >= 0.0f)
                continue;

            if (planes_[i].normal.x*(p.x - size) +
                planes_[i].normal.y*max_h +
                planes_[i].normal.z*(p.y - size) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + size) +
                planes_[i].normal.y*max_h +
                planes_[i].normal.z*(p.y - size) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + size) +
                planes_[i].normal.y*max_h +
                planes_[i].normal.z*(p.y + size) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x - size) +
                planes_[i].normal.y*max_h +
                planes_[i].normal.z*(p.y + size) +
                planes_[i].offset >= 0.0f)
                continue;

            return false;
        }
        return true;
    }
    bool Frustum::IsBoxIn(const vec3& p, float sx, float sy, float sz) const
    {
        for (int i = 0; i < 6; ++i)
        {
            if (planes_[i].normal.x*(p.x - sx) +
                planes_[i].normal.y*(p.y - sy) +
                planes_[i].normal.z*(p.z - sz) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + sx) +
                planes_[i].normal.y*(p.y - sy) +
                planes_[i].normal.z*(p.z - sz) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + sx) +
                planes_[i].normal.y*(p.y - sy) +
                planes_[i].normal.z*(p.z + sz) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x - sx) +
                planes_[i].normal.y*(p.y - sy) +
                planes_[i].normal.z*(p.z + sz) +
                planes_[i].offset >= 0.0f)
                continue;

            if (planes_[i].normal.x*(p.x - sx) +
                planes_[i].normal.y*(p.y + sy) +
                planes_[i].normal.z*(p.z - sz) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + sx) +
                planes_[i].normal.y*(p.y + sy) +
                planes_[i].normal.z*(p.z - sz) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x + sx) +
                planes_[i].normal.y*(p.y + sy) +
                planes_[i].normal.z*(p.z + sz) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(p.x - sx) +
                planes_[i].normal.y*(p.y + sy) +
                planes_[i].normal.z*(p.z + sz) +
                planes_[i].offset >= 0.0f)
                continue;

            return false;
        }
        return true;
    }
    bool Frustum::IsBoxIn(const BoundingBox& bb) const
    {
        for (int i = 0; i < 6; ++i)
        {
            if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
                planes_[i].normal.y*(bb.center.y - bb.extent.y) +
                planes_[i].normal.z*(bb.center.z - bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
                planes_[i].normal.y*(bb.center.y - bb.extent.y) +
                planes_[i].normal.z*(bb.center.z - bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
                planes_[i].normal.y*(bb.center.y - bb.extent.y) +
                planes_[i].normal.z*(bb.center.z + bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
                planes_[i].normal.y*(bb.center.y - bb.extent.y) +
                planes_[i].normal.z*(bb.center.z + bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;

            if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
                planes_[i].normal.y*(bb.center.y + bb.extent.y) +
                planes_[i].normal.z*(bb.center.z - bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
                planes_[i].normal.y*(bb.center.y + bb.extent.y) +
                planes_[i].normal.z*(bb.center.z - bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
                planes_[i].normal.y*(bb.center.y + bb.extent.y) +
                planes_[i].normal.z*(bb.center.z + bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
                planes_[i].normal.y*(bb.center.y + bb.extent.y) +
                planes_[i].normal.z*(bb.center.z + bb.extent.z) +
                planes_[i].offset >= 0.0f)
                continue;

            return false;
        }
        return true;
    }
    bool Frustum::IsPlaneIn(float sx, float sz, float ex, float ez, float h) const
    {
        for (int i = 0; i < 6; ++i) // enum planes
        {
            if (planes_[i].normal.x*sx +
                planes_[i].normal.y*h  +
                planes_[i].normal.z*sz +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*ex +
                planes_[i].normal.y*h  +
                planes_[i].normal.z*sz +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*sx +
                planes_[i].normal.y*h  +
                planes_[i].normal.z*ez +
                planes_[i].offset >= 0.0f)
                continue;
            if (planes_[i].normal.x*ex +
                planes_[i].normal.y*h  +
                planes_[i].normal.z*ez +
                planes_[i].offset >= 0.0f)
                continue;

            return false;
        }
        return true;
    }
    CullInfo Frustum::ComputeBoxVisibility(const vec3& center, const vec3& extent, CullInfo in) const
    {
        // Check the box against each active frustum plane.
        int    bit = 1;
        for (int i = 0; i < 6; i++, bit <<= 1)
        {
            if ((bit & in.active_planes) == 0)
            {
                // This plane is already known to not cull the box.
                continue;
            }

            // Check box against this plane.
            float d = (planes_[i].normal & center) + planes_[i].offset; // was - p.d
            float extent_toward_plane = std::abs(extent.x * planes_[i].normal.x)
                                      + std::abs(extent.y * planes_[i].normal.y)
                                      + std::abs(extent.z * planes_[i].normal.z);
            if (d < 0)
            {
                if (-d > extent_toward_plane)
                {
                    // Box is culled by plane; it's not visible.
                    return CullInfo(true, 0);
                } // else this plane is ambiguous so leave it active.
            }
            else
            {
                if (d > extent_toward_plane)
                {
                    // Box is accepted by this plane, so
                    // deactivate it, since neither this
                    // box or any contained part of it can
                    // ever be culled by this plane.
                    in.active_planes &= ~bit;
                    if (in.active_planes == 0)
                    {
                        // This box is definitively inside all the culling
                        // planes_, so there's no need to continue.
                        return in;
                    }
                } // else this plane is ambigious so leave it active.
            }
        }

        return in;    // Box not definitively culled.  Return updated active plane flags.
    }

} // namespace math
} // namespace sht
