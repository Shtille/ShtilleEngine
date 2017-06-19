#include "physics_mesh.h"

#include <btBulletCollisionCommon.h>

namespace sht {
	namespace physics {

		Mesh::Mesh(const math::Vector3& position, float mass, const math::Vector3* vertices, unsigned int num_vertices)
		: Object(position)
		, mass_(mass)
		{
			triangle_mesh_ = new btTriangleMesh();
			triangle_mesh_->preallocateVertices(static_cast<int>(num_vertices));
			for (unsigned int i = 0; i < num_vertices; i += 3)
			{
				const math::Vector3& mesh_vertex0 = vertices[3 * i + 0];
				const math::Vector3& mesh_vertex1 = vertices[3 * i + 1];
				const math::Vector3& mesh_vertex2 = vertices[3 * i + 2];
				btVector3 vertex0(mesh_vertex0.x, mesh_vertex0.y, mesh_vertex0.z);
				btVector3 vertex1(mesh_vertex1.x, mesh_vertex1.y, mesh_vertex1.z);
				btVector3 vertex2(mesh_vertex2.x, mesh_vertex2.y, mesh_vertex2.z);
				triangle_mesh_->addTriangle(vertex0, vertex1, vertex2);
			}
			shape_ = new btBvhTriangleMeshShape(triangle_mesh_, true);
		}
		Mesh::~Mesh()
		{
			delete triangle_mesh_;
		}
		float Mesh::mass() const
		{
			return mass_;
		}

	} // namespace physics
} // namespace sht