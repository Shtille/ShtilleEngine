#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"

class UserApp : public sht::OpenGlApplication 
{
public:
       bool Load()
       {
          sht::graphics::Image image;
          if (!image.LoadFromFile("ava.png"))
             return false;
          if (!image.Save(sht::graphics::Image::FileFormat::kPng, "ava_new.png"))
             return false;
          return true;
       }
};

DECLARE_MAIN(UserApp);