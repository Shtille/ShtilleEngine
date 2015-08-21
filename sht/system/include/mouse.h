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
    
    //! Standart mouse class
    /*
     Mouse position is in window coordinates with origin in left bottom corner.
    */
    class Mouse {
    public:
        Mouse();
        ~Mouse();
        
        float& x(); //!< x position of mouse in window coordinates
        float& y(); //!< y position of mouse in window coordinates
        float& delta_x();
        float& delta_y();
        bool& button_down(MouseButton button);
        
    private:
        float x_;
        float y_;
        float delta_x_;
        float delta_y_;
        EnumTable<MouseButton, bool> button_down_table_;
    };
    
} // namespace sht

#endif