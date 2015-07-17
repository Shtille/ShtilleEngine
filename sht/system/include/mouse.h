#pragma once
#ifndef __SHT_SYSTEM_MOUSE_H__
#define __SHT_SYSTEM_MOUSE_H__

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
        
    private:
        int x_, y_;
        bool enabled_;
        bool *button_down;
    };
    
} // namespace sht

#endif