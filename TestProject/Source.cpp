#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"

class UserApp : public sht::OpenGlApplication 
{
public:
    const char* GetTitle(void)
    {
        return "Test project";
    }
    bool Load()
    {
        return true;
    }
    void Render()
    {   
        renderer_->ClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        renderer_->ClearColorBuffer();
    }
    bool OnKeyDown(unsigned short key)
    {
        const bool down_esc = keyt().key_down(sht::PublicKeys::kEscape);
        if (key == 'f')
        {
            ToggleFullscreen();
        }
        else if (key == 27)
        {
            Application::Terminate();
        }
        return true;
    }
};

DECLARE_MAIN(UserApp);