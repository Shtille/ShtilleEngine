#include "../include/physics_ghost_object.h"

#include "../include/physics_unit_converter.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace sht {
	namespace physics {

		GhostObject::GhostObject(const math::Vector3& position)
		: shape_(nullptr)
		, object_(nullptr)
		, triangle_mesh_(nullptr)
		{
			matrix_.Identity();
			matrix_.sa[12] = position.x;
			matrix_.sa[13] = position.y;
			matrix_.sa[14] = position.z;
		}
		GhostObject::~GhostObject()
		{
			if (shape_)
				delete shape_;
			if (object_)
				delete object_;
			if (triangle_mesh_)
				delete triangle_mesh_;
		}
		void GhostObject::CreateShape(const UnitConversion * unit_conversion, graphics::MeshVerticesEnumerator * enumerator)
		{
			triangle_mesh_ = new btTriangleMesh();
			graphics::MeshVerticesInfo vertices_info;
			while (enumerator->GetNextObject(&vertices_info))
			{
				for (unsigned int i = 0; i < vertices_info.num_vertices; i += 3)
				{
					math::Vector3 mesh_vertex0 = vertices_info.vertices[i + 0].position;
					math::Vector3 mesh_vertex1 = vertices_info.vertices[i + 1].position;
					math::Vector3 mesh_vertex2 = vertices_info.vertices[i + 2].position;
					if (unit_conversion && unit_conversion->linear_to)
					{
						unit_conversion->linear_to(&mesh_vertex0.x);
						unit_conversion->linear_to(&mesh_vertex0.y);
						unit_conversion->linear_to(&mesh_vertex0.z);
						unit_conversion->linear_to(&mesh_vertex1.x);
						unit_conversion->linear_to(&mesh_vertex1.y);
						unit_conversion->linear_to(&mesh_vertex1.z);
						unit_conversion->linear_to(&mesh_vertex2.x);
						unit_conversion->linear_to(&mesh_vertex2.y);
						unit_conversion->linear_to(&mesh_vertex2.z);
					}
					btVector3 vertex0(mesh_vertex0.x, mesh_vertex0.y, mesh_vertex0.z);
					btVector3 vertex1(mesh_vertex1.x, mesh_vertex1.y, mesh_vertex1.z);
					btVector3 vertex2(mesh_vertex2.x, mesh_vertex2.y, mesh_vertex2.z);
					triangle_mesh_->addTriangle(vertex0, vertex1, vertex2);
				}
			}
			shape_ = new btBvhTriangleMeshShape(triangle_mesh_, true);

			object_ = new btGhostObject();
			object_->setCollisionShape(shape_);

			// Set initial object transform
			btTransform transform;
			transform.setFromOpenGLMatrix(matrix_);
			object_->setWorldTransform(transform);

			// Disable collision response with rigid bodies
			object_->setCollisionFlags(object_->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
		int GhostObject::GetNumOverlappingObjects() const
		{
			return object_->getNumOverlappingObjects();
		}
		void GhostObject::SetPosition(const math::Vector3& position)
		{
			// Ignore unit conversion here
			matrix_.sa[12] = position.x;
			matrix_.sa[13] = position.y;
			matrix_.sa[14] = position.z;
		}
		void GhostObject::Translate(const math::Vector3& direction)
		{
			// Ignore unit conversion here
			matrix_.sa[12] += direction.x;
			matrix_.sa[13] += direction.y;
			matrix_.sa[14] += direction.z;
		}
		void GhostObject::SetTransform(const math::Matrix4& transform)
		{
			// Ignore unit conversion here
			matrix_ = transform;
			btTransform xform;
			xform.setFromOpenGLMatrix(matrix_);
			object_->setWorldTransform(xform);
		}
		const math::Matrix4& GhostObject::matrix() const
		{
			return matrix_;
		}

	} // namespace physics
} // namespace sht