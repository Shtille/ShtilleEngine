#pragma once
#ifndef __SHT_UI_RENDERABLE_H__
#define __SHT_UI_RENDERABLE_H__

namespace sht {
	namespace utility {
		namespace ui {

			//! Renderable class
			class IRenderable {
			public:
				virtual ~IRenderable() = default;
				
				virtual void Render() = 0;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif