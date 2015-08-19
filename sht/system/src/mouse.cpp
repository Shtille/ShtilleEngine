#include "../include/mouse.h"
#include <assert.h>

namespace sht {
    
    Mouse::Mouse()
    : x_(0), y_(0)
    , enabled_(true)
    {
        button_down_table_[MouseButton::kUnknown] = false;
        button_down_table_[MouseButton::kLeft] = false;
        button_down_table_[MouseButton::kMiddle] = false;
        button_down_table_[MouseButton::kRight] = false;
        assert(button_down_table_.size() == (size_t)MouseButton::kCount);
    }
    Mouse::~Mouse()
    {
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
    bool& Mouse::button_down(MouseButton button)
    {
        return button_down_table_[button];
    }
    
}