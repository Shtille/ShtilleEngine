#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"
#include "../sht/graphics/include/model/cube_model.h"
#include "../sht/graphics/include/model/tetrahedron_model.h"
#include "../sht/graphics/include/model/sphere_model.h"
#include "../sht/graphics/include/renderer/text.h"
#include "../sht/utility/include/console.h"
#include <cmath>

class UserApp : public sht::OpenGlApplication 
{
public:
    UserApp()
    : cube_(nullptr)
    , tetrahedron_(nullptr)
    , shader_(nullptr)
    , angle_(0.0f)
    , light_angle(0.0f)
    , view_alpha(0.0f)
    , view_theta(0.0f)
    {
        light_position.Set(5.0f, 5.0f, 5.0f);
    }
    const char* GetTitle(void) final
    {
        return "Test project";
    }
    bool Load() final
    {
        // First model
        cube_ = new sht::graphics::SphereModel(renderer_);
        cube_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
        cube_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
        cube_->Create();
        if (!cube_->MakeRenderable())
            return false;
        
        // Second model
        tetrahedron_ = new sht::graphics::TetrahedronModel(renderer_);
        tetrahedron_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
        tetrahedron_->Create();
        if (!tetrahedron_->MakeRenderable())
            return false;
        
        const char *attribs[] = {"a_position", "a_normal"};
        if (!renderer_->AddShader(shader_, "data/shaders/shader", attribs, 2))
            return false;
        
        if (!renderer_->AddShader(shader2_, "data/shaders/shader2", attribs, 1))
            return false;
        
        if (!renderer_->AddShader(text_shader_, "data/shaders/text_shader", attribs, 1))
            return false;
        
        if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored", attribs, 1))
            return false;
        
        renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), 0.1f, 100.0f));
        
        renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
//        text_ = sht::graphics::StaticText::Create(renderer_, font_, 0.1f, 0.0f, 0.5f, L"Brown F0x\njumps over the lazy dog");
        text_ = sht::graphics::DynamicText::Create(renderer_, 30);
        if (!text_)
            return false;
        
        console_ = new sht::utility::Console(renderer_, font_, gui_shader_, text_shader_, 0.7f, 0.1f, 0.8f, aspect_ratio_);
        
        return true;
    }
    void Unload() final
    {
        delete console_;
        delete text_;
        delete cube_;
        delete tetrahedron_;
    }
    void Update() final
    {
        angle_ += 0.5f * frame_time_;
        rotate_matrix = sht::math::Rotate4(cos(angle_), sin(angle_), 0.0f, 1.0f, 0.0f);
        
        light_angle += 0.2f * frame_time_;
        light_position.Set(5.0f*cosf(light_angle), 5.0f, 5.0f*sinf(light_angle));
        
        camera_position.Set(10.0f*cosf(view_theta)*cosf(view_alpha),
                            10.0f*sinf(view_theta),
                            10.0f*cosf(view_theta)*sinf(view_alpha));
        renderer_->SetViewMatrix(sht::math::LookAt(camera_position, vec3(0.0f)));
        
        console_->Update(frame_time_);
    }
    void Render() final
    {
        vec3 light_pos_eye = renderer_->view_matrix() * light_position;
        
        renderer_->Viewport(width_, height_);
        
        renderer_->ClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        renderer_->ClearColorAndDepthBuffers();
        
        shader_->Bind();
        shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
        shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
        shader_->Uniform3fv("u_light_pos", light_pos_eye);
        
        // Draw first model
        renderer_->PushMatrix();
        renderer_->Translate(2.0f, 0.0f, 0.0f);
        renderer_->MultMatrix(rotate_matrix);
        shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        normal_matrix = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix);
        cube_->Render();
        renderer_->PopMatrix();
        
        shader2_->Bind();
        shader2_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
        shader2_->UniformMatrix4fv("u_view", renderer_->view_matrix());
        
        // Draw second model
        renderer_->PushMatrix();
        renderer_->Translate(light_position);
        renderer_->Scale(0.2f);
        renderer_->MultMatrix(rotate_matrix);
        shader2_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        tetrahedron_->Render();
        renderer_->PopMatrix();
        
        // Draw text
        text_shader_->Bind();
        text_shader_->Uniform1i("u_texture", 0);
        text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
        text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", frame_rate_);
        text_->Render();
        
        // Draw console
        console_->Render();
        
        shader_->Unbind();
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
            else if (key == sht::PublicKey::kLeft)
            {
                view_alpha += 0.1f;
            }
            else if (key == sht::PublicKey::kRight)
            {
                view_alpha -= 0.1f;
            }
            else if (key == sht::PublicKey::kUp)
            {
                view_theta += 0.1f;
            }
            else if (key == sht::PublicKey::kDown)
            {
                view_theta -= 0.1f;
            }
            else if ((key == sht::PublicKey::kGraveAccent) && !(mods & sht::ModifierKey::kShift))
            {
                console_->Move();
            }
        }
    }
    void OnMouseMove() final
    {
        if (mouse_.button_down(sht::MouseButton::kLeft))
        {
            view_alpha += mouse_.delta_x()/width_;
            view_theta += mouse_.delta_y()/height_;
        }
        CursorToCenter();
    }
    void OnSize(int w, int h) final
    {
        Application::OnSize(w, h);
        // To have correct perspective when resizing
        renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), 0.1f, 100.0f));
    }
    
private:
    sht::graphics::Model * cube_;
    sht::graphics::Model * tetrahedron_;
    sht::graphics::Shader * shader_;
    sht::graphics::Shader * shader2_;
    sht::graphics::Shader * gui_shader_;
    sht::graphics::Shader * text_shader_;
    sht::graphics::Font * font_;
    sht::graphics::DynamicText * text_;
    sht::utility::Console * console_;
    
    sht::math::Matrix4 rotate_matrix;
    sht::math::Matrix3 normal_matrix;
    
    float angle_; //!< rotation angle of cube
    float light_angle;
    
    sht::math::Vector3 light_position;
    
    // Viewer params
    vec3 camera_position;
    float view_alpha;
    float view_theta;
};

DECLARE_MAIN(UserApp);