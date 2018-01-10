/*
** Simple maze game powered by OpenGL graphics and Bullet physics
*/
#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/model/screen_quad_model.h"
#include "../../sht/graphics/include/model/complex_mesh.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"
#include "../../sht/physics/include/physics_engine.h"
#include "../../sht/physics/include/physics_object.h"
#include "../../sht/utility/include/ui/slider.h"

#include <cmath>

#define APP_NAME MazeApp

namespace {
	const float kMaxAngle = 0.5f;
}

class APP_NAME : public sht::OpenGlApplication
{
public:
	APP_NAME()
	: sphere_(nullptr)
	, quad_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, camera_manager_(nullptr)
	, physics_(nullptr)
	, ball_(nullptr)
	, maze_(nullptr)
	, maze_angle_x_(0.0f)
	, maze_angle_y_(0.0f)
	, need_update_projection_matrix_(true)
	{
	}
	const char* GetTitle() final
	{
		return "Maze";
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
		object_shader_->Uniform3f("u_light.color", 1.0f, 1.0f, 1.0f);
		object_shader_->Uniform3f("u_light.direction", 1.0f, 1.0f, -1.0f);

		object_shader_->Uniform1i("u_diffuse_env_sampler", 0);
		object_shader_->Uniform1i("u_specular_env_sampler", 1);
		object_shader_->Uniform1i("u_preintegrated_fg_sampler", 2);
		object_shader_->Uniform1i("u_albedo_sampler", 3);
		object_shader_->Uniform1i("u_normal_sampler", 4);
		object_shader_->Uniform1i("u_roughness_sampler", 5);
		object_shader_->Uniform1i("u_metal_sampler", 6);
		// Iron
		// object_shader_->Uniform4f("u_albedo_color", 0.776f, 0.776f, 0.784f, 1.0f);
		// object_shader_->Uniform1f("u_roughness", 0.112f);
		// object_shader_->Uniform1f("u_metalness", 1.0f);
		// Plastic
		//object_shader_->Uniform4f("u_albedo_color", 0.219f, 0.219f, 0.219f, 0.7f);
		//object_shader_->Uniform1f("u_roughness", 0.298f);
		//object_shader_->Uniform1f("u_metalness", 0.0f);

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
		//sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTangent, 3));
		//sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kBinormal, 3));
		sphere_->Create();
		if (!sphere_->MakeRenderable())
			return false;

		// Screen quad model
		quad_ = new sht::graphics::ScreenQuadModel(renderer_);
		quad_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		quad_->Create();
		if (!quad_->MakeRenderable())
			return false;

		// Maze mesh
		maze_mesh_ = new sht::graphics::ComplexMesh(renderer_, nullptr);
		if (!maze_mesh_->LoadFromFile("data/meshes/maze/maze.obj")) return false;
		
		// Load shaders
		if (!renderer_->AddShader(text_shader_, "data/shaders/text")) return false;
		if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored")) return false;
		if (!renderer_->AddShader(env_shader_, "data/shaders/skybox")) return false;
		if (!renderer_->AddShader(irradiance_shader_, "data/shaders/pbr/irradiance")) return false;
		if (!renderer_->AddShader(prefilter_shader_, "data/shaders/pbr/prefilter")) return false;
		if (!renderer_->AddShader(object_shader_, "data/shaders/apps/Maze/pbr_no_tb")) return false;
		
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
		if (!renderer_->AddTexture(fg_texture_, "data/textures/pbr/brdfLUT.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;

		if (!renderer_->AddTexture(ball_albedo_texture_, "data/textures/pbr/metal/rusted_iron/albedo.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(ball_normal_texture_, "data/textures/pbr/metal/rusted_iron/normal.png",
		 						   sht::graphics::Texture::Wrap::kClampToEdge,
		 						   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(ball_roughness_texture_, "data/textures/pbr/metal/rusted_iron/roughness.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(ball_metal_texture_, "data/textures/pbr/metal/rusted_iron/metallic.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;

		if (!renderer_->AddTexture(maze_albedo_texture_, "data/textures/pbr/concrete/painted_cement/albedo.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(maze_normal_texture_, "data/textures/pbr/concrete/painted_cement/normal.png",
		 						   sht::graphics::Texture::Wrap::kClampToEdge,
		 						   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(maze_roughness_texture_, "data/textures/pbr/concrete/painted_cement/roughness.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(maze_metal_texture_, "data/textures/pbr/concrete/painted_cement/metallic.png",
								   sht::graphics::Texture::Wrap::kClampToEdge,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;

		// Render targets
		renderer_->CreateTextureCubemap(irradiance_rt_, 32, 32, sht::graphics::Image::Format::kRGB8, sht::graphics::Texture::Filter::kLinear);
		renderer_->CreateTextureCubemap(prefilter_rt_, 512, 512, sht::graphics::Image::Format::kRGB8, sht::graphics::Texture::Filter::kTrilinear);
		renderer_->GenerateMipmap(prefilter_rt_);

		renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
		if (font_ == nullptr)
			return false;

		fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
		if (!fps_text_)
			return false;

		camera_manager_ = new sht::utility::CameraManager();
		camera_manager_->MakeFree(vec3(10.0f), vec3(0.0f));

		// Create physics
		physics_ = new sht::physics::Engine(20 /* max sub steps */, 0.002f /* fixed time step */);
		//physics_->SetGravity(vec3(0.0f, -9800.0f, 0.0f));

		// Setup physics objects
		{
			ball_ = physics_->AddSphere(vec3(0.0f, 5.0f, 0.0f), 0.150f, 1.0f);
			ball_->SetFriction(0.28f);
			ball_->SetRollingFriction(0.2f);
			ball_->SetSpinningFriction(0.5f);
			ball_->SetRestitution(0.3f);
		}
		{
			sht::graphics::MeshVerticesEnumerator enumerator(maze_mesh_);
			maze_ = physics_->AddMesh(vec3(0.0f, -6.0f, 0.0f), 0.0f, &enumerator);
			maze_->SetFriction(0.71f);
			maze_->SetRestitution(0.0f);
			maze_->MakeKinematic();
		}

		maze_matrix_ = maze_->matrix();

		// Mesh uploading to GPU should be done after physics initialization since it uses vertices data.
		maze_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		maze_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		maze_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
		//maze_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTangent, 3));
		//maze_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kBinormal, 3));
		if (!maze_mesh_->MakeRenderable())
			return false;

		CreateUI();

		// Finally bind constants
		BindShaderConstants();

		BakeCubemaps();
		
		return true;
	}
	void Unload() final
	{
		if (ui_root_)
			delete ui_root_;
		if (physics_)
			delete physics_;
		if (camera_manager_)
			delete camera_manager_;
		if (fps_text_)
			delete fps_text_;
		if (maze_mesh_)
			delete maze_mesh_;
		if (quad_)
			delete quad_;
		if (sphere_)
			delete sphere_;
	}
	void UpdatePhysics(float sec) final
	{
		// Update physics engine
		physics_->Update(sec);
	}
	void Update() final
	{
		const float kFrameTime = GetFrameTime();

		camera_manager_->Update(kFrameTime);

		// Update UI
		ui_root_->UpdateAll(kFrameTime);

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
	void RenderMaze()
	{
		renderer_->PushMatrix();
		renderer_->MultMatrix(maze_->matrix());

		renderer_->ChangeTexture(irradiance_rt_, 0);
		renderer_->ChangeTexture(prefilter_rt_, 1);
		renderer_->ChangeTexture(fg_texture_, 2);
		renderer_->ChangeTexture(maze_albedo_texture_, 3);
		renderer_->ChangeTexture(maze_normal_texture_, 4);
		renderer_->ChangeTexture(maze_roughness_texture_, 5);
		renderer_->ChangeTexture(maze_metal_texture_, 6);
		
		object_shader_->Bind();
		object_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
		object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
		object_shader_->Uniform3fv("u_camera.position", *camera_manager_->position());
		
		maze_mesh_->Render();
		
		object_shader_->Unbind();

		renderer_->ChangeTexture(nullptr, 6);
		renderer_->ChangeTexture(nullptr, 5);
		renderer_->ChangeTexture(nullptr, 4);
		renderer_->ChangeTexture(nullptr, 3);
		renderer_->ChangeTexture(nullptr, 2);
		renderer_->ChangeTexture(nullptr, 1);
		renderer_->ChangeTexture(nullptr, 0);
		
		renderer_->PopMatrix();
	}
	void RenderBall()
	{
		renderer_->PushMatrix();
		renderer_->MultMatrix(ball_->matrix());

		renderer_->ChangeTexture(irradiance_rt_, 0);
		renderer_->ChangeTexture(prefilter_rt_, 1);
		renderer_->ChangeTexture(fg_texture_, 2);
		renderer_->ChangeTexture(ball_albedo_texture_, 3);
		renderer_->ChangeTexture(ball_normal_texture_, 4);
		renderer_->ChangeTexture(ball_roughness_texture_, 5);
		renderer_->ChangeTexture(ball_metal_texture_, 6);
		
		object_shader_->Bind();
		object_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
		object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
		object_shader_->Uniform3fv("u_camera.position", *camera_manager_->position());
		
		sphere_->Render();
		
		object_shader_->Unbind();

		renderer_->ChangeTexture(nullptr, 6);
		renderer_->ChangeTexture(nullptr, 5);
		renderer_->ChangeTexture(nullptr, 4);
		renderer_->ChangeTexture(nullptr, 3);
		renderer_->ChangeTexture(nullptr, 2);
		renderer_->ChangeTexture(nullptr, 1);
		renderer_->ChangeTexture(nullptr, 0);
		
		renderer_->PopMatrix();
	}
	void RenderObjects()
	{
		RenderMaze();
		RenderBall();
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

		// Render UI
		gui_shader_->Bind();
		ui_root_->RenderAll();
		
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
	void OnHorizontalSliderMoved()
	{
		maze_angle_x_ = (horizontal_slider_->pin_position() * 2.0f - 1.0f) * kMaxAngle;
		UpdateMazeMatrix();
	}
	void OnVerticalSliderMoved()
	{
		maze_angle_y_ = (vertical_slider_->pin_position() * 2.0f - 1.0f) * kMaxAngle;
		UpdateMazeMatrix();
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
		vec2 position(mouse_.x() / height_, mouse_.y() / height_);
		horizontal_slider_->OnTouchDown(position);
		vertical_slider_->OnTouchDown(position);
	}
	void OnMouseUp(sht::MouseButton button, int modifiers) final
	{
		vec2 position(mouse_.x() / height_, mouse_.y() / height_);
		horizontal_slider_->OnTouchUp(position);
		vertical_slider_->OnTouchUp(position);
	}
	void OnMouseMove() final
	{
		vec2 position(mouse_.x() / height_, mouse_.y() / height_);
		horizontal_slider_->OnTouchMove(position);
		if (horizontal_slider_->is_touched())
			OnHorizontalSliderMoved();
		vertical_slider_->OnTouchMove(position);
		if (vertical_slider_->is_touched())
			OnVerticalSliderMoved();
	}
	void OnSize(int w, int h) final
	{
		Application::OnSize(w, h);
		// To have correct perspective when resizing
		need_update_projection_matrix_ = true;
	}
	void UpdateMazeMatrix()
	{
		float cos_x = cosf(maze_angle_x_);
		float sin_x = sinf(maze_angle_x_);
		sht::math::Matrix4 rotation_x = sht::math::Rotate4(cos_x, sin_x, 1.0f, 0.0f, 0.0f);
		float cos_y = cosf(maze_angle_y_);
		float sin_y = sinf(maze_angle_y_);
		sht::math::Matrix4 rotation_y = sht::math::Rotate4(cos_y, sin_y, 1.0f, 0.0f, 0.0f);
		maze_->SetTransform(maze_matrix_ * rotation_x * rotation_y);
	}
	void CreateUI()
	{
		ui_root_ = new sht::utility::ui::Widget();

		// ----- User interaction UI -----
		const vec4 kBarColor(0.5f, 0.5f, 0.5f, 0.5f);
		const vec4 kPinColorNormal(1.0f, 1.0f, 1.0f, 1.0f);
		const vec4 kPinColorTouch(1.0f, 1.0f, 0.0f, 1.0f);

		horizontal_slider_ = new sht::utility::ui::SliderColored(renderer_, gui_shader_,
			kBarColor, // vec4 bar_color
			kPinColorNormal, // vec4 pin_color_normal
			kPinColorTouch, // vec4 pin_color_touch
			0.0f, // f32 x
			0.0f, // f32 y
			renderer_->aspect_ratio(), // f32 width
			0.2f, // f32 height
			(u32)sht::utility::ui::Flags::kRenderAlways// u32 flags
			);
		ui_root_->AttachWidget(horizontal_slider_);
		vertical_slider_ = new sht::utility::ui::SliderColored(renderer_, gui_shader_,
			kBarColor, // vec4 bar_color
			kPinColorNormal, // vec4 pin_color_normal
			kPinColorTouch, // vec4 pin_color_touch
			0.0f, // f32 x
			0.2f, // f32 y
			0.2f, // f32 width
			0.8f, // f32 height
			(u32)sht::utility::ui::Flags::kRenderAlways// u32 flags
			);
		ui_root_->AttachWidget(vertical_slider_);
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
	sht::graphics::ComplexMesh * maze_mesh_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * gui_shader_;
	sht::graphics::Shader * env_shader_;
	sht::graphics::Shader * object_shader_;
	sht::graphics::Shader * irradiance_shader_;
	sht::graphics::Shader * prefilter_shader_;

	sht::graphics::Texture * env_texture_;
	sht::graphics::Texture * fg_texture_;
	sht::graphics::Texture * ball_albedo_texture_;
	sht::graphics::Texture * ball_normal_texture_;
	sht::graphics::Texture * ball_roughness_texture_;
	sht::graphics::Texture * ball_metal_texture_;
	sht::graphics::Texture * maze_albedo_texture_;
	sht::graphics::Texture * maze_normal_texture_;
	sht::graphics::Texture * maze_roughness_texture_;
	sht::graphics::Texture * maze_metal_texture_;
	sht::graphics::Texture * irradiance_rt_;
	sht::graphics::Texture * prefilter_rt_;

	sht::graphics::Font * font_;
	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;

	sht::physics::Engine * physics_;
	sht::physics::Object * ball_;
	sht::physics::Object * maze_;

	sht::utility::ui::Widget * ui_root_;
	sht::utility::ui::SliderColored * horizontal_slider_;
	sht::utility::ui::SliderColored * vertical_slider_;
	
	sht::math::Matrix4 projection_view_matrix_;
	sht::math::Matrix4 maze_matrix_;

	float maze_angle_x_;
	float maze_angle_y_;

	bool need_update_projection_matrix_;
};

DECLARE_MAIN(APP_NAME);