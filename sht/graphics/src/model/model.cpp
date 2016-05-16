#include "../../include/model/model.h"

namespace sht {
    namespace graphics {
        
        Model::Model(Renderer * renderer)
		: primitive_mode_(PrimitiveType::kTriangleStrip)
        , renderer_(renderer)
        , vertex_format_(nullptr)
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
        Model::~Model()
        {
            if (vertex_format_)
                renderer_->DeleteVertexFormat(vertex_format_);
            if (vertex_buffer_)
                renderer_->DeleteVertexBuffer(vertex_buffer_);
            if (index_buffer_)
                renderer_->DeleteIndexBuffer(index_buffer_);
            if (vertex_array_object_)
                renderer_->context()->DeleteVertexArrayObject(vertex_array_object_);
            FreeArrays();
        }
        void Model::AddFormat(const VertexAttribute& attrib)
        {
            attribs_.push_back(attrib);
        }
        void Model::FreeArrays()
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
        void Model::TransformVertices()
        {
            num_vertices_ = (u32)vertices_.size();
            vertices_array_ = new u8[num_vertices_ * vertex_format_->vertex_size()];
            u8 *ptr = vertices_array_;
            for (auto &v : vertices_)
            for (auto &a : attribs_)
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
            vertices_.clear();
            vertices_.shrink_to_fit();
            
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
        bool Model::MakeRenderable()
        {
            if (attribs_.empty())
            {
                assert(!"Vertex format hasn't been set.");
                return false;
            }
            renderer_->AddVertexFormat(vertex_format_, &attribs_[0], (u32)attribs_.size());
            
            TransformVertices();
            
            renderer_->context()->GenVertexArrayObject(vertex_array_object_);
            renderer_->context()->BindVertexArrayObject(vertex_array_object_);
            
            renderer_->AddVertexBuffer(vertex_buffer_, num_vertices_ * vertex_format_->vertex_size(), vertices_array_, BufferUsage::kStaticDraw);
            if (vertex_buffer_ == nullptr) return false;
            
            renderer_->AddIndexBuffer(index_buffer_, num_indices_, index_size_, indices_array_, BufferUsage::kStaticDraw);
            if (index_buffer_ == nullptr) return false;
            
            const char* base = (char*)0;
            for (u32 i = 0; i < attribs_.size(); ++i)
            {
                renderer_->context()->VertexAttribPointer(i, vertex_format_->generic_[i].size, DataType::kFloat, vertex_format_->vertex_size(), base + vertex_format_->generic_[i].offset);
                renderer_->context()->EnableVertexAttribArray(i);
            }
            
            renderer_->context()->BindVertexArrayObject(0);
            
            FreeArrays();
            
            return true;
        }
        bool Model::HasTexture() const
        {
            for (const auto &a : attribs_)
                if (a.type == VertexAttribute::kTexcoord)
                    return true;
            return false;
        }
        void Model::Render()
        {
            renderer_->context()->BindVertexArrayObject(vertex_array_object_);
            renderer_->context()->DrawElements(primitive_mode_, num_indices_, index_data_type_);
        }

    } // namespace graphics
} // namespace sht