#include "../include/keys.h"
#include "../../common/platform.h"
#include <cstring>

namespace sht {

    Keys::Keys()
    : modifiers_(0)
    {
        memset(table_, 0, sizeof(table_));
        const int count = static_cast<int>(PublicKey::kCount);
        key_down_ = new bool[count];
        memset(key_down_, 0, sizeof(bool)*count);
        key_active_ = new bool[count];
        memset(key_active_, 0, sizeof(bool)*count);
        Fill();
    }
    Keys::~Keys()
    {
        delete [] key_down_;
        delete [] key_active_;
    }
    void Keys::Fill()
    {
        memset(table_, (int)PublicKey::kUnknown, sizeof(table_));
#if defined(TARGET_WINDOWS)
		table_[0x00B] = PublicKey::k0;
		table_[0x002] = PublicKey::k1;
		table_[0x003] = PublicKey::k2;
		table_[0x004] = PublicKey::k3;
		table_[0x005] = PublicKey::k4;
		table_[0x006] = PublicKey::k5;
		table_[0x007] = PublicKey::k6;
		table_[0x008] = PublicKey::k7;
		table_[0x009] = PublicKey::k8;
		table_[0x00A] = PublicKey::k9;
		table_[0x01E] = PublicKey::kA;
		table_[0x030] = PublicKey::kB;
		table_[0x02E] = PublicKey::kC;
		table_[0x020] = PublicKey::kD;
		table_[0x012] = PublicKey::kE;
		table_[0x021] = PublicKey::kF;
		table_[0x022] = PublicKey::kG;
		table_[0x023] = PublicKey::kH;
		table_[0x017] = PublicKey::kI;
		table_[0x024] = PublicKey::kJ;
		table_[0x025] = PublicKey::kK;
		table_[0x026] = PublicKey::kL;
		table_[0x032] = PublicKey::kM;
		table_[0x031] = PublicKey::kN;
		table_[0x018] = PublicKey::kO;
		table_[0x019] = PublicKey::kP;
		table_[0x010] = PublicKey::kQ;
		table_[0x013] = PublicKey::kR;
		table_[0x01F] = PublicKey::kS;
		table_[0x014] = PublicKey::kT;
		table_[0x016] = PublicKey::kU;
		table_[0x02F] = PublicKey::kV;
		table_[0x011] = PublicKey::kW;
		table_[0x02D] = PublicKey::kX;
		table_[0x015] = PublicKey::kY;
		table_[0x02C] = PublicKey::kZ;

		table_[0x028] = PublicKey::kApostrophe;
		table_[0x02B] = PublicKey::kBackslash;
		table_[0x033] = PublicKey::kComma;
		table_[0x00D] = PublicKey::kEqual;
		table_[0x029] = PublicKey::kGraveAccent;
		table_[0x01A] = PublicKey::kLeftBracket;
		table_[0x00C] = PublicKey::kMinus;
		table_[0x034] = PublicKey::kPeriod;
		table_[0x01B] = PublicKey::kRightBracket;
		table_[0x027] = PublicKey::kSemicolon;
		table_[0x035] = PublicKey::kSlash;
		table_[0x056] = PublicKey::kWorld2;

		table_[0x00E] = PublicKey::kBackspace;
		table_[0x153] = PublicKey::kDelete;
		table_[0x14F] = PublicKey::kEnd;
		table_[0x01C] = PublicKey::kEnter;
		table_[0x001] = PublicKey::kEscape;
		table_[0x147] = PublicKey::kHome;
		table_[0x152] = PublicKey::kInsert;
		table_[0x15D] = PublicKey::kMenu;
		table_[0x151] = PublicKey::kPageDown;
		table_[0x149] = PublicKey::kPageUp;
		table_[0x045] = PublicKey::kPause;
		table_[0x039] = PublicKey::kSpace;
		table_[0x00F] = PublicKey::kTab;
		table_[0x03A] = PublicKey::kCapsLock;
		table_[0x145] = PublicKey::kNumLock;
		table_[0x046] = PublicKey::kScrollLock;
		table_[0x03B] = PublicKey::kF1;
		table_[0x03C] = PublicKey::kF2;
		table_[0x03D] = PublicKey::kF3;
		table_[0x03E] = PublicKey::kF4;
		table_[0x03F] = PublicKey::kF5;
		table_[0x040] = PublicKey::kF6;
		table_[0x041] = PublicKey::kF7;
		table_[0x042] = PublicKey::kF8;
		table_[0x043] = PublicKey::kF9;
		table_[0x044] = PublicKey::kF10;
		table_[0x057] = PublicKey::kF11;
		table_[0x058] = PublicKey::kF12;
		table_[0x064] = PublicKey::kF13;
		table_[0x065] = PublicKey::kF14;
		table_[0x066] = PublicKey::kF15;
		table_[0x067] = PublicKey::kF16;
		table_[0x068] = PublicKey::kF17;
		table_[0x069] = PublicKey::kF18;
		table_[0x06A] = PublicKey::kF19;
		table_[0x06B] = PublicKey::kF20;
	//     table_[0x06C] = PublicKey::kF21;
	//     table_[0x06D] = PublicKey::kF22;
	//     table_[0x06E] = PublicKey::kF23;
	//     table_[0x076] = PublicKey::kF24;
		table_[0x038] = PublicKey::kLeftAlt;
		table_[0x01D] = PublicKey::kLeftControl;
		table_[0x02A] = PublicKey::kLeftShift;
		table_[0x15B] = PublicKey::kLeftSuper;
		table_[0x137] = PublicKey::kPrintScreen;
		table_[0x138] = PublicKey::kRightAlt;
		table_[0x11D] = PublicKey::kRightControl;
		table_[0x036] = PublicKey::kRightShift;
		table_[0x15C] = PublicKey::kRightSuper;
		table_[0x150] = PublicKey::kDown;
		table_[0x14B] = PublicKey::kLeft;
		table_[0x14D] = PublicKey::kRight;
		table_[0x148] = PublicKey::kUp;

		table_[0x052] = PublicKey::kKp0;
		table_[0x04F] = PublicKey::kKp1;
		table_[0x050] = PublicKey::kKp2;
		table_[0x051] = PublicKey::kKp3;
		table_[0x04B] = PublicKey::kKp4;
		table_[0x04C] = PublicKey::kKp5;
		table_[0x04D] = PublicKey::kKp6;
		table_[0x047] = PublicKey::kKp7;
		table_[0x048] = PublicKey::kKp8;
		table_[0x049] = PublicKey::kKp9;
		table_[0x04E] = PublicKey::kKpAdd;
		table_[0x053] = PublicKey::kKpDecimal;
		table_[0x135] = PublicKey::kKpDivide;
		table_[0x11C] = PublicKey::kKpEnter;
		table_[0x037] = PublicKey::kKpMultiply;
		table_[0x04A] = PublicKey::kKpSubstract;
#elif defined(TARGET_MAC)
        table_[0x1D] = PublicKey::k0;
        table_[0x12] = PublicKey::k1;
        table_[0x13] = PublicKey::k2;
        table_[0x14] = PublicKey::k3;
        table_[0x15] = PublicKey::k4;
        table_[0x17] = PublicKey::k5;
        table_[0x16] = PublicKey::k6;
        table_[0x1A] = PublicKey::k7;
        table_[0x1C] = PublicKey::k8;
        table_[0x19] = PublicKey::k9;
        table_[0x00] = PublicKey::kA;
        table_[0x0B] = PublicKey::kB;
        table_[0x08] = PublicKey::kC;
        table_[0x02] = PublicKey::kD;
        table_[0x0E] = PublicKey::kE;
        table_[0x03] = PublicKey::kF;
        table_[0x05] = PublicKey::kG;
        table_[0x04] = PublicKey::kH;
        table_[0x22] = PublicKey::kI;
        table_[0x26] = PublicKey::kJ;
        table_[0x28] = PublicKey::kK;
        table_[0x25] = PublicKey::kL;
        table_[0x2E] = PublicKey::kM;
        table_[0x2D] = PublicKey::kN;
        table_[0x1F] = PublicKey::kO;
        table_[0x23] = PublicKey::kP;
        table_[0x0C] = PublicKey::kQ;
        table_[0x0F] = PublicKey::kR;
        table_[0x01] = PublicKey::kS;
        table_[0x11] = PublicKey::kT;
        table_[0x20] = PublicKey::kU;
        table_[0x09] = PublicKey::kV;
        table_[0x0D] = PublicKey::kW;
        table_[0x07] = PublicKey::kX;
        table_[0x10] = PublicKey::kY;
        table_[0x06] = PublicKey::kZ;
        
        table_[0x27] = PublicKey::kApostrophe;
        table_[0x2A] = PublicKey::kBackslash;
        table_[0x2B] = PublicKey::kComma;
        table_[0x18] = PublicKey::kEqual;
        table_[0x32] = PublicKey::kGraveAccent;
        table_[0x21] = PublicKey::kLeftBracket;
        table_[0x1B] = PublicKey::kMinus;
        table_[0x2F] = PublicKey::kPeriod;
        table_[0x1E] = PublicKey::kRightBracket;
        table_[0x29] = PublicKey::kSemicolon;
        table_[0x2C] = PublicKey::kSlash;
        table_[0x0A] = PublicKey::kWorld1;
        
        table_[0x33] = PublicKey::kBackspace;
        table_[0x39] = PublicKey::kCapsLock;
        table_[0x75] = PublicKey::kDelete;
        table_[0x7D] = PublicKey::kDown;
        table_[0x77] = PublicKey::kEnd;
        table_[0x24] = PublicKey::kEnter;
        table_[0x35] = PublicKey::kEscape;
        table_[0x7A] = PublicKey::kF1;
        table_[0x78] = PublicKey::kF2;
        table_[0x63] = PublicKey::kF3;
        table_[0x76] = PublicKey::kF4;
        table_[0x60] = PublicKey::kF5;
        table_[0x61] = PublicKey::kF6;
        table_[0x62] = PublicKey::kF7;
        table_[0x64] = PublicKey::kF8;
        table_[0x65] = PublicKey::kF9;
        table_[0x6D] = PublicKey::kF10;
        table_[0x67] = PublicKey::kF11;
        table_[0x6F] = PublicKey::kF12;
        table_[0x69] = PublicKey::kF13;
        table_[0x6B] = PublicKey::kF14;
        table_[0x71] = PublicKey::kF15;
        table_[0x6A] = PublicKey::kF16;
        table_[0x40] = PublicKey::kF17;
        table_[0x4F] = PublicKey::kF18;
        table_[0x50] = PublicKey::kF19;
        table_[0x5A] = PublicKey::kF20;
        table_[0x73] = PublicKey::kHome;
        table_[0x72] = PublicKey::kInsert;
        table_[0x7B] = PublicKey::kLeft;
        table_[0x3A] = PublicKey::kLeftAlt;
        table_[0x3B] = PublicKey::kLeftControl;
        table_[0x38] = PublicKey::kLeftShift;
        table_[0x37] = PublicKey::kLeftSuper;
        table_[0x6E] = PublicKey::kMenu;
        table_[0x47] = PublicKey::kNumLock;
        table_[0x79] = PublicKey::kPageDown;
        table_[0x74] = PublicKey::kPageUp;
        table_[0x7C] = PublicKey::kRight;
        table_[0x3D] = PublicKey::kRightAlt;
        table_[0x3E] = PublicKey::kRightControl;
        table_[0x3C] = PublicKey::kRightShift;
        table_[0x36] = PublicKey::kRightSuper;
        table_[0x31] = PublicKey::kSpace;
        table_[0x30] = PublicKey::kTab;
        table_[0x7E] = PublicKey::kUp;
        
        table_[0x52] = PublicKey::kKp0;
        table_[0x53] = PublicKey::kKp1;
        table_[0x54] = PublicKey::kKp2;
        table_[0x55] = PublicKey::kKp3;
        table_[0x56] = PublicKey::kKp4;
        table_[0x57] = PublicKey::kKp5;
        table_[0x58] = PublicKey::kKp6;
        table_[0x59] = PublicKey::kKp7;
        table_[0x5B] = PublicKey::kKp8;
        table_[0x5C] = PublicKey::kKp9;
        table_[0x45] = PublicKey::kKpAdd;
        table_[0x41] = PublicKey::kKpDecimal;
        table_[0x4B] = PublicKey::kKpDivide;
        table_[0x4C] = PublicKey::kKpEnter;
        table_[0x51] = PublicKey::kKpEqual;
        table_[0x43] = PublicKey::kKpMultiply;
        table_[0x4E] = PublicKey::kKpSubstract;
#else
        static_assert(false, "PublicKey::Fill not implemented");
#endif
    }
    PublicKey Keys::table(unsigned short key)
    {
        if (key >= sizeof(table_)/sizeof(table_[0]))
            return PublicKey::kUnknown;
        return table_[key];
    }
    bool& Keys::key_down(PublicKey key)
    {
        return key_down_[static_cast<int>(key)];
    }
    bool& Keys::key_active(PublicKey key)
    {
        return key_active_[static_cast<int>(key)];
    }
    int& Keys::modifiers()
    {
        return modifiers_;
    }
	bool Keys::IsGoodChar(unsigned short code)
	{
		return (code >= 32 && code <= 126) || code >= 160;
	}

} // namespace sht