#pragma once

#include "../global.hpp"
#include "../bus.hpp"
#include "registers.hpp"

namespace snes {
    namespace cpu {
        using namespace registers;

        u32 operand = 0x0;
        
        // Implied
        void imp() {
            return;
        }

        // Absolute
        void abs() {
            operand = (db << 16) | bus::read16(pc);

            pc += 2;
        }

        // Absolute Indirect
        void abi() {
            u32 addr = (db << 16) | bus::read16(pc);
            operand = bus::read16(addr);

            pc += 2;
        }

        // Absolute Indirect Indexed with X
        void aix() {
            u32 addr = (db << 16) | bus::read16(pc);
            operand = bus::read16(addr + x);

            pc += 2;
        }

        // Absolute Indirect Indexed with Y
        void aiy() {
            u32 addr = (db << 16) | bus::read16(pc);
            operand = bus::read16(addr + y);

            pc += 2;
        }

        // Absolute Long Indexed with X
        void alx() {
            u32 addr = bus::read24(pc);
            operand = bus::read8(addr + x);

            pc += 3;
        }

        // Absolute Long
        void abl() {
            operand = pc;

            pc += 3;
        }

        // Relative
        void rel() {
            operand = (i8)bus::read8(pc++);
        }

        // Relative Long
        void rll() {
            operand = (i16)bus::read16(pc);

            pc += 2;
        }

        // Immediate
        void imm() {
            operand = pc;

            pc += test_flag(MF) ? 2 : 1;
        }

        // Direct Indirect
        void dri() {
            u32 addr = bus::read8(pc++);         // Read immediate
            u16 indirect = bus::read16(addr);   // Read 16-bit address at immediate
            operand = bus::read8(indirect);      // Operand is read from the indirect address
        }

        // Direct Indirect Indexed with X
        void dix() {
            u32 addr = bus::read8(pc++);             // Read immediate
            u16 indirect = bus::read16(addr + x);   // Read 16-bit address at immediate + X
            operand = bus::read8(indirect);          // Operand is read from the indirect address
        }

        // Direct Indirect Indexed with Y
        void diy() {
            u32 addr = bus::read8(pc++);             // Read immediate
            u16 indirect = bus::read16(addr);       // Read 16-bit address at immediate
            operand = bus::read8(indirect + y);      // Operand is read from the indirect address + Y
        }

        // Direct Indirect Long
        void dli() {
            u32 addr = bus::read8(pc++);             // Read immediate
            u32 indirect = bus::read24(addr);       // Read 24-bit address at immediate
            operand = bus::read8(indirect);
        }

        // Direct Indirect Long Indexed with Y
        void dly() {
            u32 addr = bus::read8(pc++);             // Read immediate
            u32 indirect = bus::read24(addr);       // Read 24-bit address at immediate
            operand = bus::read8(indirect + y);
        }

        // Direct Indexed with X
        void drx() {
            u16 addr = bus::read8(pc++) + x;
            operand = bus::read8(addr);
        }

        // Direct Indexed with Y
        void dry() {
            u16 addr = bus::read8(pc++) + y;
            operand = bus::read8(addr);
        }

        // Stack Relative
        void str() {
            u8 relative = bus::read8(pc++);
            operand = bus::read8(sp + relative);
        }

        // Stack Relative Indexed with Y
        void sry() {
            u8 relative = bus::read8(pc++);
            u16 indirect = bus::read16(sp + relative);
            operand = bus::read8(indirect + y);
        }
    }
}