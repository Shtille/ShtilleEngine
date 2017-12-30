#pragma once
#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "animation.h"
#include "pose_listener.h"

#include "utility/include/scene/scene.h"
#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"
#include "graphics/include/model/model.h"
#include "graphics/include/model/complex_mesh.h"

class LogoScene : public sht::utility::Scene {
public:
	LogoScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener);
	virtual ~LogoScene();

	void Update();
	void Render();

	void Load();
	void Unload();

private:
	void BindShaderConstants();
	void RenderLogotype();
	void BakeCubemaps();

	sht::utility::EventListenerInterface * event_listener_;
	float time_;
	float opacity_; //!< opacity for rendering
	sht::math::Matrix4 projection_view_matrix_;
	vec3 camera_position_;
	vec3 logotype_position_;

	sht::utility::ResourceID logotype_shader_id_;
	sht::utility::ResourceID irradiance_shader_id_;
	sht::utility::ResourceID prefilter_shader_id_;
	sht::utility::ResourceID fg_texture_id_;
	sht::utility::ResourceID env_texture_id_;
	sht::utility::ResourceID logotype_mesh_id_;

	sht::graphics::Shader * logotype_shader_;
	sht::graphics::Shader * irradiance_shader_;
	sht::graphics::Shader * prefilter_shader_;

	sht::graphics::Texture * fg_texture_;
	sht::graphics::Texture * env_texture_;
	sht::graphics::Texture * irradiance_rt_;
	sht::graphics::Texture * prefilter_rt_;

	sht::graphics::Model * quad_;

	sht::graphics::ComplexMesh * logotype_mesh_;
};

#endif