#include "game_scene.h"

#include "material_binder.h"

#include "utility/include/event.h"
#include "system/include/time/time_manager.h"

GameScene::GameScene(sht::graphics::Renderer * renderer, MaterialBinder * material_binder)
: Scene(renderer)
, material_binder_(material_binder)
, text_shader_(nullptr)
, object_shader_(nullptr)
, font_(nullptr)
, ball_mesh_(nullptr)
, cue_mesh_(nullptr)
, rack_mesh_(nullptr)
, table_mesh_(nullptr)
, fps_text_(nullptr)
, camera_manager_(nullptr)
, physics_(nullptr)
, light_angle_(0.0f)
, light_distance_(10000.0f)
{
	// Register resources to load automatically on scene change
	text_shader_id_   = AddResourceIdByName(ConstexprStringId("shader_text"));
	object_shader_id_ = AddResourceIdByName(ConstexprStringId("shader_object"));
	font_id_          = AddResourceIdByName(ConstexprStringId("font_good_dog"));
	ball_mesh_id_     = AddResourceIdByName(ConstexprStringId("mesh_ball"));
	cue_mesh_id_      = AddResourceIdByName(ConstexprStringId("mesh_cue"));
	rack_mesh_id_     = AddResourceIdByName(ConstexprStringId("mesh_rack"));
	table_mesh_id_    = AddResourceIdByName(ConstexprStringId("mesh_table"));
}
GameScene::~GameScene()
{
	Unload();
}
void GameScene::Update()
{
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_time = time_manager->GetFrameTime();

	// Update light parameters
	light_angle_ += 0.2f * frame_time;
	light_position_.Set(light_distance_*cosf(light_angle_), light_distance_, light_distance_*sinf(light_angle_));

	// Update physics engine
	physics_->Update(frame_time);

	// Camera should be updated after physics
	camera_manager_->Update(frame_time);

	// Update view matrix
	renderer_->SetViewMatrix(camera_manager_->view_matrix());
}
void GameScene::RenderTable()
{
	object_shader_->Uniform4f("u_color", 1.0f, 1.0f, 0.0f, 1.0f);
	table_mesh_->Render();
}
void GameScene::RenderBalls()
{
	object_shader_->Uniform4f("u_color", 1.0f, 0.0f, 0.0f, 1.0f);
	ball_mesh_->Render();
}
void GameScene::RenderRack()
{

}
void GameScene::RenderCue()
{

}
void GameScene::RenderObjects()
{
	sht::math::Matrix3 normal_matrix;
	vec3 light_pos_eye = renderer_->view_matrix() * light_position_;

	object_shader_->Bind();
	object_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
	object_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
	object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	object_shader_->Uniform3fv("u_light_pos", light_pos_eye);
	normal_matrix = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
	object_shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix);

	RenderTable();
	RenderBalls();
	RenderRack();
	RenderCue();
}
void GameScene::RenderInterface()
{
	renderer_->DisableDepthTest();

	// Draw FPS
	text_shader_->Bind();
	text_shader_->Uniform1i("u_texture", 0);
	text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_rate = time_manager->GetFrameRate();
	fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", frame_rate);
	fps_text_->Render();

	renderer_->EnableDepthTest();
}
void GameScene::Render()
{
	RenderObjects();
	RenderInterface();
}
void GameScene::Load()
{
	// Link resources IDs with its resource pointers
	sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();

	text_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(text_shader_id_));
	object_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(object_shader_id_));
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));
	ball_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(ball_mesh_id_));
	cue_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(cue_mesh_id_));
	rack_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(rack_mesh_id_));
	table_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_mesh_id_));

	// Bind shader to material binder
	material_binder_->SetShader(object_shader_);

	// Create meshes that have been loaded earlier
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	ball_mesh_->MakeRenderable();
	cue_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	cue_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	cue_mesh_->MakeRenderable();
	rack_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	rack_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	rack_mesh_->MakeRenderable();
	table_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	table_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	table_mesh_->MakeRenderable();

	fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);

	// Create camera attached to the controlled ball
	camera_manager_ = new sht::utility::CameraManager();
	auto cam_id = camera_manager_->Add(vec3(2000.0f), vec3(0.0f));
	camera_manager_->SetCurrent(cam_id);
	camera_manager_->SetManualUpdate();

	// Create physics
	physics_ = new sht::physics::Engine();
}
void GameScene::Unload()
{
	if (physics_)
	{
		delete physics_;
		physics_ = nullptr;
	}
	if (camera_manager_)
	{
		delete camera_manager_;
		camera_manager_ = nullptr;
	}
	if (fps_text_)
	{
		delete fps_text_;
		fps_text_ = nullptr;
	}
}