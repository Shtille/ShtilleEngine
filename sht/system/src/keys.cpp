#include "../include/keys.h"
#include "../../common/platform.h"
#include <memory.h>

namespace sht {

    KeyTable::KeyTable()
    : key_queue_size_(0)
    {
        memset(table_, 0, sizeof(table_));
        const int count = static_cast<int>(PublicKeys::kCount);
        key_down_ = new bool[count];
        key_active_ = new bool[count];
        Fill();
    }
    KeyTable::~KeyTable()
    {
        delete [] key_down_;
        delete [] key_active_;
    }
void KeyTable::Fill()
{
#if defined(TARGET_MAC)
    table_[0x1D] = PublicKeys::k0;
    table_[0x12] = PublicKeys::k1;
    table_[0x13] = PublicKeys::k2;
    table_[0x14] = PublicKeys::k3;
    table_[0x15] = PublicKeys::k4;
    table_[0x17] = PublicKeys::k5;
    table_[0x16] = PublicKeys::k6;
    table_[0x1A] = PublicKeys::k7;
    table_[0x1C] = PublicKeys::k8;
    table_[0x19] = PublicKeys::k9;
    table_[0x00] = PublicKeys::kA;
    table_[0x0B] = PublicKeys::kB;
    table_[0x08] = PublicKeys::kC;
    table_[0x02] = PublicKeys::kD;
    table_[0x0E] = PublicKeys::kE;
    table_[0x03] = PublicKeys::kF;
    table_[0x05] = PublicKeys::kG;
    table_[0x04] = PublicKeys::kH;
    table_[0x22] = PublicKeys::kI;
    table_[0x26] = PublicKeys::kJ;
    table_[0x28] = PublicKeys::kK;
    table_[0x25] = PublicKeys::kL;
    table_[0x2E] = PublicKeys::kM;
    table_[0x2D] = PublicKeys::kN;
    table_[0x1F] = PublicKeys::kO;
    table_[0x23] = PublicKeys::kP;
    table_[0x0C] = PublicKeys::kQ;
    table_[0x0F] = PublicKeys::kR;
    table_[0x01] = PublicKeys::kS;
    table_[0x11] = PublicKeys::kT;
    table_[0x20] = PublicKeys::kU;
    table_[0x09] = PublicKeys::kV;
    table_[0x0D] = PublicKeys::kW;
    table_[0x07] = PublicKeys::kX;
    table_[0x10] = PublicKeys::kY;
    table_[0x06] = PublicKeys::kZ;
    
    table_[0x27] = PublicKeys::kApostrophe;
    table_[0x2A] = PublicKeys::kBackslash;
    table_[0x2B] = PublicKeys::kComma;
    table_[0x18] = PublicKeys::kEqual;
    table_[0x32] = PublicKeys::kGraveAccent;
    table_[0x21] = PublicKeys::kLeftBracket;
    table_[0x1B] = PublicKeys::kMinus;
    table_[0x2F] = PublicKeys::kPeriod;
    table_[0x1E] = PublicKeys::kRightBracket;
    table_[0x29] = PublicKeys::kSemicolon;
    table_[0x2C] = PublicKeys::kSlash;
    table_[0x0A] = PublicKeys::kWorld1;
    
    table_[0x33] = PublicKeys::kBackspace;
    table_[0x39] = PublicKeys::kCapsLock;
    table_[0x75] = PublicKeys::kDelete;
    table_[0x7D] = PublicKeys::kDown;
    table_[0x77] = PublicKeys::kEnd;
    table_[0x24] = PublicKeys::kEnter;
    table_[0x35] = PublicKeys::kEscape;
    table_[0x7A] = PublicKeys::kF1;
    table_[0x78] = PublicKeys::kF2;
    table_[0x63] = PublicKeys::kF3;
    table_[0x76] = PublicKeys::kF4;
    table_[0x60] = PublicKeys::kF5;
    table_[0x61] = PublicKeys::kF6;
    table_[0x62] = PublicKeys::kF7;
    table_[0x64] = PublicKeys::kF8;
    table_[0x65] = PublicKeys::kF9;
    table_[0x6D] = PublicKeys::kF10;
    table_[0x67] = PublicKeys::kF11;
    table_[0x6F] = PublicKeys::kF12;
    table_[0x69] = PublicKeys::kF13;
    table_[0x6B] = PublicKeys::kF14;
    table_[0x71] = PublicKeys::kF15;
    table_[0x6A] = PublicKeys::kF16;
    table_[0x40] = PublicKeys::kF17;
    table_[0x4F] = PublicKeys::kF18;
    table_[0x50] = PublicKeys::kF19;
    table_[0x5A] = PublicKeys::kF20;
    table_[0x73] = PublicKeys::kHome;
    table_[0x72] = PublicKeys::kInsert;
    table_[0x7B] = PublicKeys::kLeft;
    table_[0x3A] = PublicKeys::kLeftAlt;
    table_[0x3B] = PublicKeys::kLeftControl;
    table_[0x38] = PublicKeys::kLeftShift;
    table_[0x37] = PublicKeys::kLeftSuper;
    table_[0x6E] = PublicKeys::kMenu;
    table_[0x47] = PublicKeys::kNumLock;
    table_[0x79] = PublicKeys::kPageDown;
    table_[0x74] = PublicKeys::kPageUp;
    table_[0x7C] = PublicKeys::kRight;
    table_[0x3D] = PublicKeys::kRightAlt;
    table_[0x3E] = PublicKeys::kRightConrol;
    table_[0x3C] = PublicKeys::kRightShift;
    table_[0x36] = PublicKeys::kRightSuper;
    table_[0x31] = PublicKeys::kSpace;
    table_[0x30] = PublicKeys::kTab;
    table_[0x7E] = PublicKeys::kUp;
    
    table_[0x52] = PublicKeys::kKp0;
    table_[0x53] = PublicKeys::kKp1;
    table_[0x54] = PublicKeys::kKp2;
    table_[0x55] = PublicKeys::kKp3;
    table_[0x56] = PublicKeys::kKp4;
    table_[0x57] = PublicKeys::kKp5;
    table_[0x58] = PublicKeys::kKp6;
    table_[0x59] = PublicKeys::kKp7;
    table_[0x5B] = PublicKeys::kKp8;
    table_[0x5C] = PublicKeys::kKp9;
    table_[0x45] = PublicKeys::kKpAdd;
    table_[0x41] = PublicKeys::kKpDecimal;
    table_[0x4B] = PublicKeys::kKpDivide;
    table_[0x4C] = PublicKeys::kKpEnter;
    table_[0x51] = PublicKeys::kKpEqual;
    table_[0x43] = PublicKeys::kKpMultiply;
    table_[0x4E] = PublicKeys::kKpSubstract;
#else
    static_assert(false, "PublicKeys::Fill not implemented");
#endif
}
    PublicKeys KeyTable::table(unsigned short key)
    {
        if (key >= sizeof(table_)/sizeof(table_[0]))
            return PublicKeys::kUnknown;
        return table_[key];
    }
    bool& KeyTable::key_down(PublicKeys key)
    {
        return key_down_[static_cast<int>(key)];
    }
    bool& KeyTable::key_active(PublicKeys key)
    {
        return key_active_[static_cast<int>(key)];
    }

} // namespace sht