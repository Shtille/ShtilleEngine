#include "../../include/renderer/vertex_format.h"
#include <cstring>
#include <assert.h>

namespace sht {
	namespace graphics {

		VertexFormat::VertexFormat()
        : vertex_size_(0)
        , max_generic_(0)
		{
			memset((void*)generic_, 0, sizeof(generic_));
		}
		VertexFormat::~VertexFormat()
		{
		}
        bool VertexFormat::operator == (const VertexFormat& vf)
        {
            return memcmp((void*)this, (void*)&vf, sizeof(VertexFormat)) == 0;
        }
        u32 VertexFormat::vertex_size() const
        {
            return vertex_size_;
        }
        const VertexFormat::Attrib& VertexFormat::generic(u32 index) const
        {
            return generic_[index];
        }
		void VertexFormat::Fill(VertexAttribute *attribs, u32 num_attribs)
		{
			/*
			A GLSL shader can't bind both gl_Vertex and generic attribute 0, so if a vertex
			array is present in the provided format, we'll start on generic attribute 1.
			*/
//			for (u32 i = 0; i < num_attribs; ++i)
//				if (attribs[i].type == VertexAttribute::kVertex)
//				{
//					max_generic_ = 1;
//					break;
//				}
			// Loop through and add all arrays to the format
			for (u32 i = 0; i < num_attribs; ++i)
			{
				switch (attribs[i].type)
				{
				case VertexAttribute::kGeneric:
				case VertexAttribute::kTangent:
				case VertexAttribute::kBinormal:
                case VertexAttribute::kVertex:
                case VertexAttribute::kTexcoord:
				case VertexAttribute::kNormal:
				case VertexAttribute::kColor:
					generic_[i].size = attribs[i].size;
					generic_[i].offset = vertex_size_;
					++max_generic_;
					break;
				default:
					assert(!"Unkown vertex attribute");
				}

				vertex_size_ += attribs[i].size * sizeof(float);
			}
		}

	} // namespace graphics
} // namespace sht