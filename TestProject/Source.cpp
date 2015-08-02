#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"
#include "../sht/graphics/include/model/cube_model.h"
#include <cmath>

class UserApp : public sht::OpenGlApplication 
{
public:
    UserApp()
    : model1_(nullptr)
    , model2_(nullptr)
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
        model1_ = new sht::graphics::CubeModel(renderer_);
        model1_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
        model1_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
        model1_->Create();
        if (!model1_->MakeRenderable())
            return false;
        
        // Second model
        model2_ = new sht::graphics::CubeModel(renderer_);
        model2_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
        model2_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
        model2_->Create();
        if (!model2_->MakeRenderable())
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
        delete model1_;
        delete model2_;
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
        
        // TODO: unfirom different matrices !!!
        
        // Draw first model
        renderer_->PushMatrix();
        renderer_->Translate(2.0f, 0.0f, 0.0f);
        renderer_->MultMatrix(rotate_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("u_model", renderer_->model_matrix());
        model1_->Render();
        renderer_->PopMatrix();
        
        // Draw second model
        renderer_->PushMatrix();
        renderer_->Translate(0.0f, 0.0f, 2.0f);
        renderer_->MultMatrix(rotate_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("u_model", renderer_->model_matrix());
        model2_->Render();
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
    sht::graphics::CubeModel * model1_;
    sht::graphics::CubeModel * model2_;
    sht::graphics::Shader * shader_;
    
    sht::math::Matrix4 rotate_matrix;
    
    float angle_; //!< rotation angle of cube
};

DECLARE_MAIN(UserApp);