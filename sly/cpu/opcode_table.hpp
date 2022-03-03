#pragma once

#include "../global.hpp"

#include "instructions.hpp"
#include "addressing_modes.hpp"

#include <unordered_map>

namespace snes {
    namespace cpu {
        typedef void (*operation_t)(void);
        typedef void (*addressing_mode_t)(void);

        struct opcode_t {
            operation_t operation;
            addressing_mode_t addressing_mode;
            int cycles;
        };

        opcode_t map[] = {
            //     0                1                2                3                4                5                6                7                8                9                a                b                c                d                e                f
            /*0*/{ brk, imp, 7 }, { ora, dix, 6 }, { cop, imm, 7 }, { ora, str, 4 }, { tsb, dpg, 5 }, { ora, dpg, 3 }, { asl, dpg, 5 }, { ora, dil, 6 }, { php, imp, 3 }, { ora, ima, 2 }, { sla, imp, 2 }, { phd, imp, 4 }, { tsb, abd, 6 }, { ora, abd, 4 }, { asl, abd, 6 }, { ora, lng, 5 },
            /*1*/{ bpl, rel, 2 }, { ora, diy, 5 }, { ora, dpi, 5 }, { ora, sri, 7 }, { trb, dpg, 5 }, { ora, dpx, 4 }, { asl, dpx, 6 }, { ora, dly, 6 }, { clc, imp, 2 }, { ora, aby, 4 }, { ina, imp, 2 }, { tcs, imp, 2 }, { trb, abd, 6 }, { ora, abx, 4 }, { asl, abx, 7 }, { ora, lnx, 4 },
            /*2*/{ jsr, abj, 6 }, { bnd, dix, 6 }, { jsl, lng, 8 }, { bnd, str, 4 }, { bit, dpg, 3 }, { bnd, dpg, 3 }, { rol, dpg, 5 }, { bnd, dil, 6 }, { plp, imp, 4 }, { bnd, ima, 2 }, { rla, imp, 2 }, { pld, imp, 5 }, { bit, abd, 4 }, { bnd, abd, 4 }, { rol, abd, 6 }, { bnd, lng, 5 },
            /*3*/{ bmi, rel, 2 }, { bnd, diy, 5 }, { bnd, dpi, 5 }, { bnd, sri, 7 }, { bit, dpx, 4 }, { bnd, dpx, 4 }, { rol, dpx, 6 }, { bnd, dly, 6 }, { sec, imp, 2 }, { bnd, aby, 4 }, { dea, imp, 2 }, { tsc, imp, 2 }, { bit, abx, 4 }, { bnd, abx, 4 }, { rol, abx, 7 }, { bnd, lnx, 5 },
            /*4*/{ rti, imp, 6 }, { eor, dix, 6 }, { wdm, imm, 2 }, { eor, str, 4 }, { mvp, bmm, 1 }, { eor, dpg, 3 }, { lsr, dpg, 5 }, { eor, dil, 6 }, { pha, imp, 3 }, { eor, ima, 2 }, { sra, imp, 2 }, { phk, imp, 3 }, { jmp, abj, 3 }, { eor, abd, 4 }, { lsr, abd, 6 }, { eor, lng, 5 },
            /*5*/{ bvc, rel, 2 }, { eor, diy, 5 }, { eor, dpi, 5 }, { eor, sri, 6 }, { mvn, bmm, 1 }, { eor, dpx, 4 }, { lsr, dpx, 6 }, { eor, dly, 6 }, { cli, imp, 2 }, { eor, aby, 4 }, { phy, imp, 3 }, { tcd, imp, 2 }, { jmp, lng, 4 }, { eor, abx, 4 }, { lsr, abx, 7 }, { eor, lnx, 5 },
            /*6*/{ rts, imp, 6 }, { adc, dix, 6 }, { per, rll, 6 }, { adc, str, 4 }, { stz, dpg, 3 }, { adc, dpg, 3 }, { ror, dpg, 5 }, { adc, dil, 6 }, { pla, imp, 4 }, { adc, ima, 2 }, { rra, imp, 2 }, { rtl, imp, 6 }, { jmp, abi, 5 }, { adc, abd, 4 }, { ror, abd, 6 }, { adc, lng, 5 },
            /*7*/{ bvs, rel, 2 }, { adc, diy, 5 }, { adc, dpi, 5 }, { adc, sri, 7 }, { stz, dpx, 4 }, { adc, dpx, 4 }, { ror, dpx, 6 }, { adc, dly, 6 }, { sei, imp, 2 }, { adc, aby, 4 }, { ply, imp, 4 }, { tdc, imp, 2 }, { jmp, aix, 6 }, { adc, abx, 4 }, { ror, abx, 7 }, { adc, lnx, 5 },
            /*8*/{ bra, rel, 3 }, { sta, dix, 6 }, { brl, rll, 4 }, { sta, str, 4 }, { sty, dpg, 3 }, { sta, dpg, 3 }, { stx, dpg, 3 }, { sta, dil, 6 }, { dey, imp, 2 }, { bit, ima, 2 }, { txa, imp, 2 }, { phb, imp, 3 }, { sty, abd, 4 }, { sta, abd, 4 }, { stx, abd, 4 }, { sta, lng, 5 },
            /*9*/{ bcc, rel, 2 }, { sta, diy, 6 }, { sta, dpi, 5 }, { sta, sri, 7 }, { sty, dpx, 4 }, { sta, dpx, 4 }, { stx, dpy, 4 }, { sta, dly, 6 }, { tya, imp, 2 }, { sta, aby, 5 }, { txs, imp, 2 }, { txy, imp, 2 }, { stz, abd, 4 }, { sta, abx, 5 }, { stz, abx, 5 }, { sta, lnx, 5 },
            /*a*/{ ldy, imx, 2 }, { lda, dix, 6 }, { ldx, imx, 2 }, { lda, str, 4 }, { ldy, dpg, 3 }, { lda, dpg, 3 }, { ldx, dpg, 3 }, { lda, dil, 6 }, { tay, imp, 2 }, { lda, ima, 2 }, { tax, imp, 2 }, { plb, imp, 4 }, { ldy, abd, 4 }, { lda, abd, 4 }, { ldx, abd, 4 }, { lda, lng, 5 },
            /*b*/{ bcs, rel, 2 }, { lda, diy, 5 }, { lda, dpi, 5 }, { lda, sri, 7 }, { ldy, dpx, 4 }, { lda, dpx, 4 }, { ldx, dpy, 4 }, { lda, dly, 6 }, { clv, imp, 2 }, { lda, aby, 4 }, { tsx, imp, 2 }, { tyx, imp, 2 }, { ldy, abx, 4 }, { lda, abx, 4 }, { ldx, aby, 4 }, { lda, lnx, 5 },
            /*c*/{ cpy, imx, 2 }, { cmp, dix, 6 }, { rep, imm, 3 }, { cmp, str, 4 }, { cpy, dpg, 3 }, { cmp, dpg, 3 }, { dec, dpg, 5 }, { cmp, dil, 6 }, { iny, imp, 2 }, { cmp, ima, 2 }, { dex, imp, 2 }, { wai, imp, 3 }, { cpy, abd, 4 }, { cmp, abd, 4 }, { dec, abd, 6 }, { cmp, lng, 5 },
            /*d*/{ bne, rel, 2 }, { cmp, diy, 5 }, { cmp, dpi, 5 }, { cmp, sri, 7 }, { pei, dpi, 6 }, { cmp, dpx, 4 }, { dec, dpx, 6 }, { cmp, dly, 6 }, { cld, imp, 2 }, { cmp, aby, 4 }, { phx, imp, 3 }, { stp, imp, 3 }, { jmp, ail, 6 }, { cmp, abx, 4 }, { dec, abx, 7 }, { cmp, lnx, 5 },
            /*e*/{ cpx, imx, 2 }, { sbc, dix, 6 }, { sep, imm, 3 }, { sbc, str, 4 }, { cpx, dpg, 3 }, { sbc, dpg, 3 }, { inc, dpg, 5 }, { sbc, dil, 6 }, { inx, imp, 2 }, { sbc, ima, 2 }, { nop, imp, 2 }, { xba, imp, 3 }, { cpx, abd, 4 }, { sbc, abd, 4 }, { inc, abd, 6 }, { sbc, lng, 5 },
            /*f*/{ beq, rel, 2 }, { sbc, diy, 5 }, { sbc, dpi, 5 }, { sbc, sri, 7 }, { pea, abd, 5 }, { sbc, dpx, 4 }, { inc, dpx, 6 }, { sbc, dly, 6 }, { sed, imp, 2 }, { sbc, aby, 4 }, { plx, imp, 4 }, { xce, imp, 2 }, { jsr, aix, 8 }, { sbc, abx, 4 }, { inc, abx, 7 }, { sbc, lnx, 5 }
        };
    }
}