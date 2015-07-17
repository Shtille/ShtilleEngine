#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"

class UserApp : public sht::OpenGlApplication 
{
public:
    const char* GetTitle(void)
    {
        return "Test project";
    }
    bool Load() final
    {
        return true;
    }
    void Render() final
    {   
        renderer_->ClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        renderer_->ClearColorBuffer();
        
        // Draw cube model
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
};

DECLARE_MAIN(UserApp);