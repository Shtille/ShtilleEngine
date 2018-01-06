#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/model/screen_quad_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"
#include <cmath>

/*
PBR shader to use in application
*/
#define USE_PBR 3

#define APP_NAME PbrApp

class APP_NAME : public sht::OpenGlApplication
{
public:
	APP_NAME()
	: sphere_(nullptr)
	, quad_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, camera_manager_(nullptr)
	, need_update_projection_matrix_(true)
	{
	}
	const char* GetTitle() final
	{
		return "Physics Based Rendering";
	}
	const bool IsMultisample() final
	{
		return true;
	}
	void BindShaderConstants()
	{
		env_shader_->Bind();
		env_shader_->Uniform1i("u_texture", 0);

		object_shader_->Bind();
#if USE_PBR == 1
		object_shader_->Uniform3f("u_light_direction", 1.0f, 1.0f, -1.0f);
		object_shader_->Uniform1i("u_env_sampler", 0);
		object_shader_->Uniform1i("u_albedo_sampler", 1);
		object_shader_->Uniform1i("u_normal_sampler", 2);
		object_shader_->Uniform1i("u_roughness_sampler", 3);
		object_shader_->Uniform1i("u_metal_sampler", 4);
#elif USE_PBR == 2
		object_shader_->Uniform3f("u_light.color", 1.0f, 1.0f, 1.0f);
		object_shader_->Uniform3f("u_light.direction", 1.0f, 1.0f, -1.0f);
		object_shader_->Uniform1f("u_light.intensity", 1.0f);

		object_shader_->Uniform1i("u_preintegrated_fg_sampler", 0);
		object_shader_->Uniform1i("u_environment_sampler", 1);
		object_shader_->Uniform1i("u_albedo_sampler", 2);
		object_shader_->Uniform1i("u_normal_sampler", 3);
		object_shader_->Uniform1i("u_roughness_sampler", 4);
		//object_shader_->Uniform1i("u_metal_sampler", 5);

		object_shader_->Uniform4f("u_albedo_color", 1.0f, 1.0f, 1.0f, 1.0f);
		object_shader_->Uniform3f("u_specular_color", 1.0f, 1.0f, 1.0f);
		object_shader_->Uniform1f("u_roughness_color", 1.0f);
		//object_shader_->Uniform1f("u_metalness_color", 1.0f);

		object_shader_->Uniform1f("u_using_albedo_map", 1.0f);
		object_shader_->Uniform1i("u_using_normal_map", 1);
		object_shader_->Uniform1f("u_using_roughness_map", 1.0f);
		//object_shader_->Uniform1f("u_using_metal_map", 1.0f);
#elif USE_PBR == 3
		object_shader_->Uniform3f("u_light.color", 1.0f, 1.0f, 1.0f);
		object_shader_->Uniform3f("u_light.direction", 1.0f, 1.0f, -1.0f);

		object_shader_->Uniform1i("u_diffuse_env_sampler", 0);
		object_shader_->Uniform1i("u_specular_env_sampler", 1);
		object_shader_->Uniform1i("u_preintegrated_fg_sampler", 2);
		object_shader_->Uniform1i("u_albedo_sampler", 3);
		object_shader_->Uniform1i("u_normal_sampler", 4);
		object_shader_->Uniform1i("u_roughness_sampler", 5);
		object_shader_->Uniform1i("u_metal_sampler", 6);
#else
#error PBR version hasn't been defined
#endif
		object_shader_->Unbind();
	}
	void BindShaderVariables()
	{
	}
	bool Load() final
	{
		// Sphere model
		sphere_ = new sht::graphics::SphereModel(renderer_, 128, 64);
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTangent, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kBinormal, 3));
		sphere_->Create();
		if (!sphere_->MakeRenderable())
			return false;

		// Screen quad model
		quad_ = new sht::graphics::ScreenQuadModel(renderer_);
		quad_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		quad_->Create();
		if (!quad_->MakeRenderable())
			return false;
		
		// Load shaders
		if (!renderer_->AddShader(text_shader_, "data/shaders/text")) return false;
		if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored")) return false;
		if (!renderer_->AddShader(env_shader_, "data/shaders/skybox")) return false;
		if (!renderer_->AddShader(irradiance_shader_, "data/shaders/pbr/irradiance")) return false;
		if (!renderer_->AddShader(prefilter_shader_, "data/shaders/pbr/prefilter")) return false;
		if (!renderer_->AddShader(integrate_shader_, "data/shaders/pbr/integrate")) return false;
#if USE_PBR == 1
		if (!renderer_->AddShader(object_shader_, "data/shaders/apps/PBR/object_pbr")) return false;
