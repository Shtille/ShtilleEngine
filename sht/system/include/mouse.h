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
        
        int& x();
        int& y();
        bool& enabled();
        bool& button_down(MouseButton button);
        
    private:
        int x_, y_;
        bool enabled_;
        EnumTable<MouseButton, bool> button_down_table_;
    };
    
} // namespace sht

#endif