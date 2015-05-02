#pragma once
#ifndef __SHT_GRAPHICS_RENDERER_VERTEX_FORMAT_H__
#define __SHT_GRAPHICS_RENDERER_VERTEX_FORMAT_H__

#include "../../../common/types.h"

namespace sht {
	namespace graphics {

		const int kMaxGeneric = 16;
		const int kMaxTexcoord = 8;

		//! Vertex attribute struct
		struct VertexAttribute {

			//! Vertex attrib types
			enum Type {
				kGeneric,
				kVertex,
				kNormal,
				kTexcoord,
				kColor,
				kTangent,
				kBinormal
			};

			Type type;			//!< Specifies the vertex type.
			u32 size;			//!< Specifies the vertex format size.
		};

		//! Vertex format class
		class VertexFormat {
			friend class Renderer;
			friend class OpenGlRenderer;

		public:
			struct Attrib {
				int offset;
				int size;
			};

		protected:
			VertexFormat();
			virtual ~VertexFormat();
			VertexFormat(const VertexFormat&) = delete;
			void operator = (const VertexFormat&) = delete;

			void Fill(VertexAttribute *attribs, u32 num_attribs);

		private:
			Attrib generic_[kMaxGeneric];
			Attrib texcoord_[kMaxTexcoord];
			Attrib vertex_;
			Attrib normal_;
			Attrib color_;

			u32 vertex_size_;
			u8 max_generic_;
			u8 max_texcoord_;
		};

	} // namespace graphics
} // namespace sht

#endif