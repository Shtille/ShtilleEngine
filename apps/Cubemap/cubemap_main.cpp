#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/screen_quad_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"

#include <cmath>

/*
The main concept of creating this application is testing cubemap edge artefacts during accessing mipmaps.
*/

#define APP_NAME CubemapApp

class APP_NAME : public sht::OpenGlApplication
{
public:
	APP_NAME()
	: quad_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, camera_manager_(nullptr)
	, render_type_(0)
	, cubemap_lod_(0.0f)
	, need_update_projection_matrix_(true)
	{
	}
	const char* GetTitle() final
	{
		return "Cubemap test";
	}
	const bool IsMultisample() final
	{
		return true;
	}
	void BindShaderConstants()
	{
	}
	void BindShaderVariables()
	{
	}
	void ApplyLod()
	{
		if (cubemap_lod_ < 0.0f)
			cubemap_lod_ = 0.0f;
		if (cubemap_lod_ > 10.0f)
			cubemap_lod_ = 10.0f;
	}
	void ChooseRenderType()
	{
		const int kNumTypes = 2;
		render_type_ = render_type_ % kNumTypes;
		switch (render_type_)
		{
		case 0:
			fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"standard cubemap, lod = %.1f", cubemap_lod_);
			break;
		case 1:
			fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"sphere cubemap, lod = %.1f", cubemap_lod_);
			break;
		}
	}
	bool Load() final
	{
		// Quad model
		quad_ = new sht::graphics::ScreenQuadModel(renderer_);
		quad_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		quad_->Create();
		if (!quad_->MakeRenderable())
			return false;
		
		// Load shaders
		if (!renderer_->AddShader(text_shader_, "data/shaders/text")) return false;
		if (!renderer_->AddShader(env_shader_, "data/shaders/apps/Cubemap/skybox")) return false;
		if (!renderer_->AddShader(env_convert_shader_, "data/shaders/apps/Cubemap/prefilter")) return false;
		
		// Load textures
		const char * cubemap_filenames[6] = {
			"data/textures/skybox/ashcanyon_ft.jpg",
			"data/textures/skybox/ashcanyon_bk.jpg",
			"data/textures/skybox/ashcanyon_up.jpg",
			"data/textures/skybox/ashcanyon_dn.jpg",
			"data/textures/skybox/ashcanyon_rt.jpg",
			"data/textures/skybox/ashcanyon_lf.jpg"
		};
		if (!renderer_->AddTextureCubemap(standard_cubemap_texture_, cubemap_filenames, true)) return false;
		if (!renderer_->AddTexture(sphere_texture_, "data/textures/skybox/GravelPlaza_8k.jpg")) return false;

		// Render targets
		renderer_->CreateTextureCubemap(environment_rt_, 512, 512, sht::graphics::Image::Format::kRGB8, sht::graphics::Texture::Filter::kTrilinear);
		renderer_->GenerateMipmap(environment_rt_);

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

		// Generate environment
		GenerateEnvironment();

		ChooseRenderType();
		
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
	void GenerateEnvironment()
	{
		renderer_->DisableDepthTest();

		renderer_->ChangeTexture(sphere_texture_);
		env_convert_shader_->Bind();
		sht::math::Matrix4 projection_matrix = sht::math::PerspectiveMatrix(90.0f, 512, 512, 0.1f, 100.0f);
		env_convert_shader_->Uniform1i("u_texture", 0);
		env_convert_shader_->UniformMatrix4fv("u_projection", projection_matrix);
		const int kMaxMipLevels = 10; // 10 levels for 512x512 image
		for (int mip = 0; mip < kMaxMipLevels; ++mip)
		{
			for (int face = 0; face < 6; ++face)
			{
				sht::math::Matrix4 view_matrix = sht::math::LookAtCube(vec3(0.0f), face);
				env_convert_shader_->UniformMatrix4fv("u_view", view_matrix);
				//env_convert_shader_->Uniform1f("u_lod", (float)mip);
				float roughness = (float)mip / (float)kMaxMipLevels;
				env_convert_shader_->Uniform1f("u_roughness", roughness);
				renderer_->ChangeRenderTargetsToCube(1, &environment_rt_, nullptr, face, mip);
				renderer_->ClearColorBuffer();
				quad_->Render();
			}
		}
		renderer_->ChangeRenderTarget(nullptr, nullptr);
		env_convert_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		renderer_->EnableDepthTest();
	}
	void RenderEnvironment(sht::graphics::Texture * env_texture)
	{
		renderer_->DisableDepthTest();

		renderer_->ChangeTexture(env_texture);
		env_shader_->Bind();
		env_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
		env_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
		env_shader_->Uniform1i("u_texture", 0);
		env_shader_->Uniform1f("u_lod", cubemap_lod_);
		quad_->Render();
		env_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		renderer_->EnableDepthTest();
	}
	void RenderInterface()
	{
		renderer_->DisableDepthTest();
		
		// Draw FPS
		text_shader_->Bind();
		text_shader_->Uniform1i("u_texture", 0);
		text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
		fps_text_->Render();
		text_shader_->Unbind();

		renderer_->ChangeTexture(nullptr);
		
		renderer_->EnableDepthTest();
	}
	void Render() final
	{
		renderer_->SetViewport(width_, height_);
		
		renderer_->ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		renderer_->ClearColorAndDepthBuffers();
		
		switch (render_type_)
		{
		case 0:
			RenderEnvironment(standard_cubemap_texture_);
			break;
		case 1:
			RenderEnvironment(environment_rt_);
			break;
		}
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
		else if (key == sht::PublicKey::kPageUp)
		{
			cubemap_lod_ += 1.0f;
			ApplyLod();
			ChooseRenderType();
		}
		else if (key == sht::PublicKey::kPageDown)
		{
			cubemap_lod_ -= 1.0f;
			ApplyLod();
			ChooseRenderType();
		}
		else if (key == sht::PublicKey::kSpace)
		{
			++render_type_;
			ChooseRenderType();
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
	sht::graphics::Model * quad_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * env_shader_;
	sht::graphics::Shader * env_convert_shader_;

	sht::graphics::Texture * standard_cubemap_texture_;
	sht::graphics::Texture * sphere_texture_;
	sht::graphics::Texture * environment_rt_;

	sht::graphics::Font * font_;
	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	
	sht::math::Matrix4 projection_view_matrix_;
	
	int render_type_;
	float cubemap_lod_;
	bool need_update_projection_matrix_;
};

DECLARE_MAIN(APP_NAME);