#pragma once
#ifndef __GAME_SCENE_MANAGER_H__
#define __GAME_SCENE_MANAGER_H__

#include "utility/include/scene/scene_manager.h"
#include "utility/include/resource_manager.h"
#include "graphics/include/renderer/renderer.h"

#include <unordered_map>
#include <string>

class LoadingScene;
class MaterialBinder;

class GameSceneManager : public sht::utility::SceneManager {
	friend class IniFileReader;
public:
	GameSceneManager(sht::graphics::Renderer * renderer);
	~GameSceneManager();

	sht::graphics::Renderer * renderer();
	MaterialBinder * material_binder();

	const char* GetResourcePath(sht::utility::ResourceID id);

private:
	void RegisterAllResources();
	void AddResourcePath(sht::utility::ResourceID id, const char* path);
	void OnEvent(const sht::utility::Event * event) override;

	sht::graphics::Renderer * renderer_;
	MaterialBinder * material_binder_;
	sht::utility::Scene * logo_scene_;
	sht::utility::Scene * menu_scene_;
	LoadingScene * loading_scene_;
	sht::utility::Scene * game_scene_;

	typedef std::unordered_map<sht::utility::ResourceID, std::string> ResourceIdToPathTable;
	ResourceIdToPathTable resources_paths_;
};

#endif