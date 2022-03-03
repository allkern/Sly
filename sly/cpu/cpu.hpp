#pragma once

#include "instructions.hpp"
#include "registers.hpp"
#include "addressing_modes.hpp"
#include "opcode_table.hpp"
#include "../bus.hpp"

namespace snes {
    namespace cpu {
        u8 opcode = 0x0;
        u32 full_opcode = 0x0;
        u32 base_pc = 0x0;

        void init() {
            enmiv = bus::read16(0xfffa);
            ebrkv = bus::read16(0xfff6);
            ecopv = bus::read16(0xfff4);
            nnmiv = bus::read16(0xffea);
            nbrkv = bus::read16(0xffe6);
            ncopv = bus::read16(0xffe4);

            stopped = false;
            waiting = false;

            nmiv = nnmiv;
            brkv = nbrkv;
            copv = ncopv;

            // _log(debug, "enmiv=%04x", enmiv);
            // _log(debug, "ebrkv=%04x", ebrkv);
            // _log(debug, "ecopv=%04x", ecopv);
            // _log(debug, "nnmiv=%04x", nnmiv);
            // _log(debug, "nbrkv=%04x", nbrkv);
            // _log(debug, "ncopv=%04x", ncopv);
            pc = bus::read16(0xfffc);
            // _log(debug, "resetv=%04x", pc);
            sp = 0x1ff;
            p = 0x34;
        }

        opcode_t instruction;

        void fetch() {
            if (waiting || stopped) return;

            base_pc = registers::pc;
            opcode = bus::read8(registers::pc++);
            full_opcode = bus::read24(registers::pc) << 8;
            full_opcode |= opcode;
        
            instruction = map[opcode];
        }

        void execute() {
            if (waiting || stopped) { last_cycles = 2; return; }

            instruction.addressing_mode();
            instruction.operation();

            immediate_mode = false;

            last_cycles = instruction.cycles + additional_cycles;
            total_cycles += last_cycles;

            additional_cycles = 0;
        }
    }
}