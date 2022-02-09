#pragma once

#include "../global.hpp"

#include "addressing_modes.hpp"
#include "opcode_table.hpp"
#include "interrupts.hpp"
#include "registers.hpp"
#include "stack.hpp"
#include "bus.hpp"

#include <iostream>

namespace nes {
    namespace cpu {
        typedef void (*instruction_t)();

        using namespace registers;

        inline void add_page_cross_cycles() {
            if (page_crossed && (opcode != 0xa0)) {
                cycles_elapsed++;
                last_cycles++;
            }

            page_crossed = false;
        }

        inline void check_branch_cycles() {
            if ((operand & 0xff00) == (pc & 0xff00)) {
                cycles_elapsed++;
                last_cycles++;
            } else {
                cycles_elapsed += 2;
                last_cycles += 2;
            }
        }

        void i_adc() {
            u8 i = bus::read(operand), pa = a;

            u16 r = a + i + (p & CF);

            set_flags(CF, r & 0xff00);

            r &= 0xff;

            set_flags(NF, r & 0x80);
            set_flags(ZF, r == 0);
            set_flags(VF, (a ^ r) & (i ^ r) & 0x80);

            a = r & 0xff;
        }

        void i_sbc() {
            u8 i = bus::read(operand) ^ 0xff;
            u16 r = a + i + (p & CF);

            set_flags(CF, r & 0xff00);

            r &= 0xff;

            set_flags(NF, r & 0x80);
            set_flags(ZF, r == 0);
            set_flags(VF, (a ^ r) & (i ^ r) & 0x80);

            a = r & 0xff;
        }

        // Unimplemented opcode trap
        void i_ill() { /*_log(warning, "NOP'ing unimplemented opcode 0x%02x", opcode);*/ }