#elif USE_PBR == 2
		if (!renderer_->AddShader(object_shader_, "data/shaders/apps/PBR/object_pbr2")) return false;
#elif USE_PBR == 3
		if (!renderer_->AddShader(object_shader_, "data/shaders/apps/PBR/pbr")) return false;
#else
#error PBR version hasn't been defined
#endif
		
		// Load textures
		const char * cubemap_filenames[6] = {
			"data/textures/skybox/ashcanyon_ft.jpg",
			"data/textures/skybox/ashcanyon_bk.jpg",
			"data/textures/skybox/ashcanyon_up.jpg",
			"data/textures/skybox/ashcanyon_dn.jpg",
			"data/textures/skybox/ashcanyon_rt.jpg",
			"data/textures/skybox/ashcanyon_lf.jpg"
		};
		if (!renderer_->AddTextureCubemap(env_texture_, cubemap_filenames)) return false;
		if (!renderer_->AddTexture(albedo_texture_, "data/textures/pbr/metal/rusted_iron/albedo.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(normal_texture_, "data/textures/pbr/metal/rusted_iron/normal.png",
		 						   sht::graphics::Texture::Wrap::kClampToEdge,
		 						   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(roughness_texture_, "data/textures/pbr/metal/rusted_iron/roughness.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(metal_texture_, "data/textures/pbr/metal/rusted_iron/metallic.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(fg_texture_, "data/textures/pbr/brdfLUT.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;

		// Render targets
		renderer_->CreateTextureCubemap(irradiance_rt_, 32, 32, sht::graphics::Image::Format::kRGB8, sht::graphics::Texture::Filter::kLinear);
		renderer_->CreateTextureCubemap(prefilter_rt_, 512, 512, sht::graphics::Image::Format::kRGB8, sht::graphics::Texture::Filter::kTrilinear);
		renderer_->GenerateMipmap(prefilter_rt_);
		renderer_->AddRenderTarget(integrate_rt_, 512, 512, sht::graphics::Image::Format::kRGB8); // RG16

		renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
		if (font_ == nullptr)
			return false;

		fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
		if (!fps_text_)
			return false;

		camera_manager_ = new sht::utility::CameraManager();
		camera_manager_->MakeFree(vec3(5.0f), vec3(0.0f));

		// Finally bind constants
		BindShaderConstants();

		BakeCubemaps();
		
		return true;
	}
	void Unload() final
	{
		if (camera_manager_)
			delete camera_manager_;
		if (fps_text_)
			delete fps_text_;
		if (quad_)
			delete quad_;
		if (sphere_)
			delete sphere_;
	}
	void Update() final
	{
		const float kFrameTime = GetFrameTime();

		camera_manager_->Update(kFrameTime);

		// Update matrices
		renderer_->SetViewMatrix(camera_manager_->view_matrix());
		UpdateProjectionMatrix();
		projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();

		BindShaderVariables();
	}
	void BakeCubemaps()
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

		// Integrate LUT
		integrate_shader_->Bind();
		renderer_->ChangeRenderTarget(integrate_rt_, nullptr);
		renderer_->ClearColorBuffer();
		quad_->Render();
		renderer_->ChangeRenderTarget(nullptr, nullptr); // back to main framebuffer
		integrate_shader_->Unbind();

		renderer_->EnableDepthTest();
	}
	void RenderEnvironment()
	{
		renderer_->DisableDepthTest();

		renderer_->ChangeTexture(env_texture_);
		env_shader_->Bind();
		env_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
		env_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
		quad_->Render();
		env_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		renderer_->EnableDepthTest();
	}
	void RenderObjects()
	{
		renderer_->PushMatrix();
		renderer_->Translate(vec3(0.0f));

#if USE_PBR == 1
		renderer_->ChangeTexture(env_texture_, 0);
		renderer_->ChangeTexture(albedo_texture_, 1);
		renderer_->ChangeTexture(normal_texture_, 2);
		renderer_->ChangeTexture(roughness_texture_, 3);
		renderer_->ChangeTexture(metal_texture_, 4);
#elif USE_PBR == 2
		renderer_->ChangeTexture(fg_texture_, 0);
		renderer_->ChangeTexture(env_texture_, 1);
		renderer_->ChangeTexture(albedo_texture_, 2);
		renderer_->ChangeTexture(normal_texture_, 3);
		renderer_->ChangeTexture(roughness_texture_, 4);
		//renderer_->ChangeTexture(metal_texture_, 5);
#elif USE_PBR == 3
		renderer_->ChangeTexture(irradiance_rt_, 0);
		renderer_->ChangeTexture(prefilter_rt_, 1);
		renderer_->ChangeTexture(fg_texture_, 2);
		renderer_->ChangeTexture(albedo_texture_, 3);
		renderer_->ChangeTexture(normal_texture_, 4);
		renderer_->ChangeTexture(roughness_texture_, 5);
		renderer_->ChangeTexture(metal_texture_, 6);
#endif
		
		object_shader_->Bind();
		object_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
		object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
#if USE_PBR == 1
		object_shader_->Uniform3fv("u_camera_position", *camera_manager_->position());
#elif USE_PBR == 2
		object_shader_->Uniform3fv("u_camera.position", *camera_manager_->position());
#elif USE_PBR == 3
		object_shader_->Uniform3fv("u_camera.position", *camera_manager_->position());
#endif
		
		sphere_->Render();
		
		object_shader_->Unbind();

#if USE_PBR == 2
		//renderer_->ChangeTexture(nullptr, 5);
#elif USE_PBR == 3
		renderer_->ChangeTexture(nullptr, 6);
		renderer_->ChangeTexture(nullptr, 5);
#endif
		renderer_->ChangeTexture(nullptr, 4);
		renderer_->ChangeTexture(nullptr, 3);
		renderer_->ChangeTexture(nullptr, 2);
		renderer_->ChangeTexture(nullptr, 1);
		renderer_->ChangeTexture(nullptr, 0);
		
		renderer_->PopMatrix();
	}
	void RenderInterface()
	{
		renderer_->DisableDepthTest();
		
		// Draw FPS
		text_shader_->Bind();
		text_shader_->Uniform1i("u_texture", 0);
		text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
		fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", GetFrameRate());
		fps_text_->Render();
		
		renderer_->EnableDepthTest();
	}
	void Render() final
	{
		renderer_->SetViewport(width_, height_);
		
		renderer_->ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		renderer_->ClearColorAndDepthBuffers();
		
		RenderEnvironment();
		RenderObjects();
		RenderInterface();
	}
	void OnKeyDown(sht::PublicKey key, int mods) final
	{
		if (key == sht::PublicKey::kF)
		{
			ToggleFullscreen();
		}
		else if (key == sht::PublicKey::kEscape)
		{
			Application::Terminate();
		}
		else if (key == sht::PublicKey::kF5)
		{
			renderer_->TakeScreenshot("screenshots");
		}
		else if (key == sht::PublicKey::kLeft)
		{
			camera_manager_->RotateAroundTargetInY(0.1f);
		}
		else if (key == sht::PublicKey::kRight)
		{
			camera_manager_->RotateAroundTargetInY(-0.1f);
		}
		else if (key == sht::PublicKey::kUp)
		{
			camera_manager_->RotateAroundTargetInZ(0.1f);
		}
		else if (key == sht::PublicKey::kDown)
		{
			camera_manager_->RotateAroundTargetInZ(-0.1f);
		}
	}
	void OnMouseDown(sht::MouseButton button, int modifiers) final
	{
	}
	void OnMouseUp(sht::MouseButton button, int modifiers) final
	{
	}
	void OnMouseMove() final
	{
	}
	void OnSize(int w, int h) final
	{
		Application::OnSize(w, h);
		// To have correct perspective when resizing
		need_update_projection_matrix_ = true;
	}
	void UpdateProjectionMatrix()
	{
		if (need_update_projection_matrix_ || camera_manager_->animated())
		{
			need_update_projection_matrix_ = false;
			renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), 0.1f, 100.0f));
		}
	}
	
private:
	sht::graphics::Model * sphere_;
	sht::graphics::Model * quad_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * gui_shader_;
	sht::graphics::Shader * env_shader_;
	sht::graphics::Shader * object_shader_;
	sht::graphics::Shader * irradiance_shader_;
	sht::graphics::Shader * prefilter_shader_;
	sht::graphics::Shader * integrate_shader_;

	sht::graphics::Texture * env_texture_;
	sht::graphics::Texture * albedo_texture_;
	sht::graphics::Texture * normal_texture_;
	sht::graphics::Texture * roughness_texture_;
	sht::graphics::Texture * metal_texture_;
	sht::graphics::Texture * fg_texture_;
	sht::graphics::Texture * irradiance_rt_;
	sht::graphics::Texture * prefilter_rt_;
	sht::graphics::Texture * integrate_rt_;

	sht::graphics::Font * font_;
	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	
	sht::math::Matrix4 projection_view_matrix_;
	
	bool need_update_projection_matrix_;
};

DECLARE_MAIN(APP_NAME);