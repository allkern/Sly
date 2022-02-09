#pragma once

#include "registers.hpp"
#include "stack.hpp"

#include "../bus.hpp"

namespace nes {
    namespace cpu {
        using namespace registers;

        struct signal_t {
            bool prev, value;
        };

        signal_t irq, nmi, brk, reset;

        size_t cycles_elapsed, last_cycles;

        void handle_reset() {
            pc = (bus::read(0xfffd) << 8) | bus::read(0xfffc);

            a = 0;
            x = 0;
            y = 0;
            p = 0x04;
            sp = 0xfd;

            opcode = 0x0;
            operand = 0x0;

            last_cycles = 8;
            cycles_elapsed += last_cycles;

            reset.value = true;
        }

        void handle_nmi() {
            push(pc);

            set_flags(0b00010000, false);
            set_flags(0b00100000 | IF, true);

            push(p);

            // Read new program counter location from fixed address
            pc = (bus::read(0xfffb) << 8) | bus::read(0xfffa);

            //_log(debug, "nmi pc=%04x", pc);

            // IRQs take time
            last_cycles = 8;
            cycles_elapsed += last_cycles;
        }

        void handle_irq() {
            // Push the program counter to the stack. It's 16-bits dont
            // forget so that takes two pushes
            push(pc);

            // Then Push the status register to the stack
            set_flags(0b00010000, false);
            set_flags(0b00100000 | IF, true);

            push(p);

            // Read new program counter location from fixed address
            pc = (bus::read(0xffff) << 8) | bus::read(0xfffe);

            // IRQs take time
            last_cycles = 7;
            cycles_elapsed += last_cycles;
        }
    
        void handle_interrupts() {
            if (nmi.prev == true && nmi.value == false) {
                handle_nmi();
            }

            if (irq.prev == false && irq.value == false && (!(p & IF))) {
                handle_irq();
            }

            // Inverted Level-triggered reset?
            if (reset.prev == false && reset.value == false) {
                handle_reset();
            }
        }
    }
}