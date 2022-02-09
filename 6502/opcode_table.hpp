#pragma once

#include "../global.hpp"

#include "instructions.hpp"
#include "addressing_modes.hpp"

#include <array>

namespace nes {
    namespace cpu {
        struct opcode_t {
            addressing_mode_t addressing_mode = nullptr;
            instruction_t instruction = nullptr;
            unsigned int t = 0;
        };

        static const constexpr opcode_t opcode_map[] = {
            /////   0                  1                  2                  3                  4                  5                  6                  7                  8                  9                  a                  b                  c                  d                  e                  f
            /*0*/ { imm, i_brk, 7 }, { idx, i_ora, 6 }, { imp, i_ill, 2 }, { idx, i_slo, 8 }, { zpg, i_nop, 3 }, { zpg, i_ora, 3 }, { zpg, i_asl, 5 }, { zpg, i_slo, 5 }, { imp, i_php, 3 }, { imm, i_ora, 2 }, { acc, i_asl, 2 }, { imm, i_ill, 2 }, { abs, i_nop, 4 }, { abs, i_ora, 4 }, { abs, i_asl, 6 }, { abs, i_slo, 6 },
            /*1*/ { rel, i_bpl, 2 }, { idy, i_ora, 5 }, { imp, i_ill, 2 }, { idy, i_slo, 8 }, { zpx, i_nop, 4 }, { zpx, i_ora, 4 }, { zpx, i_asl, 6 }, { zpx, i_slo, 6 }, { imp, i_clc, 2 }, { aby, i_ora, 4 }, { imp, i_nop, 2 }, { aby, i_slo, 7 }, { abx, i_nop, 4 }, { abx, i_ora, 4 }, { abx, i_asl, 7 }, { abx, i_slo, 7 },
            /*2*/ { abs, i_jsr, 6 }, { idx, i_and, 6 }, { imp, i_ill, 2 }, { idx, i_rla, 8 }, { zpg, i_bit, 3 }, { zpg, i_and, 3 }, { zpg, i_rol, 5 }, { zpg, i_rla, 5 }, { imp, i_plp, 4 }, { imm, i_and, 2 }, { acc, i_rol, 2 }, { imm, i_ill, 2 }, { abs, i_bit, 4 }, { abs, i_and, 4 }, { abs, i_rol, 6 }, { abs, i_rla, 6 },
            /*3*/ { rel, i_bmi, 2 }, { idy, i_and, 5 }, { imp, i_ill, 2 }, { idy, i_rla, 8 }, { zpx, i_nop, 4 }, { zpx, i_and, 4 }, { zpx, i_rol, 6 }, { zpx, i_rla, 6 }, { imp, i_sec, 2 }, { aby, i_and, 4 }, { imp, i_nop, 2 }, { aby, i_rla, 7 }, { abx, i_nop, 4 }, { abx, i_and, 4 }, { abx, i_rol, 7 }, { abx, i_rla, 7 },
            /*4*/ { imp, i_rti, 6 }, { idx, i_eor, 6 }, { imp, i_ill, 2 }, { idx, i_sre, 8 }, { zpg, i_nop, 3 }, { zpg, i_eor, 3 }, { zpg, i_lsr, 5 }, { zpg, i_sre, 5 }, { imp, i_pha, 3 }, { imm, i_eor, 2 }, { acc, i_lsr, 2 }, { imm, i_ill, 2 }, { abs, i_jmp, 3 }, { abs, i_eor, 4 }, { abs, i_lsr, 6 }, { abs, i_sre, 6 },
            /*5*/ { rel, i_bvc, 2 }, { idy, i_eor, 5 }, { imp, i_ill, 2 }, { idy, i_sre, 8 }, { zpx, i_nop, 4 }, { zpx, i_eor, 4 }, { zpx, i_lsr, 6 }, { zpx, i_sre, 6 }, { imp, i_cli, 2 }, { aby, i_eor, 4 }, { imp, i_nop, 2 }, { aby, i_sre, 7 }, { abx, i_nop, 4 }, { abx, i_eor, 4 }, { abx, i_lsr, 7 }, { abx, i_sre, 7 },
            /*6*/ { imp, i_rts, 6 }, { idx, i_adc, 6 }, { imp, i_ill, 2 }, { idx, i_rra, 8 }, { zpg, i_nop, 3 }, { zpg, i_adc, 3 }, { zpg, i_ror, 5 }, { zpg, i_rra, 5 }, { imp, i_pla, 4 }, { imm, i_adc, 2 }, { acc, i_ror, 2 }, { imm, i_ill, 2 }, { ind, i_jmp, 5 }, { abs, i_adc, 4 }, { abs, i_ror, 6 }, { abs, i_rra, 6 },
            /*7*/ { rel, i_bvs, 2 }, { idy, i_adc, 5 }, { imp, i_ill, 2 }, { idy, i_rra, 8 }, { zpx, i_nop, 4 }, { zpx, i_adc, 4 }, { zpx, i_ror, 6 }, { zpx, i_rra, 6 }, { imp, i_sei, 2 }, { aby, i_adc, 4 }, { imp, i_nop, 2 }, { aby, i_rra, 7 }, { abx, i_nop, 4 }, { abx, i_adc, 4 }, { abx, i_ror, 7 }, { abx, i_rra, 7 },
            /*8*/ { imm, i_nop, 2 }, { idx, i_sta, 6 }, { imp, i_nop, 2 }, { idx, i_sax, 6 }, { zpg, i_sty, 3 }, { zpg, i_sta, 3 }, { zpg, i_stx, 3 }, { zpg, i_sax, 3 }, { imp, i_dey, 2 }, { imp, i_nop, 2 }, { imp, i_txa, 2 }, { imm, i_ill, 2 }, { abs, i_sty, 4 }, { abs, i_sta, 4 }, { abs, i_stx, 4 }, { abs, i_sax, 4 },
            /*9*/ { rel, i_bcc, 2 }, { idy, i_sta, 6 }, { imp, i_ill, 2 }, { idy, i_ill, 6 }, { zpx, i_sty, 4 }, { zpx, i_sta, 4 }, { zpy, i_stx, 4 }, { zpy, i_sax, 4 }, { imp, i_tya, 2 }, { aby, i_sta, 5 }, { imp, i_txs, 2 }, { aby, i_ill, 5 }, { abx, i_nop, 5 }, { abx, i_sta, 5 }, { imp, i_ill, 5 }, { aby, i_ill, 5 },
            /*a*/ { imm, i_ldy, 2 }, { idx, i_lda, 6 }, { imm, i_ldx, 2 }, { idx, i_lax, 6 }, { zpg, i_ldy, 3 }, { zpg, i_lda, 3 }, { zpg, i_ldx, 3 }, { zpg, i_lax, 3 }, { imp, i_tay, 2 }, { imm, i_lda, 2 }, { imp, i_tax, 2 }, { imm, i_ill, 2 }, { abs, i_ldy, 4 }, { abs, i_lda, 4 }, { abs, i_ldx, 4 }, { abs, i_lax, 4 },
            /*b*/ { rel, i_bcs, 2 }, { idy, i_lda, 5 }, { imp, i_ill, 2 }, { idy, i_lax, 5 }, { zpx, i_ldy, 4 }, { zpx, i_lda, 4 }, { zpy, i_ldx, 4 }, { zpy, i_lax, 4 }, { imp, i_clv, 2 }, { aby, i_lda, 4 }, { imp, i_tsx, 2 }, { aby, i_ill, 4 }, { abx, i_ldy, 4 }, { abx, i_lda, 4 }, { aby, i_ldx, 4 }, { aby, i_lax, 4 },
            /*c*/ { imm, i_cpy, 2 }, { idx, i_cmp, 6 }, { imp, i_nop, 2 }, { idx, i_dcp, 8 }, { zpg, i_cpy, 3 }, { zpg, i_cmp, 3 }, { zpg, i_dec, 5 }, { zpg, i_dcp, 5 }, { imp, i_iny, 2 }, { imm, i_cmp, 2 }, { imp, i_dex, 2 }, { imm, i_ill, 2 }, { abs, i_cpy, 4 }, { abs, i_cmp, 4 }, { abs, i_dec, 6 }, { abs, i_dcp, 6 },
            /*d*/ { rel, i_bne, 2 }, { idy, i_cmp, 5 }, { imp, i_ill, 2 }, { idy, i_dcp, 8 }, { zpx, i_nop, 4 }, { zpx, i_cmp, 4 }, { zpx, i_dec, 6 }, { zpx, i_dcp, 6 }, { imp, i_cld, 2 }, { aby, i_cmp, 4 }, { imp, i_nop, 2 }, { aby, i_dcp, 7 }, { abx, i_nop, 4 }, { abx, i_cmp, 4 }, { abx, i_dec, 7 }, { abx, i_dcp, 7 },
            /*e*/ { imm, i_cpx, 2 }, { idx, i_sbc, 6 }, { imp, i_nop, 2 }, { idx, i_isc, 8 }, { zpg, i_cpx, 3 }, { zpg, i_sbc, 3 }, { zpg, i_inc, 5 }, { zpg, i_isc, 5 }, { imp, i_inx, 2 }, { imm, i_sbc, 2 }, { imp, i_nop, 2 }, { imm, i_sbc, 2 }, { abs, i_cpx, 4 }, { abs, i_sbc, 4 }, { abs, i_inc, 6 }, { abs, i_isc, 6 },
            /*f*/ { rel, i_beq, 2 }, { idy, i_sbc, 5 }, { imp, i_ill, 2 }, { idy, i_isc, 8 }, { zpx, i_nop, 4 }, { zpx, i_sbc, 4 }, { zpx, i_inc, 6 }, { zpx, i_isc, 6 }, { imp, i_sed, 2 }, { aby, i_sbc, 4 }, { imp, i_nop, 2 }, { aby, i_isc, 7 }, { abx, i_nop, 4 }, { abx, i_sbc, 4 }, { abx, i_inc, 7 }, { abx, i_isc, 7 }
        };

    }
}