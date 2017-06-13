#include "../../include/model/complex_mesh.h"

#include "../../include/model/mesh.h"

#include "system/include/stream/log_stream.h"
#include "system/include/string/filename.h"
#include "system/include/filesystem/directory.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <map>
#include <memory>
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

			sht::system::Filename fn(filename);
			std::string base_dir = fn.ExtractPath() + system::GetPathDelimeter();

			std::string err;
			if (! tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, base_dir.c_str(), true))
			{
				if (!err.empty())
					error_log->PrintString("%s\n", err.c_str());
				error_log->PrintString("can't open %s\n", filename);
				return false;
			}

			if (!err.empty())
				error_log->PrintString("%s\n", err.c_str());

			math::Vector3 min = math::Vector3(1e8), max = math::Vector3(-1e8);

			// Make unique mesh per material per shape
			typedef std::map<int, std::unique_ptr<Mesh> > MaterialMap;
			std::vector< MaterialMap > shape_meshes;

			// Loop over shapes
			for (const auto& shape : shapes)
			{
				if (shape.mesh.indices.empty())
					continue;

				// Push an empty map at the beginning
				shape_meshes.push_back(MaterialMap());

				// Loop over faces
				size_t index_offset = 0;
				for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
				{
					int fv = shape.mesh.num_face_vertices[f];

					// Per-face material
					Mesh * mesh = nullptr;
					int material_id = shape.mesh.material_ids[f];
					if (material_id == -1)
					{
						error_log->PrintString("some face(s) don't have material(s) in %s\n", filename);
						return false;
					}
					else
					{
						MaterialMap& materials_map = shape_meshes.back();
						auto it = materials_map.find(material_id);
						if (it == materials_map.end())
						{
							mesh = new Mesh(renderer_);
							mesh->primitive_mode_ = PrimitiveType::kTriangles;
							materials_map[material_id].reset(mesh);
						}
						else
							mesh = it->second.get();
					}

					// Loop over vertices in the face
					for (size_t v = 0; v < fv; ++v)
					{
						// Access to vertex
						tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
						Vertex vertex;
						if (idx.vertex_index != -1)
							memcpy(&vertex.position, &attrib.vertices [3*idx.vertex_index  ], 3 * sizeof(float));
						if (idx.normal_index != -1)
							memcpy(&vertex.normal,   &attrib.normals  [3*idx.normal_index  ], 3 * sizeof(float));
						if (idx.texcoord_index != -1)
							memcpy(&vertex.texcoord, &attrib.texcoords[2*idx.texcoord_index], 2 * sizeof(float));

						min.MakeFloor(vertex.position);
						max.MakeCeil(vertex.position);

						mesh->vertices_.push_back(vertex);
						// Indices isn't necessary
					}
					index_offset += fv;
				}
			}

			bounding_box_.center = 0.5f * (max + min);
			bounding_box_.extent = 0.5f * (max - min);

			// for (const auto& material : materials)
			// {
			// 	memcpy(&vertex.position, &material.ambient[0], 3 * sizeof(float));
			// }

			// Finally fill the meshes
			for (auto& map : shape_meshes)
			{
				for (auto& pair : map)
				{
					Mesh * mesh = pair.second.release();
					meshes_.push_back(mesh);
				}
			}

			return true;
		}

	} // namespace graphics
} // namespace sht