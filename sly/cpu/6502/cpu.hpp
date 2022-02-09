#pragma once

#include "opcode_table.hpp"
#include "interrupts.hpp"
#include "registers.hpp"
#include "bus.hpp"

#include "../global.hpp"
#include "../log.hpp"

namespace nes {
    namespace cpu {
        using namespace registers;

        void init() {
            p = 0x24;
            sp = 0xfd;

            irq.value   = true; irq.prev   = irq.value;
            nmi.value   = true; nmi.prev   = nmi.value;
            reset.value = true; reset.prev = reset.value;
        }

        void fetch() {
            opcode = bus::read(pc++);
        }

        void execute() {
            const opcode_t& decoded = opcode_map[opcode];

            decoded.addressing_mode();
            decoded.instruction();

            last_cycles = decoded.t;
            cycles_elapsed += last_cycles;
        }

        void cycle() {
            fetch();
            execute();
            //handle_interrupts();

            page_crossed = false;
        }
    }
}