#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"
#include "../sht/graphics/include/model/cube_model.h"
#include <cmath>

class UserApp : public sht::OpenGlApplication 
{
public:
    UserApp()
    : model_(nullptr)
    , shader_(nullptr)
    {
        
    }
    const char* GetTitle(void)
    {
        return "Test project";
    }
    bool Load() final
    {
        model_ = new sht::graphics::CubeModel(renderer_);
        model_->Create();
        if (!model_->MakeRenderable())
            return false;
        
        const char *attribs[] = {"vertex"};
        if (!renderer_->AddShader(shader_, "shader", const_cast<char**>(attribs), 1))
            return false;
        
        projection_matrix = sht::math::PerspectiveMatrix(45.0f, width(), height(), 0.1f, 100.0f);
        view_matrix = sht::math::LookAt(vec3(5.0f), vec3(0.0f));
        
        return true;
    }
    void Unload() final
    {
        delete model_;
    }
    void Update() final
    {
        model_matrix = sht::math::Rotate4(cos(0.5f), sin(0.5f), 0.0f, 1.0f, 0.0f);
    }
    void Render() final
    {
        renderer_->ClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        renderer_->ClearColorAndDepthBuffers();
        
        renderer_->ChangeShader(shader_);
        renderer_->ChangeShaderUniformMatrix4fv("projection", projection_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("view", view_matrix);
        renderer_->ChangeShaderUniformMatrix4fv("model", model_matrix);
        
        // Draw cube model
        model_->Render();
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
    sht::graphics::CubeModel * model_;
    sht::graphics::Shader * shader_;
    
    sht::math::Matrix4 projection_matrix;
    sht::math::Matrix4 view_matrix;
    sht::math::Matrix4 model_matrix;
};

DECLARE_MAIN(UserApp);