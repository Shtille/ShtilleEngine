#include "physics_mesh.h"

#include "../include/physics_unit_converter.h"

#include <btBulletCollisionCommon.h>

namespace sht {
	namespace physics {

		Mesh::Mesh(const math::Vector3& position, float mass)
		: Object(position)
		, mass_(mass)
		, triangle_mesh_(nullptr)
		{

		}
		void Mesh::CreateShape(const UnitConversion * unit_conversion, graphics::MeshVerticesEnumerator * enumerator)
		{
			unit_conversion_ = unit_conversion;
			triangle_mesh_ = new btTriangleMesh();
			graphics::MeshVerticesInfo vertices_info;
			while (enumerator->GetNextObject(&vertices_info))
			{
				for (unsigned int i = 0; i < vertices_info.num_vertices; i += 3)
				{
					math::Vector3 mesh_vertex0 = vertices_info.vertices[i + 0].position;
					math::Vector3 mesh_vertex1 = vertices_info.vertices[i + 1].position;
					math::Vector3 mesh_vertex2 = vertices_info.vertices[i + 2].position;
					if (unit_conversion_ && unit_conversion_->linear_to)
					{
						unit_conversion_->linear_to(&mesh_vertex0.x);
						unit_conversion_->linear_to(&mesh_vertex0.y);
						unit_conversion_->linear_to(&mesh_vertex0.z);
						unit_conversion_->linear_to(&mesh_vertex1.x);
						unit_conversion_->linear_to(&mesh_vertex1.y);
						unit_conversion_->linear_to(&mesh_vertex1.z);
						unit_conversion_->linear_to(&mesh_vertex2.x);
						unit_conversion_->linear_to(&mesh_vertex2.y);
						unit_conversion_->linear_to(&mesh_vertex2.z);
					}
					btVector3 vertex0(mesh_vertex0.x, mesh_vertex0.y, mesh_vertex0.z);
					btVector3 vertex1(mesh_vertex1.x, mesh_vertex1.y, mesh_vertex1.z);
					btVector3 vertex2(mesh_vertex2.x, mesh_vertex2.y, mesh_vertex2.z);
					triangle_mesh_->addTriangle(vertex0, vertex1, vertex2);
				}
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