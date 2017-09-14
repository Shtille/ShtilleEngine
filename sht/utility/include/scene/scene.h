#pragma once
#ifndef __SHT_UTILITY_SCENE_H__
#define __SHT_UTILITY_SCENE_H__

#include "../resource_manager.h"

#include "graphics/include/renderer/renderer.h"
#include "system/include/keys.h"
#include "system/include/mouse.h"

#include <vector>

namespace sht {
	namespace utility {

		class Scene {
		public:
			Scene(sht::graphics::Renderer * renderer);
			virtual ~Scene();

			void SetNextScene(Scene * scene);
			Scene * next();

			virtual void Update() = 0;
			virtual void UpdatePhysics(float sec);
			virtual void Render() = 0;

			virtual void Load();
			virtual void Unload();

			void RequestLoad();
			void RequestUnload();

			// Desktop-based user input message processing
			virtual void OnChar(unsigned short code);
			virtual void OnKeyDown(sht::PublicKey key, int mods);
			virtual void OnMouseDown(sht::MouseButton button, int modifiers);
			virtual void OnMouseUp(sht::MouseButton button, int modifiers);
			virtual void OnMouseMove(float x, float y);

		protected:
			ResourceID AddResourceIdByName(sht::utility::StringId string_id);

			sht::graphics::Renderer * renderer_;
			Scene * next_;
			std::vector<ResourceID> resources_;
		};

	} // namespace utility
} // namespace sht

#endif