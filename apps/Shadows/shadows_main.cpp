#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/screen_quad_model.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/model/cube_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"

#include <cmath>

/*
The main concept of creating this application is testing cubemap edge artefacts during accessing mipmaps.
*/

#define APP_NAME ShadowsApp

class APP_NAME : public sht::OpenGlApplication
{
public:
	APP_NAME()
	: quad_(nullptr)
	, sphere_(nullptr)
	, cube_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, camera_manager_(nullptr)
	, angle_(0.0f)
	, light_distance_(10.0f)
	, need_update_projection_matrix_(true)
	, show_shadow_texture_(false)
	{
	}
	const char* GetTitle() final
	{
		return "Shadows test";
	}
	const bool IsMultisample() final
	{
		return true;
	}
	void BindShaderConstants()
	{
		const sht::math::Vector3 kLightColor(1.0f);

		object_shader_->Bind();
		object_shader_->Uniform3fv("u_light.color", kLightColor);
		object_shader_->Uniform1i("u_shadow_sampler", 0);
		object_shader_->Unbind();
	}
	void BindShaderVariables()
	{
		object_shader_->Bind();
		object_shader_->Uniform3fv("u_light.position", light_position_);
		object_shader_->Unbind();
	}
	bool Load() final
	{
		// Quad model
		quad_ = new sht::graphics::ScreenQuadModel(renderer_);
		quad_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		quad_->Create();
		if (!quad_->MakeRenderable())
			return false;

		// Sphere model
		sphere_ = new sht::graphics::SphereModel(renderer_, 128, 64);
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		sphere_->Create();
		if (!sphere_->MakeRenderable())
			return false;

		// Cube model
		cube_ = new sht::graphics::CubeModel(renderer_);
		cube_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		cube_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		cube_->Create();
		if (!cube_->MakeRenderable())
			return false;
		
		// Load shaders
		if (!renderer_->AddShader(text_shader_, "data/shaders/text")) return false;
		if (!renderer_->AddShader(quad_shader_, "data/shaders/apps/IBLBaker/quad")) return false;
		if (!renderer_->AddShader(object_shader_, "data/shaders/apps/Shadows/object")) return false;
		if (!renderer_->AddShader(object_shadow_shader_, "data/shaders/apps/Shadows/object_shadow")) return false;

		// Render targets
		const int kFramebufferSize = 1024;
		renderer_->CreateTextureDepth(shadow_depth_rt_, kFramebufferSize, kFramebufferSize, 32);

		// Fonts
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
		if (cube_)
			delete cube_;
	}
	void Update() final
	{
		const float kFrameTime = GetFrameTime();

		camera_manager_->Update(kFrameTime);

		angle_ += 0.1f * kFrameTime;
		light_position_.Set(light_distance_ * cosf(angle_), 1.0f, light_distance_ * sinf(angle_));
		//light_position_.Set(1.0f, light_distance_, 1.0f);

		// Update matrices
		renderer_->SetViewMatrix(camera_manager_->view_matrix());
		UpdateProjectionMatrix();
		projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();

		BindShaderVariables();
	}
	void RenderObjects(sht::graphics::Shader * shader)
	{
		// Render cube
		renderer_->PushMatrix();
		shader->UniformMatrix4fv("u_model", renderer_->model_matrix());
		cube_->Render();
		renderer_->PopMatrix();

		renderer_->PushMatrix();
		renderer_->Translate(0.0f, -6.0f, 0.0f);
		renderer_->Scale(5.0f);
		shader->UniformMatrix4fv("u_model", renderer_->model_matrix());
		cube_->Render();
		renderer_->PopMatrix();

		// Render spheres
		renderer_->PushMatrix();
		renderer_->Translate(2.0f, 0.0f, 0.0f);
		shader->UniformMatrix4fv("u_model", renderer_->model_matrix());
		sphere_->Render();
		renderer_->PopMatrix();

		renderer_->PushMatrix();
		renderer_->Translate(-2.0f, 0.0f, 0.0f);
		shader->UniformMatrix4fv("u_model", renderer_->model_matrix());
		sphere_->Render();
		renderer_->PopMatrix();

		renderer_->PushMatrix();
		renderer_->Translate(0.0f, 0.0f, 2.0f);
		shader->UniformMatrix4fv("u_model", renderer_->model_matrix());
		sphere_->Render();
		renderer_->PopMatrix();

		renderer_->PushMatrix();
		renderer_->Translate(0.0f, 0.0f, -2.0f);
		shader->UniformMatrix4fv("u_model", renderer_->model_matrix());
		sphere_->Render();
		renderer_->PopMatrix();
	}
	void ShadowPass()
	{
		// Generate matrix for shadows
		// Ortho matrix is used for directional light sources and perspective for spot ones.
		float znear = 1.0f;
		float zfar = light_distance_ + 2.0f;
		sht::math::Matrix4 depth_projection = sht::math::PerspectiveMatrix(45.0f, 1, 1, znear, zfar);//sht::math::OrthoMatrix()
		sht::math::Matrix4 depth_view = sht::math::LookAt(light_position_, vec3(0.0f));
		sht::math::Matrix4 depth_projection_view = depth_projection * depth_view;
		sht::math::Matrix4 bias_matrix(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);
		depth_bias_projection_view_matrix_ = bias_matrix * depth_projection_view;

		renderer_->ChangeRenderTarget(nullptr, shadow_depth_rt_);
		renderer_->ClearColorAndDepthBuffers();

		object_shadow_shader_->Bind();
		object_shadow_shader_->UniformMatrix4fv("u_projection_view", depth_projection_view);

		RenderObjects(object_shadow_shader_);

		object_shadow_shader_->Unbind();

		renderer_->ChangeRenderTarget(nullptr, nullptr);
	}
	void RenderScene()
	{
		// First render shadows
		renderer_->CullFace(sht::graphics::CullFaceType::kFront);
		ShadowPass();
		renderer_->CullFace(sht::graphics::CullFaceType::kBack);

		if (show_shadow_texture_)
		{
			quad_shader_->Bind();
			quad_shader_->Uniform1i("u_texture", 0);

			renderer_->ChangeTexture(shadow_depth_rt_, 0);
			quad_->Render();
			renderer_->ChangeTexture(nullptr, 0);

			quad_shader_->Unbind();
		}
		else
		{
			// Render objects
			object_shader_->Bind();
			object_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
			object_shader_->UniformMatrix4fv("u_depth_bias_projection_view", depth_bias_projection_view_matrix_);

			renderer_->ChangeTexture(shadow_depth_rt_, 0);

			RenderObjects(object_shader_);

			renderer_->ChangeTexture(nullptr, 0);

			object_shader_->Unbind();
		}
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
		text_shader_->Unbind();

		renderer_->ChangeTexture(nullptr);
		
		renderer_->EnableDepthTest();
	}
	void Render() final
	{
		renderer_->SetViewport(width_, height_);

		renderer_->ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		renderer_->ClearColorAndDepthBuffers();

		RenderScene();
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
			show_shadow_texture_ = !show_shadow_texture_;
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
	sht::graphics::Model * sphere_;
	sht::graphics::Model * cube_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * quad_shader_;
	sht::graphics::Shader * object_shader_;
	sht::graphics::Shader * object_shadow_shader_; //!< simplified version for shadows generation

	sht::graphics::Texture * shadow_depth_rt_;

	sht::graphics::Font * font_;
	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	
	sht::math::Matrix4 projection_view_matrix_;
	sht::math::Matrix4 depth_bias_projection_view_matrix_;
	sht::math::Vector3 light_position_;

	float angle_;
	const float light_distance_;

	bool need_update_projection_matrix_;
	bool show_shadow_texture_;
};

DECLARE_MAIN(APP_NAME);