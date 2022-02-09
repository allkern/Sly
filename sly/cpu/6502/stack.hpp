#pragma once

#include "bus.hpp"
#include "registers.hpp"

namespace nes {
    namespace cpu {
        using namespace registers;

        inline void push(u8 a) { bus::write(0x100 + sp--, a); }
        inline void push(u16 a) { push((u8)((a >> 8) & 0xff)); push((u8)(a & 0xff)); }
        inline u8 pop1() { return bus::read(0x100 + (++sp)); }
        inline u16 pop() { u16 v = pop1(); v |= pop1() << 8; return v; }
        inline void pop(u16& t) { t = pop1(); t |= pop1() << 8; }
    }
}