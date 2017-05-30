#pragma once
#ifndef __SHT_UTILITY_SCENE_TRANSITION_LISTENER_H__
#define __SHT_UTILITY_SCENE_TRANSITION_LISTENER_H__

namespace sht {
	namespace utility {

		class SceneTransitionListenerInterface {
		public:
			virtual ~SceneTransitionListenerInterface() = default;

			virtual void OnObtainCount(int count) = 0;
			virtual void OnStep() = 0;
			virtual void OnFinish() = 0;
		};

	} // namespace utility
} // namespace sht

#endif