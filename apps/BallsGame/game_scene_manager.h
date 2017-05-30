#pragma once
#ifndef __GAME_SCENE_MANAGER_H__
#define __GAME_SCENE_MANAGER_H__

#include "scene_manager.h"
#include "resource_manager.h"

#include "graphics/include/renderer/renderer.h"

#include <unordered_map>
#include <string>

class GameSceneManager : public SceneManager {
	friend class IniFileReader;
public:
	GameSceneManager(sht::graphics::Renderer * renderer);
	~GameSceneManager();

	sht::graphics::Renderer * renderer();

private:
	void RegisterAllResources();
	const char* GetResourcePath(ResourceID id);
	void AddResourcePath(ResourceID id, const char* path);
	void OnEvent(const sht::utility::Event * event) override;

	sht::graphics::Renderer * renderer_;
	Scene * logo_scene_;
	Scene * game_scene_;

	typedef std::unordered_map<ResourceID, std::string> ResourceIdToPathTable;
	ResourceIdToPathTable resources_paths_;
};

#endif