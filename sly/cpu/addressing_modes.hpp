#pragma once

#include "../global.hpp"
#include "../bus.hpp"
#include "registers.hpp"

namespace snes {
    namespace cpu {
        using namespace registers;

        bool immediate_mode = false;

        u32 address = 0x0;
        u32 data = 0x0;
        u32 src, dst;

        /* The opcode (always 1 byte) is parsed
           and the PC is at the next byte when the addressing
           mode is executed.

           The addressing mode should also increment the PC
           by the amount of bytes it uses.

           In this logic, address = pc++ sets the address to be
           at the start of the operand

                |
                v ->
            [ii][oo]...
        */

        void imp() {}
        void acc() {}

        // 1/2 Byte Immediates
        void imm() { immediate_mode = true; address = pc++; /*/*data = bus::read(address, true);*/ }
        void ima() { immediate_mode = true; address = pc++; if (!test_flag(MF)) pc++; /*/*data = bus::read(address, test_flag(MF));*/ }
        void imx() { immediate_mode = true; address = pc++; if (!test_flag(XF)) pc++; /*/*data = bus::read(address, test_flag(XF));*/ }
        
        // Relative
        void rel() { address = pc + (i32)(i8)bus::read(pc++, true); }
        void rll() { address = pc + (i32)(i16)bus::read(pc++, false) + 1; pc++; }

        // Absolute
        void abj() { address = (pc & 0xff0000) | bus::read(pc, false); pc += 2; }
        void abd() { address = (db << 16) | bus::read(pc, false); pc += 2; }
        void abx() {
            address = (db << 16) | (bus::read(pc, false) + x); pc += 2;
            //_log(debug, "address=%06x", address);
        }
        void aby() {
            address = (db << 16) | (bus::read(pc, false) + y); pc += 2;
        }
        
        // Absolute Indirect
        void abi() { address = (pc & 0xff0000) | bus::read(pc, false); address = (pc & 0xff0000) | bus::read(address, false); pc += 2; }
        void aix() { address = (pc & 0xff0000) | bus::read(pc, false) + x; address = (pc & 0xff0000) | bus::read(address, false); pc += 2; }
        void ail() { address = (pc & 0xff0000) | bus::read(pc, false); address = bus::read24(address); pc += 2; }
        
        // Direct Page
        void dpg() { address = (d + bus::read8(pc++)) & 0xffff; if (d & 0xff) additional_cycles += 1; }
        void dpx() { address = (d + bus::read8(pc++) + x) & 0xffff; /*data = bus::read(address, false, true);*/ if (d & 0xff) additional_cycles += 1; }
        void dpy() { address = (d + bus::read8(pc++) + y) & 0xffff; /*data = bus::read(address, false, true);*/ if (d & 0xff) additional_cycles += 1; }

        // Direct Page Indirect
        void dpi() { address = (d + bus::read8(pc++)) & 0xffff; address = (db << 16) | bus::read(address, false); /*data = bus::read(address, false, true);*/ if (d & 0xff) additional_cycles += 1; }
        void dix() { address = (d + bus::read8(pc++) + x) & 0xffff; address = (db << 16) | bus::read(address, false); /*data = bus::read(address, false, true);*/ if (d & 0xff) additional_cycles += 1; }
        void diy() {
            address = (d + bus::read8(pc++)) & 0xffff;
            address = (db << 16) | bus::read(address, false) + y;
            if (d & 0xff) additional_cycles += 1;
        }
        void dil() { address = (d + bus::read8(pc++)) & 0xffff; address = bus::read24(address); if (d & 0xff) additional_cycles += 1; }
        void dly() {
            address = (d + bus::read8(pc++)) & 0xffff;
            address = bus::read24(address) + y;
            u32 old_address = address - y;
            u8 value = bus::read8(address);
            if (d == 0x100) _log(debug, "dly imm=%02x, d=%04x, y=%04x, address-y=%06x, address=%06x, value=%02x", bus::read8(pc - 1), d, y, old_address, address, value);
            //address -= 0x10;
            if (d & 0xff) additional_cycles += 1;
        }

        // Long
        void lng() { address = bus::read24(pc); pc += 3; /*data = bus::read(address, false);*/ }
        void lnx() { address = bus::read24(pc) + x; pc += 3; }
        
        // Stack Relative
        void str() { address = sp + (i16)bus::read8(pc++); /*data = bus::read(address, false, true);*/ }
        void sri() { address = sp + (i16)bus::read8(pc++); address = (db << 16) | bus::read(address, false); /*data = bus::read(address, false);*/ }

        // Block Move
        void bmm() { data = bus::read(pc, false); pc += 2; src = ((data & 0xff00) << 8); dst = ((data & 0xff) << 16); }
    }
}