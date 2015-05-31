#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"

class UserApp : public sht::OpenGlApplication 
{
public:
       bool Load()
       {
          sht::graphics::Image image;
          if (!image.LoadFromFile("ava.bmp"))
             return false;
          if (!image.Save(sht::graphics::Image::FileFormat::kJpg, "ava_new.bmp"))
             return false;
          return true;
       }
};

DECLARE_MAIN(UserApp);