#include "logo_scene.h"

#include "graphics/include/model/skybox_quad_model.h"
#include "utility/include/event.h"
#include "system/include/time/time_manager.h"

LogoScene::LogoScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener)
: Scene(renderer)
, event_listener_(event_listener)
, time_(0.0f)
, opacity_(0.0f)
, irradiance_rt_(nullptr)
, prefilter_rt_(nullptr)
, quad_(nullptr)
{
	// Register resources to load automatically on scene change
	logotype_shader_id_		= AddResourceIdByName(ConstexprStringId("shader_logotype"));
	irradiance_shader_id_	= AddResourceIdByName(ConstexprStringId("shader_irradiance"));
	prefilter_shader_id_	= AddResourceIdByName(ConstexprStringId("shader_prefilter"));
	fg_texture_id_			= AddResourceIdByName(ConstexprStringId("texture_fg"));
	env_texture_id_			= AddResourceIdByName(ConstexprStringId("texture_env"));
	logotype_mesh_id_		= AddResourceIdByName(ConstexprStringId("mesh_logotype"));
}
LogoScene::~LogoScene()
{
	Unload();
}
void LogoScene::BindShaderConstants()
{
	logotype_shader_->Bind();
	logotype_shader_->Uniform3fv("u_camera.position", camera_position_);
	logotype_shader_->Uniform3f("u_light.color", 1.0f, 1.0f, 1.0f);
	logotype_shader_->Uniform3f("u_light.direction", 1.0f, 1.0f, -1.0f);

	logotype_shader_->Uniform1i("u_diffuse_env_sampler", 0);
	logotype_shader_->Uniform1i("u_specular_env_sampler", 1);
	logotype_shader_->Uniform1i("u_preintegrated_fg_sampler", 2);
	logotype_shader_->Uniform4f("u_albedo_color", 1.0f, 1.0f, 1.0f, 1.0f);
	logotype_shader_->Uniform1f("u_roughness", 0.2f);
	logotype_shader_->Uniform1f("u_metallic", 0.9f);
	logotype_shader_->Unbind();
}
void LogoScene::Update()
{
	// Set opacity value
	const float kTimerInterval = 7.0f;
	const float kFadeInInterval = 2.0f;
	const float kFadeOutInterval = 2.0f;
	if (time_ < kFadeInInterval)
		opacity_ = time_ / kFadeInInterval;
	else if (time_ >= kFadeInInterval && time_ < kTimerInterval - kFadeOutInterval)
		opacity_ = 1.0f;
	else if (time_ >= kTimerInterval - kFadeOutInterval && time_ < kTimerInterval)
		opacity_ = (kTimerInterval - time_) / kFadeOutInterval;
	else
		opacity_ = 0.0f;

	if (time_ >= kTimerInterval)
	{
		time_ = 0.0f;
		sht::utility::Event event(ConstexprStringId("logotype_scene_finished"));
		event_listener_->OnEvent(&event);
	}
	// Finally
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_time = time_manager->GetFixedFrameTime();
	time_ += frame_time;

	projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();
}
void LogoScene::RenderLogotype()
{
	renderer_->ChangeTexture(irradiance_rt_, 0);
	renderer_->ChangeTexture(prefilter_rt_, 1);
	renderer_->ChangeTexture(fg_texture_, 2);

	logotype_shader_->Bind();
	logotype_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
	logotype_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	logotype_shader_->Uniform1f("u_opacity", opacity_);
	logotype_mesh_->Render();
	logotype_shader_->Unbind();

	renderer_->ChangeTexture(nullptr, 2);
	renderer_->ChangeTexture(nullptr, 1);
	renderer_->ChangeTexture(nullptr, 0);
}
void LogoScene::Render()
{
	RenderLogotype();
}
void LogoScene::Load()
{
	// Link resources IDs with its resource pointers
	sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();
	logotype_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(logotype_shader_id_));
	irradiance_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(irradiance_shader_id_));
	prefilter_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(prefilter_shader_id_));
	fg_texture_ = dynamic_cast<sht::graphics::Texture *>(resource_manager->GetResource(fg_texture_id_));
	env_texture_ = dynamic_cast<sht::graphics::Texture *>(resource_manager->GetResource(env_texture_id_));
	logotype_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(logotype_mesh_id_));

	// Screen quad model
	quad_ = new sht::graphics::SkyboxQuadModel(renderer_);
	quad_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	quad_->Create();
	quad_->MakeRenderable();

	// Render targets
	renderer_->CreateTextureCubemap(irradiance_rt_, 32, 32, sht::graphics::Image::Format::kRGB8, sht::graphics::Texture::Filter::kLinear);
	renderer_->CreateTextureCubemap(prefilter_rt_, 512, 512, sht::graphics::Image::Format::kRGB8, sht::graphics::Texture::Filter::kTrilinear);
	renderer_->GenerateMipmap(prefilter_rt_);

	// Create mesh
	logotype_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	logotype_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	logotype_mesh_->MakeRenderable();

	camera_position_.Set(0.0f, 0.0f, 100.0f);
	logotype_position_.Set(0.0f, 0.0f, 0.0f);
	renderer_->SetViewMatrix(sht::math::LookAt(camera_position_, logotype_position_));

	BindShaderConstants();

	BakeCubemaps();
}
void LogoScene::Unload()
{
	if (quad_)
	{
		delete quad_;
		quad_ = nullptr;
	}
	if (irradiance_rt_)
	{
		renderer_->DeleteTexture(irradiance_rt_);
		irradiance_rt_ = nullptr;
	}
	if (prefilter_rt_)
	{
		renderer_->DeleteTexture(prefilter_rt_);
		prefilter_rt_ = nullptr;
	}
}
void LogoScene::BakeCubemaps()
{
	renderer_->DisableDepthTest();

	// Irradiance cubemap
	renderer_->ChangeTexture(env_texture_);
	irradiance_shader_->Bind();
	sht::math::Matrix4 projection_matrix = sht::math::PerspectiveMatrix(90.0f, 512, 512, 0.1f, 100.0f);
	irradiance_shader_->Uniform1i("u_texture", 0);
	irradiance_shader_->UniformMatrix4fv("u_projection", projection_matrix);
	for (int face = 0; face < 6; ++face)
	{
		sht::math::Matrix4 view_matrix = sht::math::LookAtCube(vec3(0.0f), face);
		irradiance_shader_->UniformMatrix4fv("u_view", view_matrix);
		renderer_->ChangeRenderTargetsToCube(1, &irradiance_rt_, nullptr, face, 0);
		renderer_->ClearColorBuffer();
		quad_->Render();
	}
	renderer_->ChangeRenderTarget(nullptr, nullptr); // back to main framebuffer
	irradiance_shader_->Unbind();
	renderer_->ChangeTexture(nullptr);

	// Prefilter cubemap
	renderer_->ChangeTexture(env_texture_);
	prefilter_shader_->Bind();
	projection_matrix = sht::math::PerspectiveMatrix(90.0f, 512, 512, 0.1f, 100.0f);
	prefilter_shader_->Uniform1i("u_texture", 0);
	//prefilter_shader_->Uniform1f("u_cube_resolution", (float)prefilter_rt_->width());
	prefilter_shader_->UniformMatrix4fv("u_projection", projection_matrix);
	const int kMaxMipLevels = 5;
	for (int mip = 0; mip < kMaxMipLevels; ++mip)
	{
		float roughness = (float)mip / (float)(kMaxMipLevels - 1);
		prefilter_shader_->Uniform1f("u_roughness", roughness);
		for (int face = 0; face < 6; ++face)
		{
			sht::math::Matrix4 view_matrix = sht::math::LookAtCube(vec3(0.0f), face);
			prefilter_shader_->UniformMatrix4fv("u_view", view_matrix);
			renderer_->ChangeRenderTargetsToCube(1, &prefilter_rt_, nullptr, face, mip);
			renderer_->ClearColorBuffer();
			quad_->Render();
		}
	}
	renderer_->ChangeRenderTarget(nullptr, nullptr); // back to main framebuffer
	prefilter_shader_->Unbind();
	renderer_->ChangeTexture(nullptr);

	renderer_->EnableDepthTest();
}