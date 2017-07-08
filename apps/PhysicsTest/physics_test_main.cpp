#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/model/box_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/console.h"
#include "../../sht/utility/include/camera.h"
#include "../../sht/physics/include/physics_engine.h"

#include "simple_object.h"

class PhysicsTestApp : public sht::OpenGlApplication
{
public:
    PhysicsTestApp()
    : sphere_model_(nullptr)
    , ground_box_(nullptr)
    , object_shader_(nullptr)
    , gui_shader_(nullptr)
    , text_shader_(nullptr)
    , ball_texture_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, console_(nullptr)
    , camera_manager_(nullptr)
    , physics_(nullptr)
    , ball_(nullptr)
    , ball2_(nullptr)
    , box_(nullptr)
    , light_angle_(0.0f)
    , light_distance_(100.0f)
    , need_update_projection_matrix_(true)
    {
    }
    const char* GetTitle() final
    {
        return "Physics test";
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
    bool Load() final
    {
        // Sphere model
		sphere_model_ = new sht::graphics::SphereModel(renderer_, 20, 10);
		sphere_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		sphere_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		sphere_model_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
		sphere_model_->Create();
        if (!sphere_model_->MakeRenderable())
            return false;
        ground_box_ = new sht::graphics::SphereModel(renderer_, 20, 10, 50.0f);
        ground_box_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
        ground_box_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
        ground_box_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
        ground_box_->Create();
        if (!ground_box_->MakeRenderable())
            return false;
        
		// Load shaders
        const char *attribs[] = {"a_position", "a_normal", "a_texcoord"};
        if (!renderer_->AddShader(object_shader_, "data/shaders/apps/PhysicsTest/shader", attribs, 3)) return false;
        if (!renderer_->AddShader(text_shader_, "data/shaders/text", attribs, 1)) return false;
        if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored", attribs, 1)) return false;

        // Load textures
        if (!renderer_->AddTexture(ball_texture_, "data/textures/chess.jpg",
                                   sht::graphics::Texture::Wrap::kClampToEdge,
                                   sht::graphics::Texture::Filter::kTrilinearAniso)) return false;

        // Load fonts
        renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
		if (font_ == nullptr)
			return false;

		fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
        if (!fps_text_)
            return false;
        
        console_ = new sht::utility::Console(renderer_, font_, gui_shader_, text_shader_, 0.7f, 0.1f, 0.8f, aspect_ratio_);

        // Create physics
        physics_ = new sht::physics::Engine();
        physics_->SetGravity(vec3(0.0f, 0.0f, 0.0f));

        // Setup our objects
        sht::physics::Object * object;
        object = physics_->AddSphere(vec3(0.0f, 3.0f, 0.0f), 1.0f, 1.0f);
        object->SetFriction(0.5f);
        object->SetRollingFriction(0.1f);
        object->SetSpinningFriction(0.1f);
        object->SetRestitution(0.8f);
        ball_ = new sht::SimpleObject(renderer_, object_shader_, sphere_model_, object);

        object = physics_->AddSphere(vec3(0.0f, -50.0f, 0.0f), 0.0f, 50.0f);
        object->SetFriction(1.0f);
        ball2_ = new sht::SimpleObject(renderer_, object_shader_, ground_box_, object);

        // Ground
        // object = physics_->AddBox(vec3(0.0f, -50.0f, 0.0f), 0.0f, 50.0f, 50.0f, 50.0f);
        // object->SetFriction(1.0f);
        // //object->SetRollingFriction(1.0f);
        // box_ = new sht::SimpleObject(renderer_, object_shader_, ground_box_, object);

        // Create camera attached to the controlled ball
        camera_manager_ = new sht::utility::CameraManager();
        auto cam_id = camera_manager_->Add(quat(vec3(5.0f, 5.0f, 0.0f), vec3(0.0f)), 
            const_cast<vec3*>(ball_->body()->GetPositionPtr()), 10.0f);
        camera_manager_->SetCurrent(cam_id);
        camera_manager_->SetManualUpdate();

		// Finally bind constants
		BindShaderConstants();
        
        return true;
    }
    void Unload() final
    {
        if (box_)
            delete box_;
        if (ball2_)
            delete ball2_;
        if (ball_)
            delete ball_;
        if (physics_)
            delete physics_;
        if (camera_manager_)
            delete camera_manager_;
		if (console_)
			delete console_;
		if (fps_text_)
			delete fps_text_;
        if (ground_box_)
            delete ground_box_;
		if (sphere_model_)
			delete sphere_model_;
    }
    void UpdatePhysics()
    {
        const float kPushPower = 5.0f;
        // sht::math::Vector3 force(0.0f);
        // if (keys_.key_down(sht::PublicKey::kLeft))
        //     force.z += kPushPower;
        // if (keys_.key_down(sht::PublicKey::kRight))
        //     force.z -= kPushPower;
        // if (keys_.key_down(sht::PublicKey::kDown))
        //     force.x += kPushPower;
        // if (keys_.key_down(sht::PublicKey::kUp))
        //     force.x -= kPushPower;

        sht::math::Vector3 force = ball2_->body()->position() - ball_->body()->position();
        force.Normalize();
        force *= 10.0; // gravity force
        if (keys_.key_down(sht::PublicKey::kLeft))
            force.z += kPushPower;
        if (keys_.key_down(sht::PublicKey::kRight))
            force.z -= kPushPower;
        if (keys_.key_down(sht::PublicKey::kDown))
            force.x += kPushPower;
        if (keys_.key_down(sht::PublicKey::kUp))
            force.x -= kPushPower;
        // Update forces
        ball_->body()->Activate(); // this body may be sleeping, thus we activate it
        ball_->body()->ApplyCentralForce(force);

        // Update physics engine
        physics_->Update(GetFrameTime());
    }
    void Update() final
    {
        const float kFrameTime = GetFrameTime();

        // Update light parameters
        light_angle_ += 0.2f * kFrameTime;
        light_position_.Set(light_distance_*cosf(light_angle_), light_distance_, light_distance_*sinf(light_angle_));

        UpdatePhysics();

        console_->Update(kFrameTime);

        // Camera should be updated after physics
        camera_manager_->Update(kFrameTime);

        // Update matrices
        UpdateProjectionMatrix();
        renderer_->SetViewMatrix(camera_manager_->view_matrix());
        projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();

		BindShaderVariables();
    }
	void RenderObjects()
	{
        vec3 light_pos_eye = renderer_->view_matrix() * light_position_;

        object_shader_->Bind();
        object_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
        object_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
        object_shader_->Uniform3fv("u_light_pos", light_pos_eye);
        object_shader_->Uniform1i("u_texture", 0);

        renderer_->ChangeTexture(ball_texture_, 0);
        ball_->Render();
        ball2_->Render();
        //box_->Render();
        renderer_->ChangeTexture(nullptr, 0);
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
        
        // Draw console
        console_->Render();
        
        renderer_->EnableDepthTest();
    }
    void Render() final
    {
        renderer_->SetViewport(width_, height_);
        
        renderer_->ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        renderer_->ClearColorAndDepthBuffers();

		RenderObjects();

        RenderInterface();
    }
    void OnChar(unsigned short code)
    {
        if (console_->IsActive())
        {
            console_->ProcessCharInput(code);
        }
    }
    void OnKeyDown(sht::PublicKey key, int mods) final
    {
        // Console blocks key input
        if (console_->IsActive())
        {
            console_->ProcessKeyInput(key, mods);
        }
        else // process another input
        {
            if (key == sht::PublicKey::kF)
            {
                ToggleFullscreen();
            }
            else if (key == sht::PublicKey::kEscape)
            {
                Application::Terminate();
            }
            else if ((key == sht::PublicKey::kGraveAccent) && !(mods & sht::ModifierKey::kShift))
            {
                console_->Move();
            }
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
            
            const float znear = 0.1f;
            const float zfar = 100.0f;
            renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), znear, zfar));
        }
    }
    
private:
    sht::graphics::Model * sphere_model_;
    sht::graphics::Model * ground_box_;
    sht::graphics::Shader * object_shader_;
    sht::graphics::Shader * gui_shader_;
    sht::graphics::Shader * text_shader_;
    sht::graphics::Texture * ball_texture_;
    sht::graphics::Font * font_;
    sht::graphics::DynamicText * fps_text_;
    sht::utility::Console * console_;
    sht::utility::CameraManager * camera_manager_;

    sht::physics::Engine * physics_;
    sht::SimpleObject * ball_;
    sht::SimpleObject * ball2_;
    sht::SimpleObject * box_;
    
    sht::math::Matrix4 projection_view_matrix_;

    sht::math::Matrix3 normal_matrix_; //!< added to not allocate memory per each call

    // Light parameters
    float light_angle_;
    float light_distance_;
    sht::math::Vector3 light_position_;
    
    bool need_update_projection_matrix_;
};

DECLARE_MAIN(PhysicsTestApp);