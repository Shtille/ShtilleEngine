#include "../../include/model/complex_mesh.h"

#include "../../include/model/mesh.h"
#include "../../include/material.h"

#include "system/include/stream/file_stream.h"
#include "system/include/stream/log_stream.h"

#include "utility/include/string_id.h"

namespace {
	const uint32_t kSignature = ConstexprStringId("SCM");
	const uint32_t kVersion = 1;

	static_assert(sizeof(sht::graphics::Material) == 5 * sizeof(sht::math::Vector3) + 2 * sizeof(float),
		"material structure has changed, update .scm file version");
	static_assert(sizeof(sht::graphics::Vertex) == 4 * sizeof(sht::math::Vector3) + sizeof(sht::math::Vector2),
		"vertex structure has changed, update .scm file version");
}

namespace sht {
	namespace graphics {

		bool ComplexMesh::SaveToFileScm(const char *filename)
		{
			system::ErrorLogStream * error_log = system::ErrorLogStream::GetInstance();
			system::FileStream file;
			if (!file.Open(filename, system::StreamAccess::kWriteBinary))
			{
				error_log->PrintString("can't open for write %s\n", filename);
				return false;
			}

			// Write header
			file.WriteValue(kSignature);
			file.WriteValue(kVersion);

			// Write bounding box
			file.Write(&bounding_box_, sizeof(math::BoundingBox));

			// Write materials
			uint32_t num_materials = static_cast<uint32_t>(materials_.size());
			file.WriteValue(num_materials);
			if (num_materials != 0)
				file.Write(&materials_[0], materials_.size() * sizeof(Material));

			// Write meshes
			uint32_t num_meshes = static_cast<uint32_t>(meshes_.size());
			file.WriteValue(num_meshes);
			for (auto mesh : meshes_)
			{
				uint32_t material_index = 0;
				while ((material_index < num_materials) && (&materials_[material_index] != mesh->material_))
					++material_index;
				file.WriteValue(material_index);
				uint32_t primitive_mode = static_cast<uint32_t>(mesh->primitive_mode_);
				file.WriteValue(primitive_mode);
				uint32_t num_vertices = static_cast<uint32_t>(mesh->vertices_.size());
				file.WriteValue(num_vertices);
				if (num_vertices != 0)
					file.Write(&mesh->vertices_[0], mesh->vertices_.size() * sizeof(Vertex));
				uint32_t num_indices = static_cast<uint32_t>(mesh->indices_.size());
				file.WriteValue(num_indices);
				if (num_indices != 0)
					file.Write(&mesh->indices_[0], mesh->indices_.size() * sizeof(uint32_t));
			}

			return true;
		}
		bool ComplexMesh::LoadFromFileScm(const char *filename)
		{
			system::ErrorLogStream * error_log = system::ErrorLogStream::GetInstance();
			system::FileStream file;
			if (!file.Open(filename, system::StreamAccess::kReadBinary))
			{
				error_log->PrintString("can't open %s\n", filename);
				return false;
			}

			// Read header
			uint32_t signature;
			file.ReadValue(signature);
			if (signature != kSignature)
			{
				error_log->PrintString("wrong file signature (%s)\n", filename);
				return false;
			}
			uint32_t version;
			file.ReadValue(version);
			if (version != kVersion)
			{
				error_log->PrintString("wrong file version (%s)\n", filename);
				return false;
			}

			// Read bounding box
			file.Read(&bounding_box_, sizeof(math::BoundingBox));

			// Read materials
			uint32_t num_materials;
			file.ReadValue(num_materials);
			materials_.resize(num_materials);
			if (num_materials != 0)
				file.Read(&materials_[0], materials_.size() * sizeof(Material));

			// Read meshes
			uint32_t num_meshes;
			file.ReadValue(num_meshes);
			meshes_.resize(num_meshes);
			for (auto& mesh : meshes_)
			{
				uint32_t material_index;
				file.ReadValue(material_index);
				uint32_t primitive_mode;
				file.ReadValue(primitive_mode);

				mesh = new Mesh(renderer_);
				mesh->primitive_mode_ = static_cast<PrimitiveType>(primitive_mode);
				mesh->material_ = &materials_[material_index];

				uint32_t num_vertices;
				file.ReadValue(num_vertices);
				mesh->vertices_.resize(num_vertices);
				if (num_vertices != 0)
					file.Read(&mesh->vertices_[0], mesh->vertices_.size() * sizeof(Vertex));
				uint32_t num_indices;
				file.ReadValue(num_indices);
				mesh->indices_.resize(num_indices);
				if (num_indices != 0)
					file.Read(&mesh->indices_[0], mesh->indices_.size() * sizeof(uint32_t));
			}

			return true;
		}

	} // namespace graphics
} // namespace sht