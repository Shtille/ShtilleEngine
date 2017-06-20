#pragma once
#ifndef __SHT_PHYSICS_MESH_H__
#define __SHT_PHYSICS_MESH_H__

#include "../include/physics_object.h"

#include "graphics/include/model/mesh_vertices_enumerator.h"

class btTriangleMesh;

namespace sht {
	namespace physics {

		class Mesh : public Object
		{
		public:
			explicit Mesh(const math::Vector3& position, float mass, const math::Vector3* vertices, unsigned int num_vertices);
			explicit Mesh(const math::Vector3& position, float mass, graphics::MeshVerticesEnumerator * enumerator);
			virtual ~Mesh();

			float mass() const;

		protected:
			float mass_;
			btTriangleMesh * triangle_mesh_;
		};

	} // namespace physics
} // namespace sht

#endif