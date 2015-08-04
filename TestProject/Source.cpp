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
    {
        
    }
    const char* GetTitle(void)
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
        tetrahedron_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
        tetrahedron_->Create();
        if (!tetrahedron_->MakeRenderable())
            return false;
        
        const char *attribs[] = {"a_position", "a_normal"};
        if (!renderer_->AddShader(shader_, "shader", const_cast<char**>(attribs), 1))
            return false;
        
        renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), 0.1f, 100.0f));
        renderer_->SetViewMatrix(sht::math::LookAt(vec3(5.0f), vec3(0.0f)));
        renderer_->SetModelMatrix(sht::math::Identity4());
        
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
    }
    void Render() final
    {
        renderer_->ClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        renderer_->ClearColorAndDepthBuffers();
        
        renderer_->ChangeShader(shader_);
        renderer_->ChangeShaderUniformMatrix4fv("u_projection", renderer_->projection_matrix());
        renderer_->ChangeShaderUniformMatrix4fv("u_view", renderer_->view_matrix());
        
        // Draw first model
        renderer_->PushMatrix();
        renderer_->Translate(2.0f, 0.0f, 0.0f);
        renderer_->MultMatrix(rotate_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("u_model", renderer_->model_matrix());
        cube_->Render();
        renderer_->PopMatrix();
        
        // Draw second model
        renderer_->PushMatrix();
        renderer_->Translate(0.0f, 0.0f, 2.0f);
        renderer_->MultMatrix(rotate_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("u_model", renderer_->model_matrix());
        tetrahedron_->Render();
        renderer_->PopMatrix();
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
    }
    
private:
    sht::graphics::CubeModel * cube_;
    sht::graphics::TetrahedronModel * tetrahedron_;
    sht::graphics::Shader * shader_;
    
    sht::math::Matrix4 rotate_matrix;
    
    float angle_; //!< rotation angle of cube
};

DECLARE_MAIN(UserApp);