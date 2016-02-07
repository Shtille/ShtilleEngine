#pragma once
#ifndef __SHT_UI_WIDGET_H__
#define __SHT_UI_WIDGET_H__

#include "../../../common/types.h"
#include "../../../math/vector.h"

#include <list>

namespace sht {
	namespace utility {
		namespace ui {

			enum class Flags : u32 {
				kRenderNever	= 0,
				kRenderAlways	= 1,
				kRenderIfActive	= 2,
				kSelectable		= 4,
				kDraggable		= 8,
				kDroppable		= 16
			};

			//! Base class for any widget
            /*!
             We won't use any rotation, so we dont need to store transformation matrix.
             But anyway i gonna think about that.
            */
			class Widget {
			public:
				void set_position(const vec2& position);
				void set_active(bool active);
				void set_enabled(bool enabled);

				void AttachWidget(Widget * widget);
				void DetachWidget(Widget * widget);

				virtual void Update(f32 sec); //!< update this widget only
                void UpdateAll(f32 sec); //!< update entire tree starting from this widget
                virtual void Render();
                void RenderAll();
                
                void ActivateAll();
                void DeactivateAll();
                
                bool HasFlag(Flags flag);
                
                void ObtainGlobalPosition(vec2 * position);

			protected:
				Widget(f32 x, f32 y, u32 flags);
				virtual ~Widget();
                
                virtual void BindConstUniforms();

                Widget * parent_;
				std::list<Widget*> nodes_; //!< child nodes
				vec2 position_;
				u32 flags_;
				bool active_;
				bool enabled_;

			private:
				// Do we need to copy it?
				Widget(const Widget&) = delete;
				Widget& operator =(const Widget&) = delete;
			};

		} // namespace ui
	} // namespace utility

	namespace ui = utility::ui;
} // namespace sht

#endif