        void i_and() { a &= bus::read(operand); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_bcc() { if (!(p & CF)) { check_branch_cycles(); pc = operand; } }
        void i_bcs() { if (p & CF) { check_branch_cycles(); pc = operand; } }
        void i_beq() { if (p & ZF) { check_branch_cycles(); pc = operand; } }
        void i_bit() { u8 b = bus::read(operand), r = b & a; set_flags(ZF, !r); set_flags(VF, b & 0x40); set_flags(NF, b & 0x80); }
        void i_bmi() { if (p & NF) { check_branch_cycles(); pc = operand; } }
        void i_bne() { if (!(p & ZF)) { check_branch_cycles(); pc = operand; } }
        void i_bpl() { if (!(p & NF)) { check_branch_cycles(); pc = operand; } }
        void i_brk() { set_flags(IF, true); push(pc); push((u8)(p | 0b00110000)); set_flags(BF, false); pc = (bus::read(0xffff) << 8) | bus::read(0xfffe); }
        void i_bvc() { if (!(p & VF)) { check_branch_cycles(); pc = operand; } }
        void i_bvs() { if (p & VF) { check_branch_cycles(); pc = operand; } }
        void i_clc() { set_flags(CF, false); }
        void i_cld() { set_flags(DF, false); }
        void i_cli() { set_flags(IF, false); }
        void i_clv() { set_flags(VF, false); }
        void i_cmp() { u8 b = bus::read(operand), r = a - b; set_flags(CF, a >= b); set_flags(ZF, a == b); set_flags(NF, r & 0x80); }
        void i_cpx() { u8 b = bus::read(operand), r = x - b; set_flags(CF, x >= b); set_flags(ZF, x == b); set_flags(NF, r & 0x80); }
        void i_cpy() { u8 b = bus::read(operand), r = y - b; set_flags(CF, y >= b); set_flags(ZF, y == b); set_flags(NF, r & 0x80); }
        void i_dec() { u8 b = bus::read(operand); b--; bus::write(operand, b); set_flags(ZF, !b); set_flags(NF, b & 0x80); } // Side-effects?
        void i_dex() { x--; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
        void i_dey() { y--; set_flags(ZF, !y); set_flags(NF, y & 0x80); }
        void i_eor() { a ^= bus::read(operand); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_inc() { u8 b = bus::read(operand); b++; bus::write(operand, b); set_flags(ZF, !b); set_flags(NF, b & 0x80); } // Side-effects?
        void i_inx() { x++; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
        void i_iny() { y++; set_flags(ZF, !y); set_flags(NF, y & 0x80); }
        void i_jmp() { pc = operand; }
        void i_jsr() { push((u16)(pc - 1)); pc = operand; } // Hack?
        void i_lda() { u8 b = bus::read(operand); set_flags(ZF, !b); set_flags(NF, b & 0x80); a = b; add_page_cross_cycles(); }
        void i_ldx() { u8 b = bus::read(operand); set_flags(ZF, !b); set_flags(NF, b & 0x80); x = b; add_page_cross_cycles(); }
        void i_ldy() { u8 b = bus::read(operand); set_flags(ZF, !b); set_flags(NF, b & 0x80); y = b; add_page_cross_cycles(); }
        void i_ora() { a |= bus::read(operand); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_pha() { push(a); }
        void i_php() { push((u8)(p | 0b00110000)); }
        void i_pla() { a = pop1(); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_plp() { u8 b = p; p = pop1() & 0b11001111; p |= b & 0b00110000; }
        void i_rti() { u8 b = p; p = pop1() & 0b11001111; p |= b & 0b00110000; pc = pop(); }
        void i_rts() { pc = pop() + 1; }
        void i_sec() { set_flags(CF, true); }
        void i_sed() { set_flags(DF, true); }
        void i_sei() { set_flags(IF, true); }
        void i_sta() { bus::write(operand, a); }
        void i_stx() { bus::write(operand, x); }
        void i_sty() { bus::write(operand, y); }
        void i_tax() { set_flags(ZF, !a); set_flags(NF, a & 0x80); x = a; }
        void i_tay() { set_flags(ZF, !a); set_flags(NF, a & 0x80); y = a; }
        void i_txa() { set_flags(ZF, !x); set_flags(NF, x & 0x80); a = x; }
        void i_tya() { set_flags(ZF, !y); set_flags(NF, y & 0x80); a = y; }
        void i_txs() { sp = x; }
        void i_tsx() { set_flags(ZF, !sp); set_flags(NF, sp & 0x80); x = sp; }
        void i_asl() {
            if (opcode == 0x0a) {
                set_flags(CF, a & 0x80);

                a <<= 1;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x80);

                b <<= 1;

                set_flags(ZF, !b);
                set_flags(NF, b & 0x80);

                bus::write(operand, b);
            }
        }

        void i_lsr() {
            set_flags(NF, false);

            if (opcode == 0x4a) {
                set_flags(CF, a & 0x1);

                a >>= 1;

                set_flags(ZF, !a);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x1);

                b >>= 1;

                set_flags(ZF, !b);

                bus::write(operand, b);
            }
        }

        void i_rol() {
            bool c = p & CF;

            if (opcode == 0x2a) {
                set_flags(CF, a & 0x80);

                a <<= 1;

                if (c) a |= 0x1;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x80);

                b <<= 1;

                if (c) b |= 0x1;

                set_flags(ZF, !b);
                set_flags(NF, b & 0x80);

                bus::write(operand, b);
            }
        }

        void i_ror() {
            bool c = p & CF;

            if (opcode == 0x6a) {
                set_flags(CF, a & 0x1);

                a >>= 1;
                if (c) a |= 0x80;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x1);

                b >>= 1;
                if (c) b |= 0x80;

                set_flags(ZF, !b);
                set_flags(NF, b & 0x80);

                bus::write(operand, b);
            }
        }

        void i_nop() {
            if ((opcode & 0xf) == 0xc) add_page_cross_cycles(); // Also emulate invalid NOPs
        }

        // Unofficial opcodes
        // void i_alr();
        // void i_anc();
        // void i_arr();
        // void i_axs();
        void i_lax() { i_lda(); i_tax(); }
        void i_sax() { bus::write(operand, a & x); }
        void i_dcp() { i_dec(); i_cmp(); }
        void i_isc() { i_inc(); i_sbc(); }
        void i_rla() { i_rol(); i_and(); }
        void i_rra() { i_ror(); i_adc(); }
        void i_slo() { i_asl(); i_ora(); }
        void i_sre() { i_lsr(); i_eor(); }
        // void i_tas();
        // void i_skb();
        // void i_ign();
    }
}