#pragma once
#ifndef __SHT_SYSTEM_MOUSE_H__
#define __SHT_SYSTEM_MOUSE_H__

#include "../../common/table.h"

namespace sht {
    
    enum class MouseButton {
        kUnknown,
        
        kLeft,
        kRight,
        kMiddle,
        
        kCount
    };
    
    class Mouse {
    public:
        Mouse();
        ~Mouse();
        
        float& x();
        float& y();
        float& delta_x();
        float& delta_y();
        bool& enabled();
        bool& center();
        bool& button_down(MouseButton button);
        
    private:
        float x_;
        float y_;
        float delta_x_;
        float delta_y_;
        bool enabled_;
        bool center_;
        EnumTable<MouseButton, bool> button_down_table_;
    };
    
} // namespace sht

#endif