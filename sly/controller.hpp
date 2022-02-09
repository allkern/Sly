#pragma once

#include "global.hpp"

#define CTRL_A      0b00000000'10000000
#define CTRL_B      0b10000000'00000000
#define CTRL_X      0b00000000'01000000
#define CTRL_Y      0b01000000'00000000
#define CTRL_L      0b00000000'00100000
#define CTRL_R      0b00000000'00010000
#define CTRL_SELECT 0b00100000'00000000
#define CTRL_START  0b00010000'00000000
#define CTRL_UP     0b00001000'00000000
#define CTRL_DOWN   0b00000100'00000000
#define CTRL_LEFT   0b00000010'00000000
#define CTRL_RIGHT  0b00000001'00000000

namespace snes {
    namespace controllers {
        u16 controller[4];

        void keydown(int mask, int ctrl) {
            controller[ctrl] |= mask;
        }

        void keyup(int mask, int ctrl) {
            controller[ctrl] &= ~mask;
        }

        u8 read(u16 addr) {
            int ctrl = (addr >> 1) & 0x3;
            bool high = addr & 0x1;

            return high ? ((controller[ctrl] >> 8) & 0xff) : (controller[ctrl] & 0xff);
        }
    }
}