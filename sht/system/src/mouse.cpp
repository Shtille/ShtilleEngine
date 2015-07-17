#include "../include/mouse.h"
#include <memory.h>

namespace sht {
    
    Mouse::Mouse()
    : x_(0), y_(0)
    , enabled_(true)
    {
        const int count = static_cast<int>(MouseButton::kCount);
        button_down = new bool[count];
        memset(button_down, 0, sizeof(bool)*count);
    }
    Mouse::~Mouse()
    {
        delete [] button_down;
    }
    int& Mouse::x()
    {
        return x_;
    }
    int& Mouse::y()
    {
        return y_;
    }
    bool& Mouse::enabled()
    {
        return enabled_;
    }
    
}