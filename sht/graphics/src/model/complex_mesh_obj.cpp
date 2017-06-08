#include "../../include/model/complex_mesh.h"

#include "../../include/model/mesh.h"

#include "system/include/stream/log_stream.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <map>
#include <cstring>
#include <assert.h>

#ifdef TINYOBJLOADER_USE_DOUBLE
	static_assert(false, "disable double precision on tinyobj");
#endif

namespace sht {
	namespace graphics {

		bool ComplexMesh::SaveToFileObj(const char *filename)
		{
			assert(!"Haven't been implemented yet");
			return true;
		}
		bool ComplexMesh::LoadFromFileObj(const char *filename)
		{
			system::ErrorLogStream * error_log = system::ErrorLogStream::GetInstance();

			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;

			std::string err;
			if (! tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, base_dir.c_str()))
			{
				if (!err.empty())
					error_log->PrintString("%s\n", err.c_str());
				error_log->PrintString("can't open %s\n", filename);
				return false;
			}

			if (!err.empty())
				error_log->PrintString("%s\n", err.c_str());

			// Make unique mesh per material per shape
			std::vector< std::map<int, Mesh *> > shape_meshes;

			// Loop over shapes
			for (size_t s = 0; s < shapes.size(); ++s)
			{
				// Loop over faces
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
				{
					int fv = shapes[s].mesh.num_face_vertices[f];
					// Loop over vertices in the face
					for (size_t v = 0; v < fv; ++v)
					{
						// Access to vertex
						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
						if (idx.vertex_index   == -1 ||
							idx.normal_index   == -1 ||
							idx.texcoord_index == -1)
						{
							error_log->PrintString("some arrays don't present in %s\n", filename);
							return false;
						}
					}
					index_offset += fv;

					// Per-face material
					int material_id = shapes[s].mesh.material_ids[f];
					if (material_id == -1)
					{
						error_log->PrintString("some face(s) don't have material(s) in %s\n", filename);
						return false;
					}
					else
					{
						auto it = materials_map.find(material_id);
						if (it == materials_map.end())
							materials_map[material_id] = new Mesh(renderer_);
					}
				}
			}

			// Loop over shapes
			for (size_t s = 0; s < shapes.size(); ++s)
			{
				// Loop over faces
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
				{
					int fv = shapes[s].mesh.num_face_vertices[f];

					int material_id = shapes[s].mesh.material_ids[f];
					Mesh * mesh = materials_map[material_id];

					// Loop over vertices in the face
					for (size_t v = 0; v < fv; ++v)
					{
						// Access to vertex
						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
						Vertex vertex;
						memcpy(&vertex.position, &attrib.vertices [3*idx.vertex_index  ], 3 * sizeof(float));
						memcpy(&vertex.normal,   &attrib.normals  [3*idx.normal_index  ], 3 * sizeof(float));
						memcpy(&vertex.texcoord, &attrib.texcoords[2*idx.texcoord_index], 2 * sizeof(float));

						mesh->vertices_.push_back(vertex);
						mesh->indices_.push_back(static_cast<unsigned int>(mesh->indices_.size()));
					}
					index_offset += fv;
				}
			}

			for (auto& pair : materials_map)
			{
				meshes_.push_back(pair.second);
			}

			return true;
		}

	} // namespace graphics
} // namespace sht