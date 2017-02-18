#include "../include/scene_node.h"

namespace sht {
	namespace utility {

		SceneNode::SceneNode()
		: parent_(nullptr)
		, children_()
		{

		}
		SceneNode::~SceneNode()
		{

		}
		void SceneNode::Update()
		{

		}
		void SceneNode::Render()
		{
			
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

	} // namespace utility
} // namespace sht