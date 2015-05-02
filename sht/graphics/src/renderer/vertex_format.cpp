#include "../../include/renderer/vertex_format.h"
#include <memory.h>
#include <assert.h>

namespace sht {
	namespace graphics {

		VertexFormat::VertexFormat()
		{
			memset(this, 0, sizeof(VertexFormat));
		}
		VertexFormat::~VertexFormat()
		{

		}
		void VertexFormat::Fill(VertexAttribute *attribs, u32 num_attribs)
		{
			/*
			A GLSL shader can't bind both gl_Vertex and generic attribute 0, so if a vertex
			array is present in the provided format, we'll start on generic attribute 1.
			*/
			for (u32 i = 0; i < num_attribs; ++i)
				if (attribs[i].type == VertexAttribute::kVertex)
				{
					max_generic_ = 1;
					break;
				}
			// Loop through and add all arrays to the format
			for (u32 i = 0; i < num_attribs; ++i)
			{
				switch (attribs[i].type)
				{
				case VertexAttribute::kGeneric:
				case VertexAttribute::kTangent:
				case VertexAttribute::kBinormal:
					generic_[i].size = attribs[i].size;
					generic_[i].offset = vertex_size_;
					++max_generic_;
					break;
				case VertexAttribute::kVertex:
					vertex_.size = attribs[i].size;
					vertex_.offset = vertex_size_;
					break;
				case VertexAttribute::kTexcoord:
					texcoord_[max_texcoord_].size = attribs[i].size;
					texcoord_[max_texcoord_].offset = vertex_size_;
					++max_texcoord_;
					break;
				case VertexAttribute::kNormal:
					normal_.size = attribs[i].size;
					normal_.offset = vertex_size_;
					break;
				case VertexAttribute::kColor:
					color_.size = attribs[i].size;
					color_.offset = vertex_size_;
					break;
				default:
					assert(!"Unkown vertex attribute");
				}

				vertex_size_ += attribs[i].size * sizeof(float);
			}
		}

	} // namespace graphics
} // namespace sht