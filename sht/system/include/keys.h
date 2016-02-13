#pragma once
#ifndef __SHT_SYSTEM_KEYS_H__
#define __SHT_SYSTEM_KEYS_H__

#include "../../common/platform.h"

namespace sht {

    struct ModifierKey {
        static const int kShift     = 0x01;
        static const int kControl   = 0x02;
        static const int kAlt       = 0x04;
        static const int kSuper     = 0x08;
    };
	
    enum class PublicKey : int {
        kUnknown,
        
        k0,
        k1,
        k2,
        k3,
        k4,
        k5,
        k6,
        k7,
        k8,
        k9,
        
        kA,
        kB,
        kC,
        kD,
        kE,
        kF,
        kG,
        kH,
        kI,
        kJ,
        kK,
        kL,
        kM,
        kN,
        kO,
        kP,
        kQ,
        kR,
        kS,
        kT,
        kU,
        kV,
        kW,
        kX,
        kY,
        kZ,
        
        kF1,
        kF2,
        kF3,
        kF4,
        kF5,
        kF6,
        kF7,
        kF8,
        kF9,
        kF10,
        kF11,
        kF12,
        kF13,
        kF14,
        kF15,
        kF16,
        kF17,
        kF18,
        kF19,
        kF20,
        
        kApostrophe,
        kSlash,
        kBackslash,
        kComma,
        kEqual,
        kMinus,
        kPeriod,
        kSemicolon,
        kLeftBracket,
        kRightBracket,
        kGraveAccent,
        kWorld1,
		kWorld2,
        
        kBackspace,
        kCapsLock,
        kInsert,
        kDelete,
        kHome,
        kEnd,
        kEnter,
        kEscape,
        kMenu,
        kNumLock,
		kScrollLock,
		kPause,
        kPageUp,
        kPageDown,
        kSpace,
        kTab,
        
        kUp,
        kDown,
        kLeft,
        kRight,
        
        kLeftAlt,
        kLeftControl,
        kLeftShift,
        kLeftSuper,
        kRightAlt,
        kRightControl,
        kRightShift,
        kRightSuper,
        
        kKp0,
        kKp1,
        kKp2,
        kKp3,
        kKp4,
        kKp5,
        kKp6,
        kKp7,
        kKp8,
        kKp9,
        kKpAdd,
        kKpSubstract,
        kKpMultiply,
        kKpDivide,
        kKpDecimal,
        kKpEqual,
        kKpEnter,
        kPrintScreen,
        
        kCount
    };
    
    class Keys {
    public:
        Keys();
        ~Keys();
        
        PublicKey table(unsigned short key);
        bool& key_down(PublicKey key);
        bool& key_active(PublicKey key);
        int& modifiers();

		static bool IsGoodChar(unsigned short code);
        
    private:
        void Fill();
#if defined(TARGET_WINDOWS)
        PublicKey table_[512];
#else
		PublicKey table_[256];
#endif
        bool *key_down_;
        bool *key_active_;
        int modifiers_;
    };

} // namespace sht

#endif