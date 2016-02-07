#include "../../include/ui/widget.h"

namespace sht {
	namespace utility {
		namespace ui {

			Widget::Widget(f32 x, f32 y, u32 flags)
            : parent_(nullptr)
            , position_(x, y)
			, flags_(flags)
			, active_(false)
			, enabled_(true)
			{

			}
			Widget::~Widget()
			{
                for (auto &w : nodes_)
                {
                    delete w;
                }
			}
			void Widget::set_position(const vec2& position)
			{
				position_ = position;
			}
			void Widget::set_active(bool active)
			{
				active_ = active;
			}
			void Widget::set_enabled(bool enabled)
			{
				enabled_ = enabled;
			}
			void Widget::AttachWidget(Widget * widget)
			{
				nodes_.push_back(widget);
                widget->parent_ = this;
			}
			void Widget::DetachWidget(Widget * widget)
			{
				nodes_.remove(widget);
                widget->parent_ = nullptr;
			}
			void Widget::Update(f32 sec)
			{
				
			}
            void Widget::UpdateAll(f32 sec)
            {
                Update(sec);
                for (auto &w : nodes_)
                {
                    w->UpdateAll(sec);
                }
            }
            void Widget::Render()
            {
                
            }
            void Widget::RenderAll()
            {
                if (!enabled_)
                    return;
                
                if (HasFlag(Flags::kRenderAlways) ||
                    (HasFlag(Flags::kRenderIfActive) && active_))
                    Render();
                
                for (auto &w : nodes_)
                {
                    w->RenderAll();
                }
            }
            void Widget::ActivateAll()
            {
                active_ = true;
                
                for (auto &w : nodes_)
                {
                    w->ActivateAll();
                }
            }
            void Widget::DeactivateAll()
            {
                active_ = false;
                
                for (auto &w : nodes_)
                {
                    w->DeactivateAll();
                }
            }
            bool Widget::HasFlag(Flags flag)
            {
                return (flags_ & (u32)flag) != 0;
            }
            void Widget::ObtainGlobalPosition(vec2 * position)
            {
                position->x = 0.0f;
                position->y = 0.0f;
                
                Widget * widget = this;
                while (widget)
                {
                    *position += widget->position_;
                    widget = widget->parent_;
                }
            }
            void Widget::BindConstUniforms()
            {
                
            }

		} // namespace ui
	} // namespace utility
} // namespace sht