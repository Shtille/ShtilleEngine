#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/model/skybox_quad_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"

#include <cmath>
#include <cstdio>

class IBLBakerApp : public sht::OpenGlApplication
{
public:
	IBLBakerApp()
	: sphere_(nullptr)
	, quad_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, camera_manager_(nullptr)
	, need_update_projection_matrix_(true)
	, render_original_(true)
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

		env_convert_shader_->Bind();
		env_convert_shader_->Uniform1i("u_texture", 0);
		env_convert_shader_->Unbind();
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

		// Cube model
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
		if (!renderer_->AddShader(env_convert_shader_, "data/shaders/apps/IBLBaker/env_convert")) return false;
		if (!renderer_->AddShader(irradiance_shader_, "data/shaders/apps/IBLBaker/irradiance")) return false;
		// if (!renderer_->AddShader(prefilter_shader_, "data/shaders/apps/IBLBaker/prefilter")) return false;
		// if (!renderer_->AddShader(integrate_shader_, "data/shaders/apps/IBLBaker/integrate")) return false;
		
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
		const int kFramebufferSize = 1024;
		renderer_->AddRenderTarget(test_texture_, kFramebufferSize, kFramebufferSize, sht::graphics::Image::Format::kRGB8);
		renderer_->CreateTextureCubemap(cubemap_rt_, 512, 512, sht::graphics::Image::Format::kRGB8);
		renderer_->CreateTextureCubemap(irradiance_rt_, 32, 32, sht::graphics::Image::Format::kRGB8); // TODO: LINEAR
		renderer_->CreateTextureCubemap(prefilter_rt_, 128, 128, sht::graphics::Image::Format::kRGB8); // TODO: TRILINEAR
		renderer_->CreateTextureCubemap(integrate_rt_, 512, 512, sht::graphics::Image::Format::kRGB8); // TODO: LINEAR

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
		BakeCubemap(irradiance_rt_, irradiance_shader_);
		
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
	void BakeCubemap(sht::graphics::Texture * render_target, sht::graphics::Shader * shader)
	{
		renderer_->DisableDepthTest();

		// Render to cubemap
		renderer_->ChangeTexture(env_texture_);
		shader->Bind();
		sht::math::Matrix4 projection_matrix = sht::math::PerspectiveMatrix(90.0f, 512, 512, 0.1f, 100.0f);
		shader->Uniform1i("u_texture", 0);
		shader->UniformMatrix4fv("u_projection", projection_matrix);
		for (int face = 0; face < 6; ++face)
		{
			sht::math::Matrix4 view_matrix = sht::math::LookAtCube(vec3(0.0f), face);
			shader->UniformMatrix4fv("u_view", view_matrix);
			renderer_->ChangeRenderTargetsToCube(1, &render_target, nullptr, face, 0);
			quad_->Render();
		}
		renderer_->ChangeRenderTarget(nullptr, nullptr); // back to main framebuffer
		shader->Unbind();
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
		quad_->Render();
		env_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		renderer_->EnableDepthTest();
	}
	void RenderToTextureTest()
	{
		renderer_->DisableDepthTest();

		// Test single render target
		renderer_->ChangeRenderTarget(test_texture_, nullptr);

		renderer_->ChangeTexture(env_texture_);
		env_shader_->Bind();
		env_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
		env_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
		quad_->Render();
		env_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		renderer_->ChangeRenderTarget(nullptr, nullptr); // back to main framebuffer

		// Render quad with texture
		renderer_->ChangeTexture(test_texture_);
		quad_shader_->Bind();
		quad_->Render();
		quad_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		renderer_->EnableDepthTest();
	}
	void BuildCubemap(sht::graphics::Texture * render_target)
	{
		renderer_->DisableDepthTest();

		// Render to cubemap
		renderer_->ChangeTexture(env_texture_);
		env_convert_shader_->Bind();
		sht::math::Matrix4 projection_matrix = sht::math::PerspectiveMatrix(90.0f, 512, 512, 0.1f, 100.0f);
		for (int face = 0; face < 6; ++face)
		{
			sht::math::Matrix4 view_matrix = sht::math::LookAtCube(vec3(0.0f), face);
			renderer_->ChangeRenderTargetsToCube(1, &render_target, nullptr, face, 0);
			env_convert_shader_->UniformMatrix4fv("u_projection", projection_matrix);
			env_convert_shader_->UniformMatrix4fv("u_view", view_matrix);
			quad_->Render();
		}
		renderer_->ChangeRenderTarget(nullptr, nullptr); // back to main framebuffer
		env_convert_shader_->Unbind();
		renderer_->ChangeTexture(nullptr);

		// Render quad with texture
		renderer_->ChangeTexture(render_target);
		env_shader_->Bind();
		env_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
		env_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
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
		fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", GetFrameRate());
		fps_text_->Render();

		renderer_->ChangeTexture(nullptr);
		
		renderer_->EnableDepthTest();
	}
	void Render() final
	{
		renderer_->SetViewport(width_, height_);
		
		renderer_->ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		renderer_->ClearColorAndDepthBuffers();
		
		if (render_original_)
			RenderEnvironment(env_texture_);
		else
			RenderEnvironment(irradiance_rt_);
			//RenderToTextureTest();
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
			render_original_ = !render_original_;
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
	sht::graphics::Shader * quad_shader_;
	sht::graphics::Shader * env_convert_shader_;
	sht::graphics::Shader * irradiance_shader_;
	sht::graphics::Shader * prefilter_shader_;
	sht::graphics::Shader * integrate_shader_;

	sht::graphics::Texture * env_texture_;
	sht::graphics::Texture * test_texture_;
	sht::graphics::Texture * cubemap_rt_;
	sht::graphics::Texture * irradiance_rt_;
	sht::graphics::Texture * prefilter_rt_;
	sht::graphics::Texture * integrate_rt_;

	sht::graphics::Font * font_;
	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	
	sht::math::Matrix4 projection_view_matrix_;
	
	bool need_update_projection_matrix_;
	bool render_original_;
};

DECLARE_MAIN(IBLBakerApp);