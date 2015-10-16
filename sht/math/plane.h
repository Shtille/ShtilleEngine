#pragma once
#ifndef __SHT_MATH_PLANE_H__
#define __SHT_MATH_PLANE_H__

#include "vector.h"

namespace sht {
	namespace math {

	// Forward declarations
	struct Line;
	struct Segment;
	struct VerticalProfile;

    /** Standard mathematical plane definition */
    struct Plane {
        Plane();
        Plane(float x, float y, float z, float w);
        
        float Distance(const vec3& point) const;
        bool IntersectsLine(const vec3& a, const vec3& b, vec3& r) const;
        bool IntersectsLine(const Line& line, vec3& r) const;
        bool IntersectsSegment(const vec3& a, const vec3& b, vec3& r) const;
        bool IntersectsPlane(const Plane& plane, Line& line) const;
        bool IntersectsProfile(const VerticalProfile& profile, Segment& segment) const;
        void CropSegment(vec3& a, vec3& b) const;

        vec3 normal;
        float offset;
    };
    
    bool PlanesIntersection(const Plane& plane1, const Plane& plane2, const Plane& plane3, vec3& p);

	} // namespace math
} // namespace sht

#endif