#include "../include/keys.h"
#include "../../common/platform.h"
#include <memory.h>

namespace sht {

    KeyTable::KeyTable()
    : modifiers_(0)
    , key_queue_size_(0)
    {
        memset(table_, 0, sizeof(table_));
        const int count = static_cast<int>(PublicKey::kCount);
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
    table_[0x3E] = PublicKey::kRightConrol;
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
    PublicKey KeyTable::table(unsigned short key)
    {
        if (key >= sizeof(table_)/sizeof(table_[0]))
            return PublicKey::kUnknown;
        return table_[key];
    }
    bool& KeyTable::key_down(PublicKey key)
    {
        return key_down_[static_cast<int>(key)];
    }
    bool& KeyTable::key_active(PublicKey key)
    {
        return key_active_[static_cast<int>(key)];
    }
    int& KeyTable::modifiers()
    {
        return modifiers_;
    }

} // namespace sht