#pragma once
#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "utility/include/event_listener.h"

class Scene;

class SceneTransitionListenerInterface {
public:
	virtual ~SceneTransitionListenerInterface() = default;

	virtual void OnObtainCount(int count) = 0;
	virtual void OnStep() = 0;
};

enum class Transition {
	kNone,
	kImmediate,
	kDeferred,
	kAsyncronous
};

class SceneManager : public EventListenerInterface {
public:
	SceneManager();
	virtual ~SceneManager();

	void Update();
	void Render();

protected:
	void RequestImmediateTransition(Scene * scene);
	void RequestDeferredTransition(Scene * scene, Scene * loading_scene, SceneTransitionListenerInterface * listener);

private:
	void UpdateTransition();
	void ImmediateTransition();
	bool DeferredTransition();

	Scene * current_scene_;
	Scene * next_scene_;
	Scene * loading_scene_;
	SceneTransitionListenerInterface * listener_;
	int transition_phase_;
	Transition transition_type_;
};

#endif