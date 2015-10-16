#pragma once
#ifndef __SHT_MATH_FRUSTUM_H__
#define __SHT_MATH_FRUSTUM_H__

#include "vector.h"
#include "plane.h"

namespace sht {
namespace math {

	// Forward declarations
	struct BoundingBox;
	struct Line;
	struct Segment;
	struct VerticalProfile;

    /** Culling information definition */
    struct CullInfo {
        bool culled;
        char active_planes; // flags

        CullInfo(bool cull = false, char active = 0x3f) : culled(cull), active_planes(active) {}
    };

    /** 6-planes view frustum class */
    class Frustum {
    public:
	    const vec3& getDir() const; //!< returns frustum view direction
	    
        //! initializes frustum with matrix
        // Note: mvp should be column-major matrix
        void Load(const float *mvp);

        //! initializes frustum with matrix
        // Note: mvp should be row-major matrix (column-major is a normal OpenGL orientation)
        void LoadTransposed(const float *mvp);

        bool IsPointIn(const vec3& p) const;
        bool IsSphereIn(const vec3& p, float r) const;
        bool IsPolygonIn(int num_points, vec3 *p) const;
        bool IsSegmentIn(const Segment& segment) const;
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

        int IntersectionsWithSegment(const Segment& segment, vec3 points[2]) const;
        int IntersectionsWithPlane(const Plane& plane, Segment segments[6]) const;
        int IntersectionsWithProfile(const VerticalProfile& profile, Segment segments[6]) const;

    protected:
    	bool CropLine(const Line& line, int index, Segment& cropped_segment) const;

        Plane planes_[6];           		//!< frustum planes
        static int opposite_planes_[6];		//!< opposite plane indices
    };

} // namespace math
} // namespace sht

#endif