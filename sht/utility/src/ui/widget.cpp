#include "../../include/ui/widget.h"

#include "../../include/ui/rect.h"

namespace sht {
	namespace utility {
		namespace ui {

			Widget::Widget()
			: parent_(nullptr)
			, position_(0.0f, 0.0f)
			, flags_((u32)Flags::kRenderNever)
			, active_(false)
			, enabled_(true)
			{
				
			}
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
			bool Widget::active() const
			{
				return active_;
			}
			bool Widget::enabled() const
			{
				return enabled_;
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
			void Widget::Update(f32)
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
				
				if ((HasFlag(Flags::kRenderAlways)) ||
					(HasFlag(Flags::kRenderIfActive) && active_))
					Render();
				
				for (auto &child : nodes_)
				{
					child->RenderAll();
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
			void Widget::Select(float x, float y)
			{
				if (HasFlag(Flags::kSelectable))
				{
					Rect * rect = dynamic_cast<Rect*>(this);
					if (rect)
						active_ = rect->IsInsideGlobal(x, y);
				}
			}
			void Widget::SelectAll(float x, float y)
			{
				Select(x, y);
				
				for (auto &w : nodes_)
				{
					w->SelectAll(x, y);
				}
			}
			bool Widget::HasFlag(Flags flag)
			{
				return (flags_ & (u32)flag) != 0;
			}
			bool Widget::BeginChildSearch()
			{
				if (!nodes_.empty())
				{
					child_iterator_ = nodes_.begin();
					return true;
				}
				else
					return false;
			}
			void Widget::EndChildSearch()
			{
			}
			bool Widget::GetNextChild(Widget *& child)
			{
				if (child_iterator_ != nodes_.end())
				{
					child = *child_iterator_;
					++child_iterator_;
					return true;
				}
				else
				{
					child = nullptr;
					return false;
				}
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

		} // namespace ui
	} // namespace utility
} // namespace sht