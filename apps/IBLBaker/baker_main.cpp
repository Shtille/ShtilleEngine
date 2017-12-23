#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/skybox_quad_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"

#include <cmath>
#include <cstdio>

class IBLBakerApp : public sht::OpenGlApplication
{
public:
	IBLBakerApp()
	: quad_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, camera_manager_(nullptr)
	, render_type_(0)
	, need_update_projection_matrix_(true)
	{
	}
	const char* GetTitle() final
	{
		return "IBL Baker";
	}
	const bool IsMultisample() final
	{
		return true;
	}
	void BindShaderConstants()
	{
		env_shader_->Bind();
		env_shader_->Uniform1i("u_texture", 0);

		quad_shader_->Bind();
		quad_shader_->Uniform1i("u_texture", 0);
		quad_shader_->Unbind();
	}
	void BindShaderVariables()
	{
	}
	void ChooseRenderType()
	{
		const int kNumTypes = 4;
		render_type_ = render_type_ % kNumTypes;
		switch (render_type_)
		{
		case 0:
			fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"base image");
			break;
		case 1:
			fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"irradiance");
			break;
		case 2:
			fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"prefilter");
			break;
		case 3:
			fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"BRDF LUT");
			break;
		}
	}
	bool Load() final
	{
		// Quad model
		quad_ = new sht::graphics::SkyboxQuadModel(renderer_);
		quad_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		quad_->Create();
		if (!quad_->MakeRenderable())
			return false;
		
		// Load shaders
		if (!renderer_->AddShader(text_shader_, "data/shaders/text")) return false;
		if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored")) return false;
		if (!renderer_->AddShader(env_shader_, "data/shaders/apps/IBLBaker/skybox")) return false;
		if (!renderer_->AddShader(quad_shader_, "data/shaders/apps/IBLBaker/quad")) return false;
		if (!renderer_->AddShader(irradiance_shader_, "data/shaders/apps/IBLBaker/irradiance")) return false;
		if (!renderer_->AddShader(prefilter_shader_, "data/shaders/apps/IBLBaker/prefilter")) return false;
		if (!renderer_->AddShader(integrate_shader_, "data/shaders/apps/IBLBaker/integrate")) return false;
		
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

		// Render targets
		printf("width = %i, height = %i\n", width_, height_);
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

		// Bake cubemaps
		BakeCubemaps();

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
		prefilter_shader_->Uniform1f("u_cube_resolution", prefilter_rt_->width());
		prefilter_shader_->UniformMatrix4fv("u_projection", projection_matrix);
		const int maxMipLevels = 1;
		for (int mip = 0; mip < maxMipLevels; ++mip)
		{
			float roughness = (float)mip / (float)(maxMipLevels - 1);
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
		renderer_->GenerateMipmap(prefilter_rt_);

		// Integrate LUT
		integrate_shader_->Bind();
		renderer_->ChangeRenderTarget(integrate_rt_, nullptr);
		renderer_->ClearColorBuffer();
		quad_->Render();
		renderer_->ChangeRenderTarget(nullptr, nullptr); // back to main framebuffer
		integrate_shader_->Unbind();

		renderer_->EnableDepthTest();
	}
	void RenderEnvironment(sht::graphics::Texture * env_texture)
	{
		renderer_->DisableDepthTest();

		renderer_->ChangeTexture(env_texture);
		env_shader_->Bind();
		env_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
		env_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
		quad_->Render();
		env_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		renderer_->EnableDepthTest();
	}
	void RenderQuad(sht::graphics::Texture * texture)
	{
		renderer_->DisableDepthTest();

		renderer_->ChangeTexture(texture);
		quad_shader_->Bind();
		quad_->Render();
		quad_shader_->Unbind();
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
			RenderEnvironment(env_texture_);
			break;
		case 1:
			RenderEnvironment(irradiance_rt_);
			break;
		case 2:
			RenderEnvironment(prefilter_rt_);
			break;
		case 3:
			RenderQuad(integrate_rt_);
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
	sht::graphics::Shader * gui_shader_;
	sht::graphics::Shader * env_shader_;
	sht::graphics::Shader * quad_shader_;
	sht::graphics::Shader * irradiance_shader_;
	sht::graphics::Shader * prefilter_shader_;
	sht::graphics::Shader * integrate_shader_;

	sht::graphics::Texture * env_texture_;
	sht::graphics::Texture * irradiance_rt_;
	sht::graphics::Texture * prefilter_rt_;
	sht::graphics::Texture * integrate_rt_;

	sht::graphics::Font * font_;
	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	
	sht::math::Matrix4 projection_view_matrix_;
	
	int render_type_;
	bool need_update_projection_matrix_;
};

DECLARE_MAIN(IBLBakerApp);