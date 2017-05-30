#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "resource_manager.h"

#include <vector>

class Scene {
public:
	Scene();
	virtual ~Scene();

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void Load();
	virtual void Unload();

	void RequestLoad();
	void RequestUnload();

protected:
	ResourceID AddResourceIdByName(StringId string_id);

	std::vector<ResourceID> resources_;
};

#endif