#pragma once
#ifndef __SHT_GRAPHICS_MESH_VERTICES_ENUMERATOR_H__
#define __SHT_GRAPHICS_MESH_VERTICES_ENUMERATOR_H__

#include "vertex.h"

namespace sht {
	namespace graphics {

		class ComplexMesh;

		struct MeshVerticesInfo {
			const Vertex * vertices;
			unsigned int num_vertices;
		};

		class MeshVerticesEnumerator {
		public:
			MeshVerticesEnumerator(ComplexMesh * complex_mesh);

			bool GetNextObject(MeshVerticesInfo * info);

		private:
			ComplexMesh * complex_mesh_;
			unsigned int index_;
		};
		
	} // namespace graphics
} // namespace sht

#endif