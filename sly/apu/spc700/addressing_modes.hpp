#pragma once

#include "../../global.hpp"

#include "registers.hpp"
#include "../bus.hpp"

namespace snes {
    namespace apu {
        namespace spc {
            using namespace registers;

            u16 address = 0x0;

            u16 src, dst;

            void imp() {}
            void imm() { address = pc++; }
            void xin() { address = x; }
            void ixi() { address = x++; }
            void dpg() { address = get_direct_page() | bus::read(pc++); }
            void dpx() { address = get_direct_page() | bus::read(pc++) + x; }
            void dpy() { address = get_direct_page() | bus::read(pc++) + y; }
            void abs() { address = bus::read16(pc); pc += 2; }
            void abx() { address = bus::read16(pc) + x; pc += 2; }
            void aby() { address = bus::read16(pc) + y; pc += 2; }
            void idx() { address = bus::read16(get_direct_page() | bus::read(pc++) + x); }
            void idy() { address = bus::read16(get_direct_page() | bus::read(pc++)) + y; }
            void dp2() { u16 d = get_direct_page(); src = d | bus::read(pc++); dst = d | bus::read(pc++); }
            void dpi() { src = pc++; dst = get_direct_page() | bus::read(pc++); }
            void ixy() { dst = x; src = y; }
            void brl() { src = get_direct_page() | bus::read(pc++); address = pc + (i16)(i8)bus::read(pc); pc++; }
            void rel() { address = (i16)(i8)bus::read(pc++); }
        }
    }
}