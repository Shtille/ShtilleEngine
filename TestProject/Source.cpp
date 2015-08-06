#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"
#include "../sht/graphics/include/model/cube_model.h"
#include "../sht/graphics/include/model/tetrahedron_model.h"
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
        cube_ = new sht::graphics::CubeModel(renderer_);
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
        if (!renderer_->AddShader(shader_, "data/shaders/shader", const_cast<char**>(attribs), 2))
            return false;
        
        if (!renderer_->AddShader(shader2_, "data/shaders/shader2", const_cast<char**>(attribs), 1))
            return false;
        
        renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), 0.1f, 100.0f));
        
        return true;
    }
    void Unload() final
    {
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
    }
    void Render() final
    {
        vec3 light_pos_eye = renderer_->view_matrix() * light_position;
        
        renderer_->ClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        renderer_->ClearColorAndDepthBuffers();
        
        renderer_->ChangeShader(shader_);
        renderer_->ChangeShaderUniformMatrix4fv("u_projection", renderer_->projection_matrix());
        renderer_->ChangeShaderUniformMatrix4fv("u_view", renderer_->view_matrix());
        renderer_->ChangeShaderUniform3fv("u_light_pos", light_pos_eye);
        
        // Draw first model
        renderer_->PushMatrix();
        renderer_->Translate(2.0f, 0.0f, 0.0f);
        renderer_->MultMatrix(rotate_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("u_model", renderer_->model_matrix());
        normal_matrix = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        renderer_->ChangeShaderUniformMatrix3fv("u_normal_matrix", normal_matrix);
        cube_->Render();
        renderer_->PopMatrix();
        
        renderer_->ChangeShader(shader2_);
        renderer_->ChangeShaderUniformMatrix4fv("u_projection", renderer_->projection_matrix());
        renderer_->ChangeShaderUniformMatrix4fv("u_view", renderer_->view_matrix());
        
        // Draw second model
        renderer_->PushMatrix();
        renderer_->Translate(light_position);
        renderer_->Scale(0.2f);
        renderer_->MultMatrix(rotate_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("u_model", renderer_->model_matrix());
        tetrahedron_->Render();
        renderer_->PopMatrix();
        
        renderer_->ChangeShader(nullptr);
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
    }
    
private:
    sht::graphics::CubeModel * cube_;
    sht::graphics::TetrahedronModel * tetrahedron_;
    sht::graphics::Shader * shader_;
    sht::graphics::Shader * shader2_;
    
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