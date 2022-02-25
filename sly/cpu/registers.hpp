#pragma once

#include "../global.hpp"

namespace snes {
    namespace cpu {
#define CF 0b00000001
#define ZF 0b00000010
#define IF 0b00000100
#define DF 0b00001000
#define XF 0b00010000
#define MF 0b00100000
#define VF 0b01000000
#define NF 0b10000000

        namespace registers {
            // Working registers
            u16 a, x, y;

            // Stack & Direct Page
            u16 sp, d;

            // Data bank
            u8 db;

            // Program counter (pbr inside™)
            u32 pc;

            // Status (flags)
            u8 p;
        }


        inline bool test_flag(u8 mask) {
            return registers::p & mask;
        }
    
        void set_a(u16 value) { registers::a &= test_flag(MF) ? 0xff00 : 0x0000; registers::a |= value & (test_flag(MF) ? 0xff : 0xffff); }
        void set_x(u16 value) { registers::x &= test_flag(XF) ? 0xff00 : 0x0000; registers::x |= value & (test_flag(XF) ? 0xff : 0xffff); }
        void set_y(u16 value) { registers::y &= test_flag(XF) ? 0xff00 : 0x0000; registers::y |= value & (test_flag(XF) ? 0xff : 0xffff); }
        u16 get_a() { return registers::a & (test_flag(MF) ? 0xff : 0xffff); }
        u16 get_x() { return registers::x & (test_flag(XF) ? 0xff : 0xffff); }
        u16 get_y() { return registers::y & (test_flag(XF) ? 0xff : 0xffff); }

        bool get_sign_a() { return registers::a & (1 << (test_flag(MF) ? 7 : 15)); }
        bool get_zero_a() { return !(registers::a & (test_flag(MF) ? 0xff : 0xffff)); }
        bool get_sign_x() { return registers::x & (1 << (test_flag(XF) ? 7 : 15)); }
        bool get_zero_x() { return !(registers::x & (test_flag(XF) ? 0xff : 0xffff)); }
        bool get_sign_y() { return registers::y & (1 << (test_flag(XF) ? 7 : 15)); }
        bool get_zero_y() { return !(registers::y & (test_flag(XF) ? 0xff : 0xffff)); }
        bool get_sign(u16 value, bool mode) { return value & (1 << (mode ? 7 : 15)); }
        bool get_zero(u16 value, bool mode) { return !(value & (mode ? 0xff : 0xffff)); }

        inline static void set_flags(u8 mask, bool cond, bool reset = true) {
            using namespace registers;

            if (cond) {
                p |= mask;
                return;
            }

            if (reset) p &= ~mask;
        }

        int additional_cycles = 0, last_cycles = 0;

        bool waiting = false;

        u16 enmiv,
            eirqv,
            ebrkv,
            ecopv,
            nnmiv,
            nirqv,
            nbrkv,
            ncopv,
            nmiv,
            irqv,
            brkv,
            copv;
    }
}