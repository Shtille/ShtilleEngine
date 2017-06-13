#pragma once
#ifndef __SHT_GRAPHICS_COMPLEX_MESH_H__
#define __SHT_GRAPHICS_COMPLEX_MESH_H__

#include "../resource.h"
#include "../renderer/vertex_format.h"
#include "math/bounding_box.h"

#include <vector>

namespace sht {
	namespace graphics {

		class Renderer;
		class Mesh;
		struct Material;
		class MaterialBinderInterface;
		
		//! Complex mesh class
		class ComplexMesh : public Resource {
		public:

			enum class FileFormat {
				kUnknown,
				kObj
			};

			ComplexMesh(Renderer * renderer, MaterialBinderInterface * material_binder);
			virtual ~ComplexMesh();

			bool SaveToFile(const char* filename);
			bool LoadFromFile(const char* filename);
			
			virtual void Create();

			void AddFormat(const VertexAttribute& attrib);
			bool MakeRenderable();
			
			void Render();

			const math::BoundingBox& bounding_box() const;
			
		private:
			// Save routines
			bool SaveToFileObj(const char *filename);

			// Load routines
			bool LoadFromFileObj(const char *filename);

			Renderer * renderer_;
			MaterialBinderInterface * material_binder_;
			VertexFormat * vertex_format_;
			math::BoundingBox bounding_box_;

			std::vector<VertexAttribute> attribs_;
			std::vector<Mesh*> meshes_;
			std::vector<Material> materials_;
		};
		
	}
}

#endif