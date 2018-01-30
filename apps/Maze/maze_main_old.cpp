/*
** Simple maze game powered by OpenGL graphics and Bullet physics
*/
#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/model/screen_quad_model.h"
#include "../../sht/graphics/include/model/physical_box_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"
#include "../../sht/physics/include/physics_engine.h"
#include "../../sht/physics/include/physics_object.h"
#include "../../sht/utility/include/ui/slider.h"
#include "../../sht/utility/include/ui/board.h"
#include "../../sht/utility/include/ui/label.h"
#include "../../sht/utility/include/ui/rect.h"

#include <cmath>
#include <vector>

#define ROTATING_PLATFORM

#define APP_NAME MazeApp

namespace {
#ifdef ROTATING_PLATFORM
	const float kMaxAngle = 0.5f;
#endif
	const float kBallRadius = 1.0f;
}

struct WallBaseData {
	float start_x;
	float start_y;
	float end_x;
	float end_y;
};
struct WallData {
	vec3 center;
	vec3 sizes;
};

class APP_NAME : public sht::OpenGlApplication
{
public:
	APP_NAME()
	: sphere_(nullptr)
	, quad_(nullptr)
	, floor_model_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, camera_manager_(nullptr)
	, physics_(nullptr)
	, ball_(nullptr)
#ifdef ROTATING_PLATFORM
	, target_angle_x_(0.0f)
	, target_angle_y_(0.0f)
	, velocity_x_(0.0f)
	, velocity_y_(0.0f)
	, acceleration_x_(0.0f)
	, acceleration_y_(0.0f)
	, maze_angle_x_(0.0f)
	, maze_angle_y_(0.0f)
#endif
	, camera_distance_(10.0f)
	, camera_alpha_(0.0f)
	, camera_theta_(0.5f)
#ifdef ROTATING_PLATFORM
	, update_rotation_x_(false)
	, update_rotation_y_(false)
#endif
	, need_update_projection_matrix_(true)
	{
	}
	const char* GetTitle() final
	{
		return "Marble maze";
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
		object_shader_->Uniform3f("u_light.direction", 0.825f, 0.564f, 0.0f);

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

		// Floor model
		const float kCS = 10.0f; // cell size
		const float kMaterialSize = 3.0f;
		{
			floor_model_ = new sht::graphics::PhysicalBoxModel(renderer_, 12.0f * kCS, 2.0f, 12.0f * kCS, kMaterialSize, kMaterialSize);
			floor_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
			floor_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
			floor_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
			//floor_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTangent, 3));
			//floor_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kBinormal, 3));
			floor_model_->Create();
			if (!floor_model_->MakeRenderable())
				return false;
		}
		// Base wall data should be filled manually
		const WallBaseData walls_base_data[] = {
			{ -10.f * kCS,   0.f * kCS, -10.f * kCS, -10.f * kCS },
			{ -10.f * kCS, -10.f * kCS,  10.f * kCS, -10.f * kCS },
			{  10.f * kCS, -10.f * kCS,  10.f * kCS,  10.f * kCS },
			{  10.f * kCS,  10.f * kCS, -10.f * kCS,  10.f * kCS },
			{ -10.f * kCS,  10.f * kCS, -10.f * kCS,   1.f * kCS },
			// Vertical lines
			{  -9.f * kCS, -10.f * kCS,  -9.f * kCS,  -8.f * kCS },
			{  -9.f * kCS,  -6.f * kCS,  -9.f * kCS,  -4.f * kCS },
			{  -9.f * kCS,  -1.f * kCS,  -9.f * kCS,   5.f * kCS },
			{  -9.f * kCS,   6.f * kCS,  -9.f * kCS,   7.f * kCS },
			{  -9.f * kCS,   8.f * kCS,  -9.f * kCS,   9.f * kCS },
			{  -8.f * kCS,  -8.f * kCS,  -8.f * kCS,  -5.f * kCS },
			{  -8.f * kCS,  -4.f * kCS,  -8.f * kCS,   0.f * kCS },
			{  -8.f * kCS,   5.f * kCS,  -8.f * kCS,   6.f * kCS },
			{  -8.f * kCS,   7.f * kCS,  -8.f * kCS,   8.f * kCS },
			{  -8.f * kCS,   9.f * kCS,  -8.f * kCS,  10.f * kCS },
			{  -7.f * kCS,  -9.f * kCS,  -7.f * kCS,  -6.f * kCS },
			{  -7.f * kCS,  -5.f * kCS,  -7.f * kCS,  -4.f * kCS },
			{  -7.f * kCS,  -2.f * kCS,  -7.f * kCS,  -1.f * kCS },
			{  -7.f * kCS,   1.f * kCS,  -7.f * kCS,   4.f * kCS },
			{  -7.f * kCS,   8.f * kCS,  -7.f * kCS,   9.f * kCS },
			{  -6.f * kCS,  -6.f * kCS,  -6.f * kCS,  -5.f * kCS },
			{  -6.f * kCS,  -1.f * kCS,  -6.f * kCS,   1.f * kCS },
			{  -6.f * kCS,   2.f * kCS,  -6.f * kCS,   3.f * kCS },
			{  -6.f * kCS,   4.f * kCS,  -6.f * kCS,   6.f * kCS },
			{  -6.f * kCS,   7.f * kCS,  -6.f * kCS,  10.f * kCS },
			{  -5.f * kCS,  -9.f * kCS,  -5.f * kCS,  -7.f * kCS },
			{  -5.f * kCS,  -5.f * kCS,  -5.f * kCS,  -3.f * kCS },
			{  -5.f * kCS,   1.f * kCS,  -5.f * kCS,   4.f * kCS },
			{  -5.f * kCS,   6.f * kCS,  -5.f * kCS,   9.f * kCS },
			{  -4.f * kCS, -10.f * kCS,  -4.f * kCS,  -8.f * kCS },
			{  -4.f * kCS,  -6.f * kCS,  -4.f * kCS,  -5.f * kCS },
			{  -4.f * kCS,  -4.f * kCS,  -4.f * kCS,  -1.f * kCS },
			{  -4.f * kCS,   8.f * kCS,  -4.f * kCS,  10.f * kCS },
			{  -3.f * kCS,  -7.f * kCS,  -3.f * kCS,  -6.f * kCS },
			{  -3.f * kCS,  -3.f * kCS,  -3.f * kCS,   5.f * kCS },
			{  -3.f * kCS,   6.f * kCS,  -3.f * kCS,   8.f * kCS },
			{  -2.f * kCS,  -8.f * kCS,  -2.f * kCS,  -5.f * kCS },
			{  -2.f * kCS,  -4.f * kCS,  -2.f * kCS,  -3.f * kCS },
			{  -2.f * kCS,   4.f * kCS,  -2.f * kCS,   6.f * kCS },
			{  -1.f * kCS,  -9.f * kCS,  -1.f * kCS,  -4.f * kCS },
			{  -1.f * kCS,   4.f * kCS,  -1.f * kCS,   5.f * kCS },
			{   0.f * kCS, -10.f * kCS,   0.f * kCS,  -9.f * kCS },
			{   0.f * kCS,  -7.f * kCS,   0.f * kCS,  -5.f * kCS },
			{   0.f * kCS,   3.f * kCS,   0.f * kCS,   4.f * kCS },
			{   0.f * kCS,   6.f * kCS,   0.f * kCS,   8.f * kCS },
			{   1.f * kCS,  -9.f * kCS,   1.f * kCS,  -6.f * kCS },
			{   1.f * kCS,  -5.f * kCS,   1.f * kCS,  -3.f * kCS },
			{   1.f * kCS,   5.f * kCS,   1.f * kCS,   7.f * kCS },
			{   2.f * kCS,  -7.f * kCS,   2.f * kCS,  -5.f * kCS },
			{   2.f * kCS,  -4.f * kCS,   2.f * kCS,   0.f * kCS },
			{   2.f * kCS,   1.f * kCS,   2.f * kCS,   3.f * kCS },
			{   2.f * kCS,   5.f * kCS,   2.f * kCS,   6.f * kCS },
			{   2.f * kCS,   7.f * kCS,   2.f * kCS,   9.f * kCS },
			{   3.f * kCS,  -9.f * kCS,   3.f * kCS,  -6.f * kCS },
			{   3.f * kCS,  -5.f * kCS,   3.f * kCS,   2.f * kCS },
			{   3.f * kCS,   3.f * kCS,   3.f * kCS,   4.f * kCS },
			{   3.f * kCS,   5.f * kCS,   3.f * kCS,   7.f * kCS },
			{   3.f * kCS,   8.f * kCS,   3.f * kCS,   9.f * kCS },
			{   4.f * kCS,  -8.f * kCS,   4.f * kCS,  -5.f * kCS },
			{   4.f * kCS,  -4.f * kCS,   4.f * kCS,   1.f * kCS },
			{   4.f * kCS,   4.f * kCS,   4.f * kCS,   5.f * kCS },
			{   4.f * kCS,   6.f * kCS,   4.f * kCS,   9.f * kCS },
			{   5.f * kCS,  -9.f * kCS,   5.f * kCS,  -8.f * kCS },
			{   5.f * kCS,  -6.f * kCS,   5.f * kCS,   0.f * kCS },
			{   5.f * kCS,   6.f * kCS,   5.f * kCS,   8.f * kCS },
			{   6.f * kCS, -10.f * kCS,   6.f * kCS,  -9.f * kCS },
			{   6.f * kCS,  -6.f * kCS,   6.f * kCS,  -2.f * kCS },
			{   6.f * kCS,   1.f * kCS,   6.f * kCS,   2.f * kCS },
			{   6.f * kCS,   5.f * kCS,   6.f * kCS,   9.f * kCS },
			{   7.f * kCS,  -9.f * kCS,   7.f * kCS,  -6.f * kCS },
			{   7.f * kCS,  -5.f * kCS,   7.f * kCS,  -4.f * kCS },
			{   7.f * kCS,  -2.f * kCS,   7.f * kCS,   1.f * kCS },
			{   7.f * kCS,   2.f * kCS,   7.f * kCS,   6.f * kCS },
			{   8.f * kCS,  -8.f * kCS,   8.f * kCS,  -7.f * kCS },
			{   8.f * kCS,  -3.f * kCS,   8.f * kCS,  -2.f * kCS },
			{   8.f * kCS,   0.f * kCS,   8.f * kCS,   2.f * kCS },
			{   8.f * kCS,   3.f * kCS,   8.f * kCS,   8.f * kCS },
			{   9.f * kCS,  -9.f * kCS,   9.f * kCS,  -8.f * kCS },
			{   9.f * kCS,  -7.f * kCS,   9.f * kCS,   4.f * kCS },
			// Horizontal lines
			{  -8.f * kCS,  -9.f * kCS,  -6.f * kCS,  -9.f * kCS },
			{  -3.f * kCS,  -9.f * kCS,  -1.f * kCS,  -9.f * kCS },
			{   2.f * kCS,  -9.f * kCS,   5.f * kCS,  -9.f * kCS },
			{   6.f * kCS,  -9.f * kCS,   8.f * kCS,  -9.f * kCS },
			{  -9.f * kCS,  -8.f * kCS,  -8.f * kCS,  -8.f * kCS },
			{  -7.f * kCS,  -8.f * kCS,  -6.f * kCS,  -8.f * kCS },
			{  -5.f * kCS,  -8.f * kCS,  -2.f * kCS,  -8.f * kCS },
			{  -1.f * kCS,  -8.f * kCS,   1.f * kCS,  -8.f * kCS },
			{   2.f * kCS,  -8.f * kCS,   3.f * kCS,  -8.f * kCS },
			{   5.f * kCS,  -8.f * kCS,   6.f * kCS,  -8.f * kCS },
			{   8.f * kCS,  -8.f * kCS,   9.f * kCS,  -8.f * kCS },
			{ -10.f * kCS,  -7.f * kCS,  -9.f * kCS,  -7.f * kCS },
			{  -6.f * kCS,  -7.f * kCS,  -5.f * kCS,  -7.f * kCS },
			{  -4.f * kCS,  -7.f * kCS,  -3.f * kCS,  -7.f * kCS },
			{   1.f * kCS,  -7.f * kCS,   2.f * kCS,  -7.f * kCS },
			{   4.f * kCS,  -7.f * kCS,   6.f * kCS,  -7.f * kCS },
			{   8.f * kCS,  -7.f * kCS,  10.f * kCS,  -7.f * kCS },
			{  -7.f * kCS,  -6.f * kCS,  -3.f * kCS,  -6.f * kCS },
			{   6.f * kCS,  -6.f * kCS,   8.f * kCS,  -6.f * kCS },
			{  -9.f * kCS,  -5.f * kCS,  -7.f * kCS,  -5.f * kCS },
			{  -6.f * kCS,  -5.f * kCS,  -5.f * kCS,  -5.f * kCS },
			{  -3.f * kCS,  -5.f * kCS,  -2.f * kCS,  -5.f * kCS },
			{   0.f * kCS,  -5.f * kCS,   1.f * kCS,  -5.f * kCS },
			{   2.f * kCS,  -5.f * kCS,   4.f * kCS,  -5.f * kCS },
			{   7.f * kCS,  -5.f * kCS,   8.f * kCS,  -5.f * kCS },
			{  -7.f * kCS,  -4.f * kCS,  -6.f * kCS,  -4.f * kCS },
			{  -2.f * kCS,  -4.f * kCS,   0.f * kCS,  -4.f * kCS },
			{   1.f * kCS,  -4.f * kCS,   2.f * kCS,  -4.f * kCS },
			{   4.f * kCS,  -4.f * kCS,   5.f * kCS,  -4.f * kCS },
			{   7.f * kCS,  -4.f * kCS,   9.f * kCS,  -4.f * kCS },
			{ -10.f * kCS,  -3.f * kCS,  -9.f * kCS,  -3.f * kCS },
			{  -8.f * kCS,  -3.f * kCS,  -5.f * kCS,  -3.f * kCS },
			{  -3.f * kCS,  -3.f * kCS,  -2.f * kCS,  -3.f * kCS },
			{  -1.f * kCS,  -3.f * kCS,   1.f * kCS,  -3.f * kCS },
			{   6.f * kCS,  -3.f * kCS,   8.f * kCS,  -3.f * kCS },
			{ -10.f * kCS,  -2.f * kCS,  -8.f * kCS,  -2.f * kCS },
			{  -6.f * kCS,  -2.f * kCS,  -4.f * kCS,  -2.f * kCS },
			{  -3.f * kCS,  -2.f * kCS,   2.f * kCS,  -2.f * kCS },
			{   7.f * kCS,  -2.f * kCS,   8.f * kCS,  -2.f * kCS },
			{  -8.f * kCS,  -1.f * kCS,  -4.f * kCS,  -1.f * kCS },
			{   5.f * kCS,  -1.f * kCS,   6.f * kCS,  -1.f * kCS },
			{   8.f * kCS,  -1.f * kCS,   9.f * kCS,  -1.f * kCS },
			{  -7.f * kCS,   0.f * kCS,  -6.f * kCS,   0.f * kCS },
			{  -5.f * kCS,   0.f * kCS,  -3.f * kCS,   0.f * kCS },
			{   6.f * kCS,   0.f * kCS,   7.f * kCS,   0.f * kCS },
			{   7.f * kCS,   0.f * kCS,   8.f * kCS,   0.f * kCS },
			{ -10.f * kCS,   1.f * kCS,  -9.f * kCS,   1.f * kCS },
			{  -8.f * kCS,   1.f * kCS,  -7.f * kCS,   1.f * kCS },
			{  -6.f * kCS,   1.f * kCS,  -4.f * kCS,   1.f * kCS },
			{   4.f * kCS,   1.f * kCS,   6.f * kCS,   1.f * kCS },
			{  -9.f * kCS,   2.f * kCS,  -8.f * kCS,   2.f * kCS },
			{  -7.f * kCS,   2.f * kCS,  -6.f * kCS,   2.f * kCS },
			{  -5.f * kCS,   2.f * kCS,  -4.f * kCS,   2.f * kCS },
			{   3.f * kCS,   2.f * kCS,   6.f * kCS,   2.f * kCS },
			{  -8.f * kCS,   3.f * kCS,  -7.f * kCS,   3.f * kCS },
			{  -4.f * kCS,   3.f * kCS,  -3.f * kCS,   3.f * kCS },
			{  -3.f * kCS,   3.f * kCS,   7.f * kCS,   3.f * kCS },
			{   8.f * kCS,   3.f * kCS,   9.f * kCS,   3.f * kCS },
			{  -9.f * kCS,   4.f * kCS,  -8.f * kCS,   4.f * kCS },
			{  -7.f * kCS,   4.f * kCS,  -6.f * kCS,   4.f * kCS },
			{  -5.f * kCS,   4.f * kCS,  -4.f * kCS,   4.f * kCS },
			{  -2.f * kCS,   4.f * kCS,  -1.f * kCS,   4.f * kCS },
			{   0.f * kCS,   4.f * kCS,   2.f * kCS,   4.f * kCS },
			{   4.f * kCS,   4.f * kCS,   6.f * kCS,   4.f * kCS },
			{  -8.f * kCS,   5.f * kCS,  -7.f * kCS,   5.f * kCS },
			{  -6.f * kCS,   5.f * kCS,  -3.f * kCS,   5.f * kCS },
			{  -1.f * kCS,   5.f * kCS,   1.f * kCS,   5.f * kCS },
			{   2.f * kCS,   5.f * kCS,   6.f * kCS,   5.f * kCS },
			{   9.f * kCS,   5.f * kCS,  10.f * kCS,   5.f * kCS },
			{  -9.f * kCS,   6.f * kCS,  -8.f * kCS,   6.f * kCS },
			{  -7.f * kCS,   6.f * kCS,  -6.f * kCS,   6.f * kCS },
			{  -4.f * kCS,   6.f * kCS,  -3.f * kCS,   6.f * kCS },
			{  -2.f * kCS,   6.f * kCS,   0.f * kCS,   6.f * kCS },
			{   4.f * kCS,   6.f * kCS,   5.f * kCS,   6.f * kCS },
			{   7.f * kCS,   6.f * kCS,   8.f * kCS,   6.f * kCS },
			{   8.f * kCS,   6.f * kCS,   9.f * kCS,   6.f * kCS },
			{ -10.f * kCS,   7.f * kCS,  -9.f * kCS,   7.f * kCS },
			{  -8.f * kCS,   7.f * kCS,  -5.f * kCS,   7.f * kCS },
			{  -5.f * kCS,   7.f * kCS,  -4.f * kCS,   7.f * kCS },
			{  -3.f * kCS,   7.f * kCS,  -1.f * kCS,   7.f * kCS },
			{   1.f * kCS,   7.f * kCS,   3.f * kCS,   7.f * kCS },
			{   6.f * kCS,   7.f * kCS,   7.f * kCS,   7.f * kCS },
			{   9.f * kCS,   7.f * kCS,  10.f * kCS,   7.f * kCS },
			{  -9.f * kCS,   8.f * kCS,  -8.f * kCS,   8.f * kCS },
			{  -4.f * kCS,   8.f * kCS,  -3.f * kCS,   8.f * kCS },
			{  -2.f * kCS,   8.f * kCS,   1.f * kCS,   8.f * kCS },
			{   7.f * kCS,   8.f * kCS,   9.f * kCS,   8.f * kCS },
			{  -8.f * kCS,   9.f * kCS,  -7.f * kCS,   9.f * kCS },
			{  -3.f * kCS,   9.f * kCS,   3.f * kCS,   9.f * kCS },
			{   4.f * kCS,   9.f * kCS,  10.f * kCS,   9.f * kCS },
		};
		// Fill the wall data
		std::vector<WallData> wall_data;
		wall_data.resize(sizeof(walls_base_data) / sizeof(walls_base_data[0]));
		for (size_t i = 0; i < wall_data.size(); ++i)
		{
			const float kBaseHeight = floor_model_->sizes().y;
			const float kWallHeight = 2.0f;
			const float kWallWidth = 1.0f;
			WallData& data = wall_data[i];
			const WallBaseData& base_data = walls_base_data[i];
			data.center.x = 0.5f * (base_data.end_y + base_data.start_y);
			data.center.y = 0.5f * kWallHeight + kBaseHeight;
			data.center.z = 0.5f * (base_data.end_x + base_data.start_x);
			// Different filling based on orientation
			if (base_data.end_y - base_data.start_y < 0.1f) // horizontal
			{
				data.sizes.x = 0.5f * (base_data.end_y - base_data.start_y + kWallWidth);
				data.sizes.y = 0.5f * kWallHeight;
				data.sizes.z = 0.5f * (base_data.end_x - base_data.start_x - kWallWidth);
			}
			else // vertical
			{
				data.sizes.x = 0.5f * (base_data.end_y - base_data.start_y - kWallWidth);
				data.sizes.y = 0.5f * kWallHeight;
				data.sizes.z = 0.5f * (base_data.end_x - base_data.start_x + kWallWidth);
			}
		}
		// Walls models
		{
			sht::graphics::Model * wall_model;
			for (size_t i = 0; i < wall_data.size(); ++i)
			{
				const WallData& data = wall_data[i];
				wall_model = new sht::graphics::PhysicalBoxModel(renderer_, data.sizes.x, data.sizes.y, data.sizes.z, kMaterialSize, kMaterialSize);
				wall_model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
				wall_model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
				wall_model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
				//wall_model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTangent, 3));
				//wall_model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kBinormal, 3));
				wall_model->Create();
				if (!wall_model->MakeRenderable())
				{
					delete wall_model;
					return false;
				}
				walls_models_.push_back(wall_model);
			}
		}
		
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
								   sht::graphics::Texture::Wrap::kRepeat,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(ball_normal_texture_, "data/textures/pbr/metal/rusted_iron/normal.png",
								   sht::graphics::Texture::Wrap::kRepeat,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(ball_roughness_texture_, "data/textures/pbr/metal/rusted_iron/roughness.png",
								   sht::graphics::Texture::Wrap::kRepeat,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(ball_metal_texture_, "data/textures/pbr/metal/rusted_iron/metallic.png",
								   sht::graphics::Texture::Wrap::kRepeat,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;

		if (!renderer_->AddTexture(maze_albedo_texture_, "data/textures/pbr/stone/marble/albedo.png",
								   sht::graphics::Texture::Wrap::kRepeat,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(maze_normal_texture_, "data/textures/pbr/stone/marble/normal.png",
								   sht::graphics::Texture::Wrap::kRepeat,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(maze_roughness_texture_, "data/textures/pbr/stone/marble/roughness.png",
								   sht::graphics::Texture::Wrap::kRepeat,
								   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;
		if (!renderer_->AddTexture(maze_metal_texture_, "data/textures/pbr/stone/marble/metallic.png",
								   sht::graphics::Texture::Wrap::kRepeat,
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

		// Create physics
		physics_ = new sht::physics::Engine(20 /* max sub steps */, 0.01f /* fixed time step */);

		// Setup physics objects
		{
			// Ball
			const float kBaseHeight = floor_model_->sizes().y + kBallRadius;
			ball_ = physics_->AddSphere(vec3(0.0f, kBaseHeight, 0.0f), 1.0f, kBallRadius);
			ball_->SetFriction(1.28f);
			ball_->SetRollingFriction(0.2f);
			ball_->SetSpinningFriction(0.5f);
			ball_->SetRestitution(0.0f);
			ball_->DisableDeactivation();

#ifndef ROTATING_PLATFORM
			sht::physics::ClampSpeedInfo clamp_speed_info;
			const float kMaxLinearVelocity = 3.0f;
			clamp_speed_info.max_linear_velocity_ = kMaxLinearVelocity;
			clamp_speed_info.max_angular_velocity_ = kMaxLinearVelocity / kBallRadius; // w = V / R
			clamp_speed_info.clamp_linear_velocity_ = true;
			clamp_speed_info.clamp_angular_velocity_ = true;
			physics_->AddClampedSpeedObject(ball_, clamp_speed_info);
#endif
		}
		{
			// Floor
			sht::graphics::PhysicalBoxModel * floor_model = dynamic_cast<sht::graphics::PhysicalBoxModel *>(floor_model_);
			vec3 sizes = floor_model->sizes();
			floor_ = physics_->AddBox(vec3(0.0f, 0.0f, 0.0f), 0.0f, sizes.x, sizes.y, sizes.z);
		}
		{
			// Walls
			for (size_t i = 0; i < wall_data.size(); ++i)
			{
				const WallData& data = wall_data[i];
				sht::physics::Object * box = physics_->AddBox(data.center, 0.0f, data.sizes.x, data.sizes.y, data.sizes.z);
				walls_.push_back(box);
			}
		}

		// Create camera
		camera_manager_ = new sht::utility::CameraManager();
		//camera_manager_->MakeFree(vec3(10.0f), vec3(0.0f));
		UpdateCamera();

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
		for (auto model : walls_models_)
		{
			if (model)
				delete model;
		}
		if (floor_model_)
			delete floor_model_;
		if (quad_)
			delete quad_;
		if (sphere_)
			delete sphere_;
	}
#ifndef ROTATING_PLATFORM
	void ApplyForces(float sec)
	{
		const float kPushPower = 10.0f;
		sht::math::Vector3 force(0.0f);
		bool any_key_pressed = false;
		if (keys_.key_down(sht::PublicKey::kA))
		{
			any_key_pressed = true;
			force.z -= kPushPower;
		}
		if (keys_.key_down(sht::PublicKey::kD))
		{
			any_key_pressed = true;
			force.z += kPushPower;
		}
		if (keys_.key_down(sht::PublicKey::kS))
		{
			any_key_pressed = true;
			force.x -= kPushPower;
		}
		if (keys_.key_down(sht::PublicKey::kW))
		{
			any_key_pressed = true;
			force.x += kPushPower;
		}
		if (any_key_pressed)
		{
			ball_->ApplyCentralForce(force);
		}
	}
#endif
	void UpdatePhysics(float sec) final
	{
#ifndef ROTATING_PLATFORM
		ApplyForces(sec);
#endif
		// Update physics engine
		physics_->Update(sec);
	}
	void Update() final
	{
		const float kFrameTime = GetFrameTime();

		camera_manager_->Update(kFrameTime);

		// Update UI
		ui_root_->UpdateAll(kFrameTime);

		if (ball_->position().y < 0.0f)
		{
			victory_board_->Move();
		}

#ifdef ROTATING_PLATFORM
		// Update maze rotation
		UpdateRotation(kFrameTime);
#endif

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
	void RenderFloorAndWalls()
	{
		renderer_->ChangeTexture(irradiance_rt_, 0);
		renderer_->ChangeTexture(prefilter_rt_, 1);
		renderer_->ChangeTexture(fg_texture_, 2);
		renderer_->ChangeTexture(maze_albedo_texture_, 3);
		renderer_->ChangeTexture(maze_normal_texture_, 4);
		renderer_->ChangeTexture(maze_roughness_texture_, 5);
		renderer_->ChangeTexture(maze_metal_texture_, 6);

		object_shader_->Bind();
		object_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
		object_shader_->Uniform3fv("u_camera.position", *camera_manager_->position());

		// Floor
		renderer_->PushMatrix();
		renderer_->MultMatrix(floor_->matrix());
		object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
		floor_model_->Render();
		renderer_->PopMatrix();
		// Walls
		size_t num_walls = walls_models_.size();
		for (size_t i = 0; i < num_walls; ++i)
		{
			sht::graphics::Model * model = walls_models_[i];
			sht::physics::Object * object = walls_[i];

			renderer_->PushMatrix();
			renderer_->MultMatrix(object->matrix());
			object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
			model->Render();
			renderer_->PopMatrix();
		}

		object_shader_->Unbind();

		renderer_->ChangeTexture(nullptr, 6);
		renderer_->ChangeTexture(nullptr, 5);
		renderer_->ChangeTexture(nullptr, 4);
		renderer_->ChangeTexture(nullptr, 3);
		renderer_->ChangeTexture(nullptr, 2);
		renderer_->ChangeTexture(nullptr, 1);
		renderer_->ChangeTexture(nullptr, 0);
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
		RenderFloorAndWalls();
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
		if (!info_board_->IsPosMax())
		{
			if (info_board_->IsPosMin())
				info_board_->RenderAll(); // render entire tree
			else
				info_board_->Render(); // render only board rect (smart hack for labels :D)
		}
		if (!victory_board_->IsPosMax())
		{
			if (victory_board_->IsPosMin())
				victory_board_->RenderAll(); // render entire tree
			else
				victory_board_->Render(); // render only board rect (smart hack for labels :D)
		}
		horizontal_slider_->RenderAll();
		vertical_slider_->RenderAll();
		
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
#ifdef ROTATING_PLATFORM
	void UpdateRotation(float frame_time)
	{
		const float kDesiredTime = 3.0f;
		const float kAccuracy = 0.01f;
		if (update_rotation_x_)
		{
			// Calculate desired acceleration to reach the target angle within desired time
			acceleration_x_ = (target_angle_x_ - maze_angle_x_ - velocity_x_ * kDesiredTime) * 2.0f / (kDesiredTime * kDesiredTime);
			// Then do Euler integration
			velocity_x_ += acceleration_x_ * frame_time;
			maze_angle_x_ += velocity_x_ * frame_time;
		}
		if (update_rotation_y_)
		{
			// Calculate desired acceleration to reach the target angle within desired time
			acceleration_y_ = (target_angle_y_ - maze_angle_y_ - velocity_y_ * kDesiredTime) * 2.0f / (kDesiredTime * kDesiredTime);
			// Then do Euler integration
			velocity_y_ += acceleration_y_ * frame_time;
			maze_angle_y_ += velocity_y_ * frame_time;
		}
		if (update_rotation_x_ || update_rotation_y_)
		{
			UpdateMazeMatrix();
			// Check if we've reached target angle within accuracy
			if (update_rotation_x_ && fabs(target_angle_x_ - maze_angle_x_) < kAccuracy)
			{
				update_rotation_x_ = false;
			}
			if (update_rotation_y_ && fabs(target_angle_y_ - maze_angle_y_) < kAccuracy)
			{
				update_rotation_y_ = false;
			}
		}
	}
	void OnHorizontalSliderMoved()
	{
		target_angle_x_ = (horizontal_slider_->pin_position() * 2.0f - 1.0f) * kMaxAngle;
		update_rotation_x_ = true;
	}
	void OnVerticalSliderMoved()
	{
		target_angle_y_ = (vertical_slider_->pin_position() * 2.0f - 1.0f) * kMaxAngle;
		update_rotation_y_ = true;
	}
#endif
	void OnKeyDown(sht::PublicKey key, int mods) final
	{
		const float kDeltaAngle = 0.1f;
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
			camera_alpha_ += kDeltaAngle;
			UpdateCamera();
		}
		else if (key == sht::PublicKey::kRight)
		{
			camera_alpha_ -= kDeltaAngle;
			UpdateCamera();
		}
		else if (key == sht::PublicKey::kUp)
		{
			if (camera_theta_ < 1.4f)
			{
				camera_theta_ += kDeltaAngle;
				UpdateCamera();
			}
		}
		else if (key == sht::PublicKey::kDown)
		{
			if (camera_theta_ > 0.1f)
			{
				camera_theta_ -= kDeltaAngle;
				UpdateCamera();
			}
		}
	}
	void OnMouseDown(sht::MouseButton button, int modifiers) final
	{
		if (sht::MouseButton::kLeft == button)
		{
			if (victory_exit_rect_->active())
			{
				Application::Terminate();
			}
			else if (info_ok_rect_->active())
			{
				info_ok_rect_->set_active(false);
				info_board_->Move();
			}
		}
#ifdef ROTATING_PLATFORM
		vec2 position(mouse_.x() / height_, mouse_.y() / height_);
		horizontal_slider_->OnTouchDown(position);
		vertical_slider_->OnTouchDown(position);
#endif
	}
	void OnMouseUp(sht::MouseButton button, int modifiers) final
	{
#ifdef ROTATING_PLATFORM
		vec2 position(mouse_.x() / height_, mouse_.y() / height_);
		horizontal_slider_->OnTouchUp(position);
		vertical_slider_->OnTouchUp(position);
#endif
	}
	void OnMouseMove() final
	{
		vec2 position(mouse_.x() / height_, mouse_.y() / height_);
		if (info_board_->IsPosMin())
			info_board_->SelectAll(position.x, position.y);
		if (victory_board_->IsPosMin())
			victory_board_->SelectAll(position.x, position.y);
#ifdef ROTATING_PLATFORM
		horizontal_slider_->OnTouchMove(position);
		if (horizontal_slider_->is_touched())
			OnHorizontalSliderMoved();
		vertical_slider_->OnTouchMove(position);
		if (vertical_slider_->is_touched())
			OnVerticalSliderMoved();
#endif
	}
	void OnSize(int w, int h) final
	{
		Application::OnSize(w, h);
		// To have correct perspective when resizing
		need_update_projection_matrix_ = true;
	}
#ifdef ROTATING_PLATFORM
	void UpdateMazeMatrix()
	{
		// float cos_x = cosf(maze_angle_x_);
		// float sin_x = sinf(maze_angle_x_);
		// sht::math::Matrix4 rotation_x = sht::math::Rotate4(cos_x, sin_x, 1.0f, 0.0f, 0.0f);
		// float cos_y = cosf(maze_angle_y_);
		// float sin_y = sinf(maze_angle_y_);
		// sht::math::Matrix4 rotation_y = sht::math::Rotate4(cos_y, sin_y, 0.0f, 0.0f, 1.0f);
		// maze_->SetTransform(maze_matrix_ * rotation_x * rotation_y);
		quat horizontal(vec3(1.0f, 0.0f, 0.0f), maze_angle_x_);
		quat vertical(vec3(0.0f, 0.0f, 1.0f), maze_angle_y_);
		quat orient(horizontal * vertical);
		vec3 gravity = orient.RotateVector(vec3(0.0f, -9.81f, 0.0f));
		physics_->SetGravity(gravity);
	}
#endif
	void CreateUI()
	{
		sht::utility::ui::Rect * rect;
		sht::utility::ui::Label * label;

		ui_root_ = new sht::utility::ui::Widget();

		// Info menu
		info_board_ = new sht::utility::ui::ColoredBoard(renderer_, gui_shader_,
			vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
			0.8f, // f32 width
			0.6f, // f32 height
			aspect_ratio_*0.5f - 0.4f, // f32 left
			0.2f, // f32 hmin
			1.0f, // f32 hmax
			1.0f, // f32 velocity
			true, // bool is_down
			true, // bool is vertical
			(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
			);
		ui_root_->AttachWidget(info_board_);
		{
			const wchar_t * kText = L"Controls:";
			label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
				vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
				0.10f, // text height
				wcslen(kText)+1, // buffer size
				0.25f, // x
				0.50f, // y
				(u32)sht::utility::ui::Flags::kRenderAlways // flags
				);
			info_board_->AttachWidget(label);
			label->SetText(kText);
		}
		{
			const wchar_t * kText = L"WASD - movement";
			label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
				vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
				0.10f, // text height
				wcslen(kText) + 1, // buffer size
				0.05f, // x
				0.30f, // y
				(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
			info_board_->AttachWidget(label);
			label->SetText(kText);
		}
		{
			const wchar_t * kText = L"Arrow keys - camera";
			label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
				vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
				0.10f, // text height
				wcslen(kText) + 1, // buffer size
				0.05f, // x
				0.20f, // y
				(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
			info_board_->AttachWidget(label);
			label->SetText(kText);
		}
		{
			rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
				0.20f, // x
				0.0f, // y
				0.4f, // width
				0.1f, // height
				(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
				);
			info_board_->AttachWidget(rect);
			info_ok_rect_ = rect;
			const wchar_t * kText = L"OK";
			label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
				vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
				0.1f, // text height
				wcslen(kText)+1, // buffer size
				0.0f, // x
				0.0f, // y
				(u32)sht::utility::ui::Flags::kRenderAlways // flags
				);
			rect->AttachWidget(label);
			label->SetText(kText);
			label->AlignCenter(rect->width(), rect->height());
		}

		// Victory menu
		victory_board_ = new sht::utility::ui::ColoredBoard(renderer_, gui_shader_,
			vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
			0.8f, // f32 width
			0.6f, // f32 height
			aspect_ratio_*0.5f - 0.4f, // f32 left
			0.2f, // f32 hmin
			1.0f, // f32 hmax
			1.0f, // f32 velocity
			false, // bool is_down
			true, // bool is vertical
			(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
			);
		ui_root_->AttachWidget(victory_board_);
		{
			const wchar_t * kText = L"Congratulations!";
			label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
				vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
				0.10f, // text height
				wcslen(kText)+1, // buffer size
				0.15f, // x
				0.50f, // y
				(u32)sht::utility::ui::Flags::kRenderAlways // flags
				);
			victory_board_->AttachWidget(label);
			label->SetText(kText);
		}
		{
			const wchar_t * kText = L"You have finished!";
			label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
				vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
				0.10f, // text height
				wcslen(kText) + 1, // buffer size
				0.15f, // x
				0.30f, // y
				(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
			victory_board_->AttachWidget(label);
			label->SetText(kText);
		}
		{
			rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
				0.2f, // x
				0.0f, // y
				0.4f, // width
				0.1f, // height
				(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
				);
			victory_board_->AttachWidget(rect);
			victory_exit_rect_ = rect;
			const wchar_t * kText = L"Exit";
			label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
				vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
				0.1f, // text height
				wcslen(kText)+1, // buffer size
				0.0f, // x
				0.0f, // y
				(u32)sht::utility::ui::Flags::kRenderAlways // flags
				);
			rect->AttachWidget(label);
			label->SetText(kText);
			label->AlignCenter(rect->width(), rect->height());
		}

#ifdef ROTATING_PLATFORM
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
			0.03f, // f32 bar_radius
			(u32)sht::utility::ui::Flags::kRenderAlways, // u32 flags
			sht::utility::ui::SliderColored::kCircle, // bar form
			sht::utility::ui::SliderColored::kCircle // pin form
			);
		horizontal_slider_->SetPinPosition(0.5f);
		ui_root_->AttachWidget(horizontal_slider_);
		vertical_slider_ = new sht::utility::ui::SliderColored(renderer_, gui_shader_,
			kBarColor, // vec4 bar_color
			kPinColorNormal, // vec4 pin_color_normal
			kPinColorTouch, // vec4 pin_color_touch
			0.0f, // f32 x
			0.2f, // f32 y
			0.2f, // f32 width
			0.8f, // f32 height
			0.03f, // f32 bar_radius
			(u32)sht::utility::ui::Flags::kRenderAlways, // u32 flags
			sht::utility::ui::SliderColored::kCircle, // bar form
			sht::utility::ui::SliderColored::kCircle // pin form
			);
		vertical_slider_->SetPinPosition(0.5f);
		ui_root_->AttachWidget(vertical_slider_);
#endif
	}
	void UpdateCamera()
	{
		quat horizontal(vec3(0.0f, -1.0f, 0.0f), camera_alpha_);
		quat vertical(vec3(0.0f, 0.0f, -1.0f), camera_theta_);
		quat orient(horizontal * vertical);
		camera_manager_->MakeAttached(orient, ball_->GetPositionPtr(), camera_distance_);
	}
	void UpdateProjectionMatrix()
	{
		if (need_update_projection_matrix_ || camera_manager_->animated())
		{
			need_update_projection_matrix_ = false;
			renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), 0.5f, 250.0f));
		}
	}
	
private:
	sht::graphics::Model * sphere_;
	sht::graphics::Model * quad_;
	sht::graphics::PhysicalBoxModel * floor_model_;
	std::vector<sht::graphics::Model *> walls_models_;

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
	sht::physics::Object * floor_;
	std::vector<sht::physics::Object *> walls_;

	sht::utility::ui::Widget * ui_root_;
	sht::utility::ui::ColoredBoard * info_board_;
	sht::utility::ui::ColoredBoard * victory_board_;
	sht::utility::ui::Rect * info_ok_rect_;
	sht::utility::ui::Rect * victory_exit_rect_;
#ifdef ROTATING_PLATFORM
	sht::utility::ui::SliderColored * horizontal_slider_;
	sht::utility::ui::SliderColored * vertical_slider_;
	sht::math::Matrix4 maze_matrix_;
#endif
	
	sht::math::Matrix4 projection_view_matrix_;

#ifdef ROTATING_PLATFORM
	// Since slider shouldn't control angle directly we will use target angle
	float target_angle_x_;
	float target_angle_y_;
	float velocity_x_;
	float velocity_y_;
	float acceleration_x_;
	float acceleration_y_;
	float maze_angle_x_;
	float maze_angle_y_;
#endif

	float camera_distance_;
	float camera_alpha_;
	float camera_theta_;

#ifdef ROTATING_PLATFORM
	bool update_rotation_x_;
	bool update_rotation_y_;
#endif
	bool need_update_projection_matrix_;
};

DECLARE_MAIN(APP_NAME);