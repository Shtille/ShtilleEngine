#include "../../include/model/mesh.h"

#include "../../include/material.h"

namespace sht {
	namespace graphics {

		Mesh::Mesh(Renderer * renderer)
		: primitive_mode_(PrimitiveType::kTriangleStrip)
		, renderer_(renderer)
		, material_(nullptr)
		, vertex_buffer_(nullptr)
		, index_buffer_(nullptr)
		, vertex_array_object_(0)
		, num_vertices_(0)
		, vertices_array_(nullptr)
		, num_indices_(0)
		, index_size_(0)
		, indices_array_(nullptr)
		{
			
		}
		Mesh::~Mesh()
		{
			if (vertex_buffer_)
				renderer_->DeleteVertexBuffer(vertex_buffer_);
			if (index_buffer_)
				renderer_->DeleteIndexBuffer(index_buffer_);
			if (vertex_array_object_)
				renderer_->context()->DeleteVertexArrayObject(vertex_array_object_);
			FreeArrays();
		}
		void Mesh::FreeArrays()
		{
			if (vertices_array_)
			{
				delete [] vertices_array_;
				vertices_array_ = nullptr;
			}
			if (indices_array_)
			{
				delete [] indices_array_;
				indices_array_ = nullptr;
			}
		}
		void Mesh::TransformVertices(VertexFormat * vertex_format, const std::vector<VertexAttribute>& attribs)
		{
			num_vertices_ = (u32)vertices_.size();
			vertices_array_ = new u8[num_vertices_ * vertex_format->vertex_size()];
			u8 *ptr = vertices_array_;
			for (auto &v : vertices_)
			{
				for (auto &a : attribs)
				{
					switch (a.type)
					{
					case VertexAttribute::kVertex:
						memcpy(ptr, v.position, sizeof(v.position));
						break;
					case VertexAttribute::kNormal:
						memcpy(ptr, v.normal, sizeof(v.normal));
						break;
					case VertexAttribute::kTexcoord:
						memcpy(ptr, v.texcoord, sizeof(v.texcoord));
						break;
					case VertexAttribute::kTangent:
						memcpy(ptr, v.tangent, sizeof(v.tangent));
						break;
					case VertexAttribute::kBinormal:
						memcpy(ptr, v.binormal, sizeof(v.binormal));
						break;
					default:
						assert(!"Unknown vertex attribute");
					}
					ptr += a.size * sizeof(float);
				}
			}
			vertices_.clear();
			vertices_.shrink_to_fit();
			
			if (!indices_.empty())
			{
				num_indices_ = (u32)indices_.size();
				if (num_indices_ > 0xffff)
				{
					index_size_ = sizeof(u32);
					index_data_type_ = DataType::kUnsignedInt;
					indices_array_ = new u8[num_indices_ * index_size_];
					u32 *indices = reinterpret_cast<u32*>(indices_array_);
					for (size_t i = 0; i < indices_.size(); ++i)
					{
						indices[i] = static_cast<u32>(indices_[i]);
					}
				}
				else
				{
					index_size_ = sizeof(u16);
					index_data_type_ = DataType::kUnsignedShort;
					indices_array_ = new u8[num_indices_ * index_size_];
					u16 *indices = reinterpret_cast<u16*>(indices_array_);
					for (size_t i = 0; i < indices_.size(); ++i)
					{
						indices[i] = static_cast<u16>(indices_[i]);
					}
				}
				indices_.clear();
				indices_.shrink_to_fit();
			}
		}
		bool Mesh::MakeRenderable(VertexFormat * vertex_format, const std::vector<VertexAttribute>& attribs)
		{
			const bool have_indices = !indices_.empty();

			TransformVertices(vertex_format, attribs);
			
			renderer_->context()->GenVertexArrayObject(vertex_array_object_);
			renderer_->context()->BindVertexArrayObject(vertex_array_object_);
			
			renderer_->AddVertexBuffer(vertex_buffer_, num_vertices_ * vertex_format->vertex_size(), vertices_array_, BufferUsage::kStaticDraw);
			if (vertex_buffer_ == nullptr) return false;
			
			if (have_indices)
			{
				renderer_->AddIndexBuffer(index_buffer_, num_indices_, index_size_, indices_array_, BufferUsage::kStaticDraw);
				if (index_buffer_ == nullptr) return false;
			}
			
			const char* base = (char*)0;
			for (u32 i = 0; i < attribs.size(); ++i)
			{
				const VertexFormat::Attrib& generic = vertex_format->generic(i);
				renderer_->context()->VertexAttribPointer(i, generic.size, DataType::kFloat, vertex_format->vertex_size(), base + generic.offset);
				renderer_->context()->EnableVertexAttribArray(i);
			}
			
			renderer_->context()->BindVertexArrayObject(0);
			
			FreeArrays();
			
			return true;
		}
		void Mesh::Render()
		{
			renderer_->context()->BindVertexArrayObject(vertex_array_object_);
			if (index_buffer_ == nullptr)
				renderer_->context()->DrawArrays(primitive_mode_, 0, num_vertices_);
			else
				renderer_->context()->DrawElements(primitive_mode_, num_indices_, index_data_type_);
			renderer_->context()->BindVertexArrayObject(0);
		}
		void Mesh::ScaleVertices(const math::Vector3& scale)
		{
			for (auto& v : vertices_)
				v.position *= scale;
		}
		void Mesh::ScaleTexcoord(const math::Vector2& scale)
		{
			for (auto& v : vertices_)
				v.texcoord *= scale;
		}

	} // namespace graphics
} // namespace sht