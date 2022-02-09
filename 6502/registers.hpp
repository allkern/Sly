#pragma once

#include "../global.hpp"

#define CF 0b00000001
#define ZF 0b00000010
#define IF 0b00000100
#define DF 0b00001000
#define BF 0b00110000
#define VF 0b01000000
#define NF 0b10000000

namespace nes {
    namespace cpu {
        namespace registers {
            u8 a, x, y, p, sp;

            u16 pc;
        }

        u16 operand = 0x0, opcode = 0x0;

        void set_flags(u8 mask, bool cond, bool reset = true) {
            using namespace registers;

            if (cond) {
                p |= mask;
                return;
            }

            if (reset) p &= ~mask;
        }
    }
}