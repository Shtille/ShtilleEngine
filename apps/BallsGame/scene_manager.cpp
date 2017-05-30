#include "scene_manager.h"
#include "scene.h"
#include "resource_manager.h"

SceneManager::SceneManager()
: current_scene_(nullptr)
, next_scene_(nullptr)
, loading_scene_(nullptr)
, listener_(nullptr)
, transition_phase_(0)
, transition_type_(Transition::kNone)
{

}
SceneManager::~SceneManager()
{

}
void SceneManager::Update()
{
	UpdateTransition();
	if (loading_scene_)
		loading_scene_->Update();
	else if (current_scene_)
		current_scene_->Update();
}
void SceneManager::Render()
{
	if (loading_scene_)
		loading_scene_->Render();
	else if (current_scene_)
		current_scene_->Render();
}
void SceneManager::RequestImmediateTransition(Scene * scene)
{
	next_scene_ = scene;
	loading_scene_ = nullptr;
	listener_ = nullptr;
	transition_type_ = Transition::kImmediate;
}
void SceneManager::RequestDeferredTransition(Scene * scene, Scene * loading_scene, SceneTransitionListenerInterface * listener)
{
	next_scene_ = scene;
	loading_scene_ = loading_scene;
	listener_ = listener;
	transition_type_ = Transition::kDeferred;
}
void SceneManager::ImmediateTransition()
{
	if (current_scene_)
		current_scene_->RequestUnload();
	if (next_scene_)
		next_scene_->RequestLoad();
	ResourceManager::GetInstance()->Perform();
	if (current_scene_)
		current_scene_->Unload();
	if (next_scene_)
		next_scene_->Load();
}
bool SceneManager::DeferredTransition()
{
	ResourceManager * resource_manager = ResourceManager::GetInstance();
	switch (transition_phase_)
	{
	case 0:
		if (current_scene_)
			current_scene_->RequestUnload();
		if (next_scene_)
			next_scene_->RequestLoad();
		++transition_phase_;
		if (listener_)
			listener_->OnObtainCount(resource_manager->GetResourcesCountToProcess());
		return false;
	case 1:
		if (resource_manager->PerformStep())
			++transition_phase_;
		if (listener_)
			listener_->OnStep();
		return false;
	case 2:
		// User overriden functions
		if (current_scene_)
			current_scene_->Unload();
		if (next_scene_)
			next_scene_->Load();
		++transition_phase_;
		return false;
	case 3:
		// Transition has been completed
		transition_phase_ = 0;
		return true;
	}
}
void SceneManager::UpdateTransition()
{
	if (transition_type_ != Transition::kNone)
	{
		if (transition_type_ == Transition::kImmediate)
		{
			ImmediateTransition();
			current_scene_ = next_scene_;
			next_scene_ = nullptr;
			// Immediate transition doesn't use a listener
			transition_type_ = Transition::kNone;
		}
		else if (transition_type_ == Transition::kDeferred)
		{
			if (DeferredTransition())
			{
				current_scene_ = next_scene_;
				next_scene_ = nullptr;
				loading_scene_ = nullptr;
				listener_ = nullptr;
				transition_type_ = Transition::kNone;
			}
		}
		else if (transition_type_ == Transition::kAsyncronous)
		{
			// Haven't been done yet
		}
	}
}