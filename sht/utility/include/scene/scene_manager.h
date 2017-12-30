#pragma once
#ifndef __SHT_UTILITY_SCENE_MANAGER_H__
#define __SHT_UTILITY_SCENE_MANAGER_H__

#include "scene_transition_listener.h"
#include "utility/include/event_listener.h"
#include "system/include/keys.h"
#include "system/include/mouse.h"

namespace sht {
	namespace utility {

		class Scene;

		enum class Transition {
			kNone,
			kImmediate,
			kDeferred,
			kAsyncronous
		};

		class SceneManager : public sht::utility::EventListenerInterface {
		public:
			SceneManager();
			virtual ~SceneManager();

			void Update();
			void UpdatePhysics(float sec);
			void Render();

			// Desktop-based user input message processing
			void OnChar(unsigned short code);
			void OnKeyDown(sht::PublicKey key, int mods);
			void OnMouseDown(sht::MouseButton button, int modifiers, float x, float y);
			void OnMouseUp(sht::MouseButton button, int modifiers, float x, float y);
			void OnMouseMove(float x, float y);

		protected:
			void RequestImmediateTransition(Scene * scene);
			void RequestDeferredTransition(Scene * scene, Scene * loading_scene, SceneTransitionListenerInterface * listener);

		private:
			void UpdateTransition();
			bool ImmediateTransition();
			bool DeferredTransition();

			Scene * current_scene_;
			Scene * next_scene_;
			Scene * loading_scene_;
			SceneTransitionListenerInterface * listener_;
			int transition_phase_;
			int render_count_;								//!< number of render calls after transition
			Transition transition_type_;
			bool transition_finalization_;					//!< whether we need to do some render calls after transition
		};

	} // namespace utility
} // namespace sht

#endif