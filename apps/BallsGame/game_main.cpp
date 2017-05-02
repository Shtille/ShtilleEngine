#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/model/box_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/camera.h"
#include "../../sht/utility/include/event.h"
#include "../../sht/physics/include/physics_engine.h"

#include "simple_object.h"
#include "parser.h"
#include "game_console.h"
#include "object_manager.h"

class BallsGameApp
    : public sht::OpenGlApplication
    , private sht::utility::EventListenerInterface
{
public:
    BallsGameApp()
    : gui_shader_(nullptr)
    , text_shader_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
    , camera_manager_(nullptr)
    , console_(nullptr)
    , parser_(nullptr)
    , physics_(nullptr)
    , object_manager_(nullptr)
    , ball_(nullptr)
    , light_angle_(0.0f)
    , light_distance_(100.0f)
    , need_update_projection_matrix_(true)
    {
    }
    const char* GetTitle() final
    {
        return "Balls game";
    }
    const bool IsMultisample() final
    {
        return false;
    }
	void BindShaderConstants()
	{
	}
	void BindShaderVariables()
	{
	}
    bool Load() final
    {
		// Load shaders
        const char *attribs[] = {"a_position", "a_normal", "a_texcoord"};
        if (!renderer_->AddShader(text_shader_, "data/shaders/text", attribs, 1)) return false;
        if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored", attribs, 1)) return false;

        // Load fonts
        renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
		if (font_ == nullptr)
			return false;

		fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
        if (!fps_text_)
            return false;

        // Create physics
        physics_ = new sht::physics::Engine();
        //physics_->SetGravity(vec3(0.0f, 0.0f, 0.0f));

        object_manager_ = new ObjectManager(renderer_, physics_, &light_pos_eye_, this);

        // Populate our scene with objects
        object_manager_->AddSphere(0.0f, 3.0f, 0.0f, 1.0f, 1.0f, "metal");
        ball_ = object_manager_->last_object();
        object_manager_->AddSphere(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, "metal");
        object_manager_->AddBox(0.0f, -10.0f, 0.0f, 0.0f, 50.0f, 10.0f, 50.0f, "metal");

        // Wrapper to console parser
        parser_ = new Parser(object_manager_);
        
        console_ = new GameConsole(renderer_, font_, gui_shader_, text_shader_, 0.7f, 0.05f, 0.8f, aspect_ratio_);
        console_->set_parser(parser_->object());

        // Create camera attached to the controlled ball
        camera_manager_ = new sht::utility::CameraManager();
        auto cam_id = camera_manager_->Add(quat(vec3(5.0f, 5.0f, 0.0f), vec3(0.0f)), 
            ball_->body()->GetPositionPtr(), 10.0f/* distance */);
        camera_manager_->SetCurrent(cam_id);
        camera_manager_->SetManualUpdate();

		// Finally bind constants
		BindShaderConstants();
        
        return true;
    }
    void Unload() final
    {
        if (camera_manager_)
            delete camera_manager_;
        if (console_)
            delete console_;
        if (parser_)
            delete parser_;
        if (object_manager_)
            delete object_manager_;
        if (physics_)
            delete physics_;
		if (fps_text_)
			delete fps_text_;
    }
    void UpdatePhysics()
    {
        if (!object_manager_->editor_mode())
        {
            const float kPushPower = 5.0f;
            sht::math::Vector3 force(0.0f);
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
        }

        // Update physics engine
        physics_->Update(frame_time_);
    }
    void Update() final
    {
        // Update light parameters
        light_angle_ += 0.2f * frame_time_;
        light_position_.Set(light_distance_*cosf(light_angle_), light_distance_, light_distance_*sinf(light_angle_));

        UpdatePhysics();

        // Camera should be updated after physics
        camera_manager_->Update(frame_time_);

        console_->Update(frame_time_);

        // Update matrices
        UpdateProjectionMatrix();
        renderer_->SetViewMatrix(camera_manager_->view_matrix());
        projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();
        light_pos_eye_ = renderer_->view_matrix() * light_position_;

		BindShaderVariables();
    }
	void RenderObjects()
	{
        object_manager_->RenderAll();
	}
    void RenderInterface()
    {
        renderer_->DisableDepthTest();
        
        // Draw FPS
        text_shader_->Bind();
        text_shader_->Uniform1i("u_texture", 0);
        text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
        fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", frame_rate_);
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
            else if (key == sht::PublicKey::kSpace)
            {
                // For debug purposes only
                ball_->body()->SetPosition(vec3(0.0f, 3.0f, 0.0f));
            }
        }
    }
    void OnMouseDown(sht::MouseButton button, int modifiers) final
    {
        if (object_manager_->editor_mode() && button == sht::MouseButton::kLeft)
        {
            vec2 mouse_pos(mouse_.x(), mouse_.y());
            vec3 ray;
            sht::math::ScreenToRay(mouse_pos, renderer_->viewport(),
                renderer_->projection_matrix(), renderer_->view_matrix(), ray);
            vec4 plane(0.0f, 1.0f, 0.0f, 0.0f);
            vec3 intersection;
            if (sht::math::RayPlaneIntersection(*camera_manager_->position(), ray, plane, intersection))
            {
                old_intersection_ = intersection;
            }
        }
    }
    void OnMouseUp(sht::MouseButton button, int modifiers) final
    {
    }
    void OnMouseMove() final
    {
        if (object_manager_->editor_mode() && mouse_.button_down(sht::MouseButton::kLeft))
        {
            vec2 mouse_pos(mouse_.x(), mouse_.y());
            vec3 ray;
            sht::math::ScreenToRay(mouse_pos, renderer_->viewport(),
                renderer_->projection_matrix(), renderer_->view_matrix(), ray);
            vec4 plane(0.0f, 1.0f, 0.0f, 0.0f);
            vec3 intersection;
            if (sht::math::RayPlaneIntersection(*camera_manager_->position(), ray, plane, intersection))
            {
                camera_manager_->Move(old_intersection_ - intersection);
                old_intersection_ = intersection;
            }
        }
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
    void OnEvent(const sht::utility::Event * event) final
    {
        switch (event->type())
        {
        case SID("editor_mode_changed"):
            if (object_manager_->editor_mode())
                camera_manager_->MakeFree(0); // CameraID
            else
                camera_manager_->SetCurrent(0); // i guess CameraID will be 0
            break;
        }
    }
    
private:
    sht::graphics::Shader * gui_shader_;
    sht::graphics::Shader * text_shader_;
    sht::graphics::Font * font_;
    sht::graphics::DynamicText * fps_text_;
    sht::utility::CameraManager * camera_manager_;
    GameConsole * console_;
    Parser * parser_;
    sht::physics::Engine * physics_;
    ObjectManager * object_manager_;

    sht::SimpleObject * ball_;
    
    sht::math::Matrix4 projection_view_matrix_;

    sht::math::Matrix3 normal_matrix_; //!< added to not allocate memory per each call

    sht::math::Vector3 old_intersection_;

    // Light parameters
    float light_angle_;
    float light_distance_;
    sht::math::Vector3 light_position_;
    sht::math::Vector3 light_pos_eye_;
    
    bool need_update_projection_matrix_;
};

DECLARE_MAIN(BallsGameApp);