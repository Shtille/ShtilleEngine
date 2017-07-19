#include "game_scene.h"

#include "material_binder.h"

#include "utility/include/event.h"
#include "physics/include/physics_object.h"

#include <cmath> // for sinf and cosf
#include <cstdio>

GameScene::GameScene(sht::graphics::Renderer * renderer, MaterialBinder * material_binder, GameMode game_mode)
: Scene(renderer)
, material_binder_(material_binder)
, game_mode_(game_mode)
, text_shader_(nullptr)
, object_shader_(nullptr)
, ball_shader_(nullptr)
, font_(nullptr)
, ball_mesh_(nullptr)
, cue_mesh_(nullptr)
, rack_mesh_(nullptr)
, table_mesh_(nullptr)
, fps_text_(nullptr)
, camera_manager_(nullptr)
, physics_(nullptr)
, balls_(nullptr)
, balls_count_(1)
, light_angle_(0.0f)
, light_distance_(10000.0f)
{
	// Add timers
	const float kBallRequestInterval = 1.0f;
	spawn_timer_ = sht::system::TimeManager::GetInstance()->AddTimer(kBallRequestInterval);
	pocket_entrance_timer_ = sht::system::TimeManager::GetInstance()->AddTimer(kBallRequestInterval);

	// Register resources to load automatically on scene change
	text_shader_id_   					= AddResourceIdByName(ConstexprStringId("shader_text"));
	object_shader_id_ 					= AddResourceIdByName(ConstexprStringId("shader_object"));
	ball_shader_id_ 					= AddResourceIdByName(ConstexprStringId("shader_ball"));
	font_id_          					= AddResourceIdByName(ConstexprStringId("font_good_dog"));
	ball_mesh_id_     					= AddResourceIdByName(ConstexprStringId("mesh_ball"));
	cue_mesh_id_      					= AddResourceIdByName(ConstexprStringId("mesh_cue"));
	rack_mesh_id_     					= AddResourceIdByName(ConstexprStringId("mesh_rack"));
	table_mesh_id_    					= AddResourceIdByName(ConstexprStringId("mesh_table"));
	table_bed_mesh_id_					= AddResourceIdByName(ConstexprStringId("mesh_table_bed"));
	table_cushions_graphics_mesh_id_	= AddResourceIdByName(ConstexprStringId("mesh_table_cushions_graphics"));
	table_cushions_physics_mesh_id_		= AddResourceIdByName(ConstexprStringId("mesh_table_cushions_physics"));
}
GameScene::~GameScene()
{
	Unload();

	// Remove timers
	sht::system::TimeManager::GetInstance()->RemoveTimer(pocket_entrance_timer_);
	sht::system::TimeManager::GetInstance()->RemoveTimer(spawn_timer_);
}
void GameScene::SetGameMode(GameMode game_mode)
{
	game_mode_ = game_mode;
}
void GameScene::Update()
{
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_time = time_manager->GetFixedFrameTime();

	// Update light parameters
	light_angle_ += 0.2f * frame_time;
	light_position_.Set(light_distance_*cosf(light_angle_), light_distance_, light_distance_*sinf(light_angle_));

	// Camera should be updated after physics
	camera_manager_->Update(frame_time);

	// Check balls status
	CheckBallsStatus();

	// Update view matrix
	renderer_->SetViewMatrix(camera_manager_->view_matrix());
	projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();

	// Finally update shader variables
	BindShaderVariables();
}
void GameScene::UpdatePhysics(float sec)
{
	// Update physics engine
	physics_->Update(sec);
}
void GameScene::BindShaderConstants()
{
	const sht::math::Vector3 kLightColor(1.0f);

	object_shader_->Bind();
	object_shader_->Uniform3fv("u_light.color", kLightColor);

	ball_shader_->Bind();
	ball_shader_->Uniform1i("u_texture", 0);
	ball_shader_->Uniform3fv("u_light.color", kLightColor);

	ball_shader_->Unbind();
}
void GameScene::BindShaderVariables()
{
	object_shader_->Bind();
	object_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
	object_shader_->Uniform3fv("u_light.position", light_position_);
	object_shader_->Uniform3fv("u_eye_position", *camera_manager_->position());

	ball_shader_->Bind();
	ball_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
	ball_shader_->Uniform3fv("u_light.position", light_position_);
	ball_shader_->Uniform3fv("u_eye_position", *camera_manager_->position());

	ball_shader_->Unbind();
}
void GameScene::RenderTable()
{
	object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	table_mesh_->Render();
	table_bed_mesh_->Render();
	table_cushions_graphics_mesh_->Render();
}
void GameScene::RenderBalls()
{
	for (unsigned int i = 0; i < balls_count_; ++i)
	{
		renderer_->ChangeTexture(ball_textures_[i]);
		renderer_->PushMatrix();
		renderer_->MultMatrix(balls_[i]->matrix());
		ball_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
		ball_mesh_->Render();
		renderer_->PopMatrix();
	}
	renderer_->ChangeTexture(nullptr);
}
void GameScene::RenderRack()
{
	object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	rack_mesh_->Render();
}
void GameScene::RenderCue()
{
	//renderer_->PushMatrix();
	//renderer_->MultMatrix(cue_->matrix());
	//object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	//cue_mesh_->Render();
	//renderer_->PopMatrix();
}
void GameScene::RenderObjects()
{
	object_shader_->Bind();
	RenderTable();
	RenderRack();
	RenderCue();

	ball_shader_->Bind();
	RenderBalls();
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
	ball_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(ball_shader_id_));
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));
	ball_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(ball_mesh_id_));
	cue_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(cue_mesh_id_));
	rack_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(rack_mesh_id_));
	table_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_mesh_id_));
	table_bed_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_bed_mesh_id_));
	table_cushions_graphics_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_cushions_graphics_mesh_id_));
	table_cushions_physics_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_cushions_physics_mesh_id_));

	// Bind shader to material binder
	material_binder_->SetShader(object_shader_);

	// Create text objects
	fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);

	// Create camera attached to the controlled ball
	camera_manager_ = new sht::utility::CameraManager();
	camera_manager_->MakeFree(vec3(2000.0f), vec3(0.0f));

	// Create physics
	physics_ = new sht::physics::Engine(20 /* max sub steps */, 0.002f /* fixed time step */);
	physics_->SetGravity(vec3(0.0f, -9800.0f, 0.0f));

	constexpr unsigned int kBallCount = 16;
	balls_count_ = kBallCount;
	balls_ = new sht::physics::Object *[balls_count_];

	// Setup physics objects
	sht::physics::Object * object;
	{
		const float ball_size = ball_mesh_->bounding_box().extent.x;
		const float lx = 2.0f * ball_size * cosf(sht::math::kPi / 6.0);
		const float lz = 2.0f * ball_size * sinf(sht::math::kPi / 6.0);
		const float bx = 0.5f * table_bed_mesh_->bounding_box().extent.x;
		const vec3 ball_positions[16] = {
			vec3(-bx, ball_size, 0.0f),
			vec3(bx - 2.0f * lx, ball_size, 0.0f), // *
			vec3(bx - 1.0f * lx, ball_size, - 1.0f * lz),
			vec3(bx - 1.0f * lx, ball_size, - 1.0f * lz + 2.0f * ball_size),
			vec3(bx, ball_size, 0.0f),
			vec3(bx, ball_size, -2.0f * ball_size),
			vec3(bx, ball_size,  2.0f * ball_size),
			vec3(bx + 1.0f * lx, ball_size, - 1.0f * lz - 2.0f * ball_size),
			vec3(bx + 1.0f * lx, ball_size, - 1.0f * lz),
			vec3(bx + 1.0f * lx, ball_size, + 1.0f * lz),
			vec3(bx + 1.0f * lx, ball_size, + 1.0f * lz + 2.0f * ball_size),
			vec3(bx + 2.0f * lx, ball_size, -4.0f * ball_size),
			vec3(bx + 2.0f * lx, ball_size, -2.0f * ball_size),
			vec3(bx + 2.0f * lx, ball_size, 0.0f),
			vec3(bx + 2.0f * lx, ball_size, +2.0f * ball_size),
			vec3(bx + 2.0f * lx, ball_size, +4.0f * ball_size),
		};
		static_assert(kBallCount <= sizeof(ball_positions)/sizeof(ball_positions[0]), "balls count mismatch");
		for (unsigned int i = 0; i < balls_count_; ++i)
		{
			object = physics_->AddSphere(ball_positions[i], 0.150f, ball_size);
			object->SetFriction(0.28f);
			object->SetRollingFriction(0.1f);
			object->SetSpinningFriction(0.5f);
			object->SetRestitution(0.98f);
			balls_[i] = object;
		}
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(table_mesh_);
		object = physics_->AddMesh(vec3(0.0f, 0.0f, 0.0f), 0.0f, &enumerator);
		object->SetRestitution(0.5f);
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(table_bed_mesh_);
		object = physics_->AddMesh(vec3(0.0f, 0.0f, 0.0f), 0.0f, &enumerator);
		object->SetFriction(0.71f);
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(table_cushions_physics_mesh_);
		object = physics_->AddMesh(vec3(0.0f, 0.0f, 0.0f), 0.0f, &enumerator);
		object->SetFriction(0.2f);
		object->SetRestitution(0.8f);
	}

	// Create ball textures
	ball_textures_ = new sht::graphics::Texture *[balls_count_];
	if (game_mode_ == GameMode::kSimplePool)
	{
		const vec3 ball_colors[16] = {
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f),
		};
		static_assert(kBallCount <= sizeof(ball_colors)/sizeof(ball_colors[0]), "enlarge ball_colors array");
		for (unsigned int i = 0; i < balls_count_; ++i)
		{
			const vec3& color = ball_colors[i];
			renderer_->CreateTextureColor(ball_textures_[i], color.x, color.y, color.z, 1.0f);
		}
	}

	// Create meshes that have been loaded earlier
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
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
	table_bed_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	table_bed_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	table_bed_mesh_->MakeRenderable();
	table_cushions_graphics_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	table_cushions_graphics_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	table_cushions_graphics_mesh_->MakeRenderable();

	BindShaderConstants();
}
void GameScene::Unload()
{
	if (ball_textures_)
	{
		if (game_mode_ == GameMode::kSimplePool)
		{
			for (unsigned int i = 0; i < balls_count_; ++i)
				renderer_->DeleteTexture(ball_textures_[i]);
		}
		delete[] ball_textures_;
		ball_textures_ = nullptr;
	}
	if (balls_)
	{
		delete[] balls_;
		balls_ = nullptr;
	}
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
void GameScene::RespawnCueBall(const vec3& position)
{
	sht::physics::Object * cue_ball = balls_[0];
	cue_ball->Activate();
	cue_ball->SetPosition(position);
	cue_ball->SetLinearVelocity(vec3(0.0f));
	cue_ball->SetAngularVelocity(vec3(0.0f));
	spawn_timer_->Start();
	printf("respawn has started\n");
}
void GameScene::CheckBallsStatus()
{
	if (spawn_timer_->enabled() && spawn_timer_->HasExpired())
	{
		sht::physics::Object * cue_ball = balls_[0];
		if (!cue_ball->IsActive())
		{
			// Ball has been deactivated
			spawn_timer_->Stop();
			printf("respawn has finished\n");
		}
		spawn_timer_->Reset();
	}
	if (pocket_entrance_timer_->enabled() && pocket_entrance_timer_->HasExpired())
	{
		sht::physics::Object * cue_ball = balls_[0];
		if (cue_ball->position().y < 0.0f)
		{
			// Ball has fallen to pocket (or outside the table)
			pocket_entrance_timer_->Stop();
			printf("ball has gotten into the pocket\n");
		}
		pocket_entrance_timer_->Reset();
	}
}
void GameScene::OnKeyDown(sht::PublicKey key, int mods)
{
	bool velocity_been_set = false;
	const float kPushPower = 2500.0f;
	sht::math::Vector3 velocity(0.0f);
	if (key == sht::PublicKey::kA)
	{
		velocity_been_set = true;
		velocity.z += kPushPower;
	}
	if (key == sht::PublicKey::kD)
	{
		velocity_been_set = true;
		velocity.z -= kPushPower;
	}
	if (key == sht::PublicKey::kS)
	{
		velocity_been_set = true;
		velocity.x += kPushPower;
	}
	if (key == sht::PublicKey::kW)
	{
		velocity_been_set = true;
		velocity.x -= kPushPower;
	}

	// Update forces
	if (velocity_been_set)
	{
		balls_[0]->Activate(); // this body may be sleeping, thus we activate it
		balls_[0]->SetLinearVelocity(velocity);
		pocket_entrance_timer_->Start();
	}

	if (key == sht::PublicKey::kSpace)
	{
		RespawnCueBall(vec3(0.0f, 50.0f, 0.0f));
	}

	if (key == sht::PublicKey::kLeft)
		camera_manager_->RotateAroundTargetInY(-0.1f);
	else if (key == sht::PublicKey::kRight)
		camera_manager_->RotateAroundTargetInY(0.1f);
	else if (key == sht::PublicKey::kUp)
		camera_manager_->RotateAroundTargetInZ(0.1f);
	else if (key == sht::PublicKey::kDown)
		camera_manager_->RotateAroundTargetInZ(-0.1f);
}