#pragma once
#ifndef __SHT_GRAPHICS_PHYSICAL_BOX_MODEL_H__
#define __SHT_GRAPHICS_PHYSICAL_BOX_MODEL_H__

#include "model.h"

namespace sht {
	namespace graphics {
		
		class PhysicalBoxModel final : public Model {
		public:
			PhysicalBoxModel(Renderer * renderer, float size_x, float size_y, float size_z, float size_u, float size_v);
			~PhysicalBoxModel();
			
			void Create();

			const math::Vector3& sizes() const;
			const math::Vector2& texture_sizes() const;
			
		protected:
			vec3 sizes_;
			vec2 texture_sizes_;
		};
		
	} // namespace graphics
} // namespace sht

#endif