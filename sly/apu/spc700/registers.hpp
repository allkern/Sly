#pragma once

#include "../../global.hpp"

#define NF 0b10000000
#define VF 0b01000000
#define PF 0b00100000
#define BF 0b00010000
#define HF 0b00001000
#define IF 0b00000100
#define ZF 0b00000010
#define CF 0b00000001

namespace snes {
    namespace apu {
        namespace spc {
            namespace registers {
                u8 a, x, y, psw = 0x2, sp;
                u16 pc = 0xffc0;

                struct pair_t {
                    u8* h, *l;

                    operator u16() { return (*h << 8) | *l; }
                    u16 operator+(u16 rhs) { return ((u16)*this) + rhs; }
                    u16 operator-(u16 rhs) { return ((u16)*this) - rhs; }
                    u16 operator*(u16 rhs) { return ((u16)*this) * rhs; }
                    u16 operator/(u16 rhs) { return ((u16)*this) / rhs; }
                    pair_t& operator=(u16 rhs) { *l = rhs & 0xff; *h = (rhs >> 8) & 0xff; return *this; }
                } ya { &y, &a };
            }

            inline bool test_flag(u8 mask) { return registers::psw & mask; }
            inline u16 get_direct_page() { return test_flag(PF) << 8; }

            int last_cycles = 2, additional_cycles = 0;

            inline static void set_flags(u8 mask, bool cond, bool reset = true) {
                using namespace registers;

                if (cond) {
                    psw |= mask;
                    return;
                }

                if (reset) psw &= ~mask;
            }
        }
    }
}