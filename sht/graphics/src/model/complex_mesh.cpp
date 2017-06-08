#include "../../include/model/complex_mesh.h"

#include "../../include/model/mesh.h"
#include "../../include/renderer/renderer.h"
#include "system/include/string/filename.h"

#include <assert.h>

namespace sht {
	namespace graphics {

		static ComplexMesh::FileFormat ExtractFileFormat(const char* filename)
		{
			sht::system::Filename fn(filename);
			std::string ext = fn.ExtractExt();
			if (ext == "obj")
				return ComplexMesh::FileFormat::kObj;
			return ComplexMesh::FileFormat::kUnknown;
		}

		ComplexMesh::ComplexMesh(Renderer * renderer)
		: renderer_(renderer)
		, vertex_format_(nullptr)
		{

		}
		ComplexMesh::~ComplexMesh()
		{
			if (vertex_format_)
				renderer_->DeleteVertexFormat(vertex_format_);
		}
		bool ComplexMesh::LoadFromFile(const char* filename)
		{
			FileFormat fmt = ExtractFileFormat(filename);
			switch (fmt)
			{
			case FileFormat::kObj:
				return LoadFromFileObj(filename);
			default:
				assert(!"unknown model format");
				return false;
			}
		}
		void ComplexMesh::AddFormat(const VertexAttribute& attrib)
		{
			attribs_.push_back(attrib);
		}
		bool ComplexMesh::MakeRenderable()
		{
			if (attribs_.empty())
			{
				assert(!"Vertex format hasn't been set.");
				return false;
			}
			renderer_->AddVertexFormat(vertex_format_, &attribs_[0], (u32)attribs_.size());

			for (auto mesh : meshes_)
			{
				if (!mesh->MakeRenderable(vertex_format_))
					return false;
			}

			return true;
		}
		void ComplexMesh::Render()
		{
			for (auto mesh : meshes_)
			{
				mesh->Render();
			}
		}

	} // namespace graphics
} // namespace sht