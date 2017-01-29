#include "../../sht/include/sht.h"
#include "../../sht/graphics/include/model/sphere_model.h"
#include "../../sht/graphics/include/renderer/text.h"
#include "../../sht/utility/include/console.h"
#include "../../sht/utility/include/camera.h"

#include "physics_logics.h"

class PhysicsTestApp : public sht::OpenGlApplication
{
public:
    PhysicsTestApp()
    : sphere_(nullptr)
    , object_shader_(nullptr)
    , gui_shader_(nullptr)
    , text_shader_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, console_(nullptr)
    , camera_manager_(nullptr)
    , logics_(nullptr)
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
		sphere_ = new sht::graphics::SphereModel(renderer_, 20, 10);
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
		sphere_->Create();
        if (!sphere_->MakeRenderable())
            return false;
        
		// Load shaders
        const char *attribs[] = {"a_position", "a_normal", "a_texcoord"};
        if (!renderer_->AddShader(object_shader_, "data/shaders/apps/PhysicsTest/shader", attribs, 2)) return false;
        if (!renderer_->AddShader(text_shader_, "data/shaders/text", attribs, 1)) return false;
        if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored", attribs, 1)) return false;

        // Load fonts
        renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
		if (font_ == nullptr)
			return false;

		fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
        if (!fps_text_)
            return false;
        
        console_ = new sht::utility::Console(renderer_, font_, gui_shader_, text_shader_, 0.7f, 0.1f, 0.8f, aspect_ratio_);

        camera_manager_ = new sht::utility::CameraManager();
		camera_manager_->MakeFree(vec3(5.0f), vec3(0.0f));

        // Create physics
        logics_ = new sht::physics::Logics();
        logics_->SetupObjects();

		// Finally bind constants
		BindShaderConstants();
        
        return true;
    }
    void Unload() final
    {
        if (logics_)
            delete logics_;
        if (camera_manager_)
            delete camera_manager_;
		if (console_)
			delete console_;
		if (fps_text_)
			delete fps_text_;
		if (sphere_)
			delete sphere_;
    }
    void Update() final
    {
        // Update light parameters
        light_angle_ += 0.2f * frame_time_;
        light_position_.Set(light_distance_*cosf(light_angle_), light_distance_, light_distance_*sinf(light_angle_));

        console_->Update(frame_time_);

        camera_manager_->Update(frame_time_);

        // Update matrices
        renderer_->SetViewMatrix(camera_manager_->view_matrix());
        UpdateProjectionMatrix();
        projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();

        // Update physics
        logics_->Update(frame_time_);

		BindShaderVariables();
    }
	void RenderObjects()
	{
        vec3 light_pos_eye = renderer_->view_matrix() * light_position_;

        object_shader_->Bind();
        object_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
        object_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
        object_shader_->Uniform3fv("u_light_pos", light_pos_eye);
        
        // Draw first model
        vec3 position;
        logics_->GetPosition(&position);

        // renderer_->PushMatrix();
        // renderer_->Translate(2.0f, 0.0f, 0.0f);
        // object_shader_->Uniform3f("u_color", 1.0f, 0.0f, 0.0f);
        // object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        // normal_matrix_ = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        // object_shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix_);
        // sphere_->Render();
        // renderer_->PopMatrix();
        // renderer_->PushMatrix();
        // renderer_->Translate(3.0f, 0.0f, 0.0f);
        // object_shader_->Uniform3f("u_color", 1.0f, 0.0f, 0.0f);
        // object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        // normal_matrix_ = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        // object_shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix_);
        // sphere_->Render();
        // renderer_->PopMatrix();

        renderer_->PushMatrix();
        renderer_->Translate(position);
        object_shader_->Uniform3f("u_color", 0.0f, 1.0f, 0.0f);
        object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        normal_matrix_ = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        object_shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix_);
        sphere_->Render();
        renderer_->PopMatrix();
        // renderer_->PushMatrix();
        // renderer_->Translate(0.0f, 3.0f, 0.0f);
        // object_shader_->Uniform3f("u_color", 0.0f, 1.0f, 0.0f);
        // object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        // normal_matrix_ = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        // object_shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix_);
        // sphere_->Render();
        // renderer_->PopMatrix();

        renderer_->PushMatrix();
        renderer_->Translate(0.0f, 0.0f, 2.0f);
        object_shader_->Uniform3f("u_color", 0.0f, 0.0f, 1.0f);
        object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        normal_matrix_ = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        object_shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix_);
        sphere_->Render();
        renderer_->PopMatrix();
        renderer_->PushMatrix();
        renderer_->Translate(0.0f, 0.0f, 3.0f);
        object_shader_->Uniform3f("u_color", 0.0f, 0.0f, 1.0f);
        object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        normal_matrix_ = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        object_shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix_);
        sphere_->Render();
        renderer_->PopMatrix();
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
    sht::graphics::Model * sphere_;
    sht::graphics::Shader * object_shader_;
    sht::graphics::Shader * gui_shader_;
    sht::graphics::Shader * text_shader_;
    sht::graphics::Font * font_;
    sht::graphics::DynamicText * fps_text_;
    sht::utility::Console * console_;
    sht::utility::CameraManager * camera_manager_;

    sht::physics::Logics * logics_;
    
    sht::math::Matrix4 projection_view_matrix_;

    sht::math::Matrix3 normal_matrix_; //!< added to not allocate memory per each call

    // Light parameters
    float light_angle_;
    float light_distance_;
    sht::math::Vector3 light_position_;
    
    bool need_update_projection_matrix_;
};

DECLARE_MAIN(PhysicsTestApp);