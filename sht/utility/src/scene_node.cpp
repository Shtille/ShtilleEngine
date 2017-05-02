#include "../include/scene_node.h"

#include <algorithm>

namespace sht {
	namespace utility {

		SceneNode::SceneNode()
		: parent_(nullptr)
		, children_()
		{

		}
		SceneNode::~SceneNode()
		{
			for (auto& child : children_)
				delete child;
		}
		void SceneNode::Update()
		{

		}
		void SceneNode::Render()
		{
			
		}
		void SceneNode::RenderAll()
		{
			this->Render();
			for (auto child : children_)
			{
				child->Render();
			}
		}
		void SceneNode::Attach(SceneNode * node)
		{
			children_.push_back(node);
			node->parent_ = this;
		}
		void SceneNode::Detach(SceneNode * node)
		{
			for (auto it = children_.begin(); it != children_.end(); ++it)
				if (*it == node)
				{
					children_.erase(it);
					break;
				}
			node->parent_ = nullptr;
		}
		void SceneNode::Sort(SortPredicate predicate)
		{
			std::stable_sort(children_.begin(), children_.end(), predicate);
		}
		size_t SceneNode::hash()
		{
			return reinterpret_cast<size_t>(this);
		}
		SceneNode * SceneNode::parent()
		{
			return parent_;
		}
		SceneNode * SceneNode::child(size_t index)
		{
			return children_.at(index);
		}
		size_t SceneNode::num_children()
		{
			return children_.size();
		}
		void SceneNode::OnEvent(const Event *)
		{
		}

	} // namespace utility
} // namespace sht