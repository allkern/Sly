#pragma once

#include "global.hpp"

#define CTRL_A      0b0000000010000000
#define CTRL_B      0b1000000000000000
#define CTRL_X      0b0000000001000000
#define CTRL_Y      0b0100000000000000
#define CTRL_L      0b0000000000100000
#define CTRL_R      0b0000000000010000
#define CTRL_SELECT 0b0010000000000000
#define CTRL_START  0b0001000000000000
#define CTRL_UP     0b0000100000000000
#define CTRL_DOWN   0b0000010000000000
#define CTRL_LEFT   0b0000001000000000
#define CTRL_RIGHT  0b0000000100000000

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