#pragma once
#ifndef __SHT_UTILITY_SCENE_NODE_H__
#define __SHT_UTILITY_SCENE_NODE_H__

#include "event_listener.h"

#include <vector>

namespace sht {
	namespace utility {

		class SceneNode : public EventListenerInterface {
		public:
			SceneNode();
			virtual ~SceneNode();

			virtual void Update();
			virtual void Render();

			void RenderAll();

			void Attach(SceneNode * node);
			void Detach(SceneNode * node);

			typedef bool (*SortPredicate)(SceneNode *, SceneNode *);
			void Sort(SortPredicate predicate);

			virtual size_t hash();

			SceneNode * parent();
			SceneNode * child(size_t index);
			size_t num_children();

			virtual void OnEvent(const Event * event) override;

		private:
			SceneNode * parent_;
			std::vector<SceneNode *> children_;
		};

	} // namespace utility
} // namespace sht

#endif