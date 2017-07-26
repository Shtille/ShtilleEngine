#pragma once
#ifndef __SHT_PHYSICS_GHOST_OBJECT_H__
#define __SHT_PHYSICS_GHOST_OBJECT_H__

#include "math/matrix.h"

#include "graphics/include/model/mesh_vertices_enumerator.h"

class btCollisionShape;
class btCollisionObject;
class btTriangleMesh;

namespace sht {
	namespace physics {

		struct UnitConversion;

		class GhostObject {
			friend class Engine;
		public:
			explicit GhostObject(const math::Vector3& position);
			~GhostObject();

			void CreateShape(const UnitConversion * unit_conversion, graphics::MeshVerticesEnumerator * enumerator);

			void SetPosition(const math::Vector3& position);
			void Translate(const math::Vector3& direction);
			void SetTransform(const math::Matrix4& transform);

			const math::Matrix4& matrix() const;

		private:
			btCollisionShape * shape_;
			btCollisionObject * object_;
			btTriangleMesh * triangle_mesh_;
			math::Matrix4 matrix_; //!< object matrix
		};

	} // namespace physics
} // namespace sht

#endif