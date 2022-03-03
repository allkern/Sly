#pragma once

#include "../../global.hpp"

#include "addressing_modes.hpp"
#include "instructions.hpp"
#include "disasm.hpp"

namespace snes {
    namespace apu {
        namespace spc {
            u8 opcode = 0;

            void init() {
                pc  = bus::read16(0xfffe);
                a   = 0x0;
                x   = 0x0;
                y   = 0x0;
                sp  = 0x0;
                psw = 0x0;
            }

            u8 disasm_buf[3] = { 0 };

            u16 base_pc = 0x0;

            void fetch() {
                base_pc = pc;

                disasm_buf[0] = bus::read(base_pc + 0);
                disasm_buf[1] = bus::read(base_pc + 1);
                disasm_buf[2] = bus::read(base_pc + 2);

                opcode = bus::read(pc++);

                last_cycles = 0;
                additional_cycles = 0;
            }

            void log_cpu_state() {
                char* disassembled = spc700_disasm(disasm_buf, base_pc);

                size_t len = std::strlen(disassembled);

                int space_count = 22 - len;

                char* spaces = new char[space_count + 1];

                std::memset(spaces, ' ', space_count);
                spaces[space_count] = 0;

                std::strcat(disassembled, spaces);

                delete spaces;

                if (disassembled[len - 1] == 0x20) len--;

                _log(debug, "%spc=%04x, a=%02x, x=%02x, y=%02x, ya=%04x, psw=%02x, sp=%02x, stack=%02x %02x %02x %02x %02x %02x %02x %02x, lc=%u",
                    disassembled,
                    base_pc,
                    registers::a,
                    registers::x,
                    registers::y,
                    (u16)registers::ya,
                    registers::psw,
                    registers::sp,
                    ((registers::sp + 1) <= 0xef) ? bus::read(registers::sp + 1) : 0x0,
                    ((registers::sp + 2) <= 0xef) ? bus::read(registers::sp + 2) : 0x0,
                    ((registers::sp + 3) <= 0xef) ? bus::read(registers::sp + 3) : 0x0,
                    ((registers::sp + 4) <= 0xef) ? bus::read(registers::sp + 4) : 0x0,
                    ((registers::sp + 5) <= 0xef) ? bus::read(registers::sp + 5) : 0x0,
                    ((registers::sp + 6) <= 0xef) ? bus::read(registers::sp + 6) : 0x0,
                    ((registers::sp + 7) <= 0xef) ? bus::read(registers::sp + 7) : 0x0,
                    ((registers::sp + 8) <= 0xef) ? bus::read(registers::sp + 8) : 0x0,
                    last_cycles
                );
            }

            int residual_cycles = 0,
                pending_cycles = 0;

            int get_cycles(int last) {
                int total = residual_cycles + last;

                if (total == 3) {
                    residual_cycles = 0;

                    return 1;
                }
                if (total >= 3) {
                    int count = std::floor(total / 3);

                    residual_cycles = total - (3 * count);

                    return count;
                }
                if (total <= 3) {
                    residual_cycles += last_cycles;

                    return 0;
                }

                return 0;
            }

            void execute();

            void tick(int last) {
                pending_cycles += get_cycles(last);

                if (pending_cycles >= last_cycles) {
                    while (pending_cycles > 0) {
                        fetch();
                        execute();

                        pending_cycles -= last_cycles;
                    }
                }
                if (pending_cycles == last_cycles) {
                    fetch();
                    execute();

                    pending_cycles = 0;
                }
            }

            bool start_logging = false;

            void execute() {
                //if (registers::pc >= 0xfffb) start_logging = true;
                //if (start_logging) log_cpu_state();

                switch (opcode) {
                    case 0x99: { ixy(); adcm(); last_cycles = 5; } break;
                    case 0x88: { imm(); adc(); last_cycles = 2; } break;
                    case 0x86: { xin(); adc(); last_cycles = 3; } break;
                    case 0x97: { idy(); adc(); last_cycles = 6; } break;
                    case 0x87: { idx(); adc(); last_cycles = 6; } break;
                    case 0x84: { dpg(); adc(); last_cycles = 3; } break;
                    case 0x94: { dpx(); adc(); last_cycles = 4; } break;
                    case 0x85: { abs(); adc(); last_cycles = 4; } break;
                    case 0x95: { abx(); adc(); last_cycles = 5; } break;
                    case 0x96: { aby(); adc(); last_cycles = 5; } break;
                    case 0x89: { dp2(); adcm(); last_cycles = 6; } break;
                    case 0x98: { dpi(); adcm(); last_cycles = 5; } break;
                    case 0x7a: { dpg(); addw(); last_cycles = 5; } break;
                    case 0x39: { ixy(); andm(); last_cycles = 5; } break;
                    case 0x28: { imm(); band(); last_cycles = 2; } break;
                    case 0x26: { xin(); band(); last_cycles = 3; } break;
                    case 0x37: { idy(); band(); last_cycles = 6; } break;
                    case 0x27: { idx(); band(); last_cycles = 6; } break;
                    case 0x24: { dpg(); band(); last_cycles = 3; } break;
                    case 0x34: { dpx(); band(); last_cycles = 4; } break;
                    case 0x25: { abs(); band(); last_cycles = 4; } break;
                    case 0x35: { abx(); band(); last_cycles = 5; } break;
                    case 0x36: { aby(); band(); last_cycles = 5; } break;
                    case 0x29: { dp2(); andm(); last_cycles = 6; } break;
                    case 0x38: { dpi(); andm(); last_cycles = 5; } break;
                    case 0x6a: { abs(); and1(true); last_cycles = 4; } break;
                    case 0x4a: { abs(); and1(false); last_cycles = 4; } break;
                    case 0x1c: { imp(); asl(); last_cycles = 2; } break;
                    case 0x0b: { dpg(); aslm(); last_cycles = 4; } break;
                    case 0x1b: { dpx(); aslm(); last_cycles = 5; } break;
                    case 0x0c: { abs(); aslm(); last_cycles = 5; } break;
                    case 0x13: { dpg(); bbc(0); last_cycles = 5; } break;
                    case 0x33: { dpg(); bbc(1); last_cycles = 5; } break;
                    case 0x53: { dpg(); bbc(2); last_cycles = 5; } break;
                    case 0x73: { dpg(); bbc(3); last_cycles = 5; } break;
                    case 0x93: { dpg(); bbc(4); last_cycles = 5; } break;
                    case 0xb3: { dpg(); bbc(5); last_cycles = 5; } break;
                    case 0xd3: { dpg(); bbc(6); last_cycles = 5; } break;
                    case 0xf3: { dpg(); bbc(7); last_cycles = 5; } break;
                    case 0x03: { dpg(); bbs(0); last_cycles = 5; } break;
                    case 0x23: { dpg(); bbs(1); last_cycles = 5; } break;
                    case 0x43: { dpg(); bbs(2); last_cycles = 5; } break;
                    case 0x63: { dpg(); bbs(3); last_cycles = 5; } break;
                    case 0x83: { dpg(); bbs(4); last_cycles = 5; } break;
                    case 0xa3: { dpg(); bbs(5); last_cycles = 5; } break;
                    case 0xc3: { dpg(); bbs(6); last_cycles = 5; } break;
                    case 0xe3: { dpg(); bbs(7); last_cycles = 5; } break;
                    case 0x90: { rel(); bcc(); last_cycles = 2; } break;
                    case 0xb0: { rel(); bcs(); last_cycles = 2; } break;
                    case 0xf0: { rel(); beq(); last_cycles = 2; } break;
                    case 0x30: { rel(); bmi(); last_cycles = 2; } break;
                    case 0xd0: { rel(); bne(); last_cycles = 2; } break;
                    case 0x10: { rel(); bpl(); last_cycles = 2; } break;
                    case 0x50: { rel(); bvc(); last_cycles = 2; } break;
                    case 0x70: { rel(); bvs(); last_cycles = 2; } break;
                    case 0x2f: { rel(); bra(); last_cycles = 4; } break;
                    case 0x0f: { imp(); brk(); last_cycles = 8; } break;
                    case 0x3f: { abs(); call(address); last_cycles = 8; } break;
                    case 0xde: { dpx(); cmp(); rel(); bne(); last_cycles = 6; } break;
                    case 0x2e: { dpg(); cmp(); rel(); bne(); last_cycles = 5; } break;
                    case 0x12: { dpg(); clr1(0); last_cycles = 4; } break;
                    case 0x32: { dpg(); clr1(1); last_cycles = 4; } break;
                    case 0x52: { dpg(); clr1(2); last_cycles = 4; } break;
                    case 0x72: { dpg(); clr1(3); last_cycles = 4; } break;
                    case 0x92: { dpg(); clr1(4); last_cycles = 4; } break;
                    case 0xb2: { dpg(); clr1(5); last_cycles = 4; } break;
                    case 0xd2: { dpg(); clr1(6); last_cycles = 4; } break;
                    case 0xf2: { dpg(); clr1(7); last_cycles = 4; } break;
                    case 0x60: { imp(); clrc(); last_cycles = 2; } break;
                    case 0x20: { imp(); clrp(); last_cycles = 2; } break;
                    case 0xe0: { imp(); clrv(); last_cycles = 2; } break;
                    case 0x79: { ixy(); cmpm(); last_cycles = 5; } break;
                    case 0x68: { imm(); cmp(); last_cycles = 2; } break;
                    case 0x66: { xin(); cmp(); last_cycles = 3; } break;
                    case 0x77: { idy(); cmp(); last_cycles = 6; } break;
                    case 0x67: { idx(); cmp(); last_cycles = 6; } break;
                    case 0x64: { dpg(); cmp(); last_cycles = 3; } break;
                    case 0x74: { dpx(); cmp(); last_cycles = 4; } break;
                    case 0x65: { abs(); cmp(); last_cycles = 4; } break;
                    case 0x75: { abx(); cmp(); last_cycles = 5; } break;
                    case 0x76: { aby(); cmp(); last_cycles = 5; } break;
                    case 0xc8: { imm(); cpx(); last_cycles = 2; } break;
                    case 0x3e: { dpg(); cpx(); last_cycles = 3; } break;
                    case 0x1e: { abs(); cpx(); last_cycles = 4; } break;
                    case 0xad: { imm(); cpy(); last_cycles = 2; } break;
                    case 0x7e: { dpg(); cpy(); last_cycles = 3; } break;
                    case 0x5e: { abs(); cpy(); last_cycles = 4; } break;
                    case 0x69: { dp2(); cmpm(); last_cycles = 6; } break;
                    case 0x78: { dpi(); cmpm(); last_cycles = 5; } break;
                    case 0x5a: { dpg(); cmpw(); last_cycles = 4; } break;
                    case 0xdf: { imp(); nop(); last_cycles = 3; } break; //   DAA   A                decimal adjust for addition      N.....ZC
                    case 0xbe: { imp(); nop(); last_cycles = 3; } break; //   DAS   A                decimal adjust for subtraction   N.....ZC
                    case 0xfe: { imp(); dey(); rel(); bne(); last_cycles = 4; } break;
                    case 0x6e: { dpg(); decm(); rel(); bne(); last_cycles = 5; } break;
                    case 0x9c: { imp(); dec(); last_cycles = 2; } break;
                    case 0x1d: { imp(); dex(); last_cycles = 2; } break;
                    case 0xdc: { imp(); dey(); last_cycles = 2; } break;
                    case 0x8b: { dpg(); decm(); last_cycles = 4; } break;
                    case 0x9b: { dpx(); decm(); last_cycles = 5; } break;
                    case 0x8c: { abs(); decm(); last_cycles = 5; } break;
                    case 0x1a: { dpg(); decw(); last_cycles = 6; } break;
                    case 0xc0: { imp(); di(); last_cycles = 3; } break;
                    case 0x9e: { imp(); div(); last_cycles = 12; } break;
                    case 0xa0: { imp(); ei(); last_cycles = 3; } break;
                    case 0x59: { ixy(); eorm(); last_cycles = 5; } break;
                    case 0x48: { imm(); eor();  last_cycles = 2; } break;
                    case 0x46: { xin(); eor();  last_cycles = 3; } break;
                    case 0x57: { idy(); eor();  last_cycles = 6; } break;
                    case 0x47: { idx(); eor();  last_cycles = 6; } break;
                    case 0x44: { dpg(); eor();  last_cycles = 3; } break;
                    case 0x54: { dpx(); eor();  last_cycles = 4; } break;
                    case 0x45: { abs(); eor();  last_cycles = 4; } break;
                    case 0x55: { abx(); eor();  last_cycles = 5; } break;
                    case 0x56: { aby(); eor();  last_cycles = 5; } break;
                    case 0x49: { dp2(); eorm(); last_cycles = 6; } break;
                    case 0x58: { dpi(); eorm(); last_cycles = 5; } break;
                    case 0x8a: { abs(); eor1(false); last_cycles = 5; } break;
                    case 0xbc: { imp(); inc(); last_cycles = 2; } break;
                    case 0x3d: { imp(); inx(); last_cycles = 2; } break;
                    case 0xfc: { imp(); iny(); last_cycles = 2; } break;
                    case 0xab: { dpg(); incm(); last_cycles = 4; } break;
                    case 0xbb: { dpx(); incm(); last_cycles = 5; } break;
                    case 0xac: { abs(); incm(); last_cycles = 5; } break;
                    case 0x3a: { dpg(); incw(); last_cycles = 6; } break;
                    case 0x1f: { abx(); jmp(); last_cycles = 6; } break;
                    case 0x5f: { abs(); jmp(); last_cycles = 3; } break;
                    case 0x5c: { imp(); lsr(); last_cycles = 2; } break;
                    case 0x4b: { dpg(); lsrm(); last_cycles = 4; } break;
                    case 0x5b: { dpx(); lsrm(); last_cycles = 5; } break;
                    case 0x4c: { abs(); lsrm(); last_cycles = 5; } break;
                    case 0xaf: { ixi(); sta(); last_cycles = 4; } break;
                    case 0xc6: { xin(); sta(); last_cycles = 4; } break;
                    case 0xd7: { idy(); sta(); last_cycles = 7; } break;
                    case 0xc7: { idx(); sta(); last_cycles = 7; } break;
                    case 0xe8: { imm(); lda(); last_cycles = 2; } break;
                    case 0xe6: { xin(); lda(); last_cycles = 3; } break;
                    case 0xbf: { ixi(); lda(); last_cycles = 4; } break;
                    case 0xf7: { idy(); lda(); last_cycles = 6; } break;
                    case 0xe7: { idx(); lda(); last_cycles = 6; } break;
                    case 0x7d: { imp(); txa(); last_cycles = 2; } break;
                    case 0xdd: { imp(); tya(); last_cycles = 2; } break;
                    case 0xe4: { dpg(); lda(); last_cycles = 3; } break;
                    case 0xf4: { dpx(); lda(); last_cycles = 4; } break;
                    case 0xe5: { abs(); lda(); last_cycles = 4; } break;
                    case 0xf5: { abx(); lda(); last_cycles = 5; } break;
                    case 0xf6: { aby(); lda(); last_cycles = 5; } break;
                    case 0xbd: { imp(); txs(); last_cycles = 2; } break;
                    case 0xcd: { imm(); ldx(); last_cycles = 2; } break;
                    case 0x5d: { imp(); tax(); last_cycles = 2; } break;
                    case 0x9d: { imp(); tsx(); last_cycles = 2; } break;
                    case 0xf8: { dpg(); ldx(); last_cycles = 3; } break;
                    case 0xf9: { dpy(); ldx(); last_cycles = 4; } break;
                    case 0xe9: { abs(); ldx(); last_cycles = 4; } break;
                    case 0x8d: { imm(); ldy(); last_cycles = 2; } break;
                    case 0xfd: { imp(); tay(); last_cycles = 2; } break;
                    case 0xeb: { dpg(); ldy(); last_cycles = 3; } break;
                    case 0xfb: { dpx(); ldy(); last_cycles = 4; } break;
                    case 0xec: { abs(); ldy(); last_cycles = 4; } break;
                    case 0xfa: { dp2(); mov(); last_cycles = 5; } break;
                    case 0xd4: { dpx(); sta(); last_cycles = 5; } break;
                    case 0xdb: { dpx(); sty(); last_cycles = 5; } break;
                    case 0xd9: { dpy(); stx(); last_cycles = 5; } break;
                    case 0x8f: { dpi(); mov(); last_cycles = 5; } break;
                    case 0xc4: { dpg(); sta(); last_cycles = 4; } break;
                    case 0xd8: { dpg(); stx(); last_cycles = 4; } break;
                    case 0xcb: { dpg(); sty(); last_cycles = 4; } break;
                    case 0xd5: { abx(); sta(); last_cycles = 6; } break;
                    case 0xd6: { aby(); sta(); last_cycles = 6; } break;
                    case 0xc5: { abs(); sta(); last_cycles = 5; } break;
                    case 0xc9: { abs(); stx(); last_cycles = 5; } break;
                    case 0xcc: { abs(); sty(); last_cycles = 5; } break;
                    case 0xaa: { abs(); ldc(); last_cycles = 4; } break;
                    case 0xca: { abs(); stc(); last_cycles = 6; } break;
                    case 0xba: { dpg(); ldw(); last_cycles = 5; } break;
                    case 0xda: { dpg(); stw(); last_cycles = 5; } break;
                    case 0xcf: { imp(); mul(); last_cycles = 9; } break;
                    case 0x00: { imp(); nop(); last_cycles = 2; } break;
                    case 0xea: { abs(); not1(); last_cycles = 5; } break;
                    case 0xed: { imp(); notc(); last_cycles = 3; } break;
                    case 0x19: { ixy(); orm(); last_cycles = 5; } break;
                    case 0x08: { imm(); ora(); last_cycles = 2; } break;
                    case 0x06: { xin(); ora(); last_cycles = 3; } break;
                    case 0x17: { idy(); ora(); last_cycles = 6; } break;
                    case 0x07: { idx(); ora(); last_cycles = 6; } break;
                    case 0x04: { dpg(); ora(); last_cycles = 3; } break;
                    case 0x14: { dpx(); ora(); last_cycles = 4; } break;
                    case 0x05: { abs(); ora(); last_cycles = 4; } break;
                    case 0x15: { abx(); ora(); last_cycles = 5; } break;
                    case 0x16: { aby(); ora(); last_cycles = 5; } break;
                    case 0x09: { dp2(); orm(); last_cycles = 6; } break;
                    case 0x18: { dpi(); orm(); last_cycles = 5; } break;
                    case 0x2a: { abs(); or1(true); last_cycles = 5; } break;
                    case 0x0a: { abs(); or1(false); last_cycles = 5; } break;
                    case 0x4f: { imm(); pcall(); last_cycles = 6; } break;
                    case 0xae: { imp(); popa(); last_cycles = 4; } break;
                    case 0x8e: { imp(); popp(); last_cycles = 4; } break;
                    case 0xce: { imp(); popx(); last_cycles = 4; } break;
                    case 0xee: { imp(); popy(); last_cycles = 4; } break;
                    case 0x2d: { imp(); pusha(); last_cycles = 4; } break;
                    case 0x0d: { imp(); pushp(); last_cycles = 4; } break;
                    case 0x4d: { imp(); pushx(); last_cycles = 4; } break;
                    case 0x6d: { imp(); pushy(); last_cycles = 4; } break;
                    case 0x6f: { imp(); ret(); last_cycles = 5; } break;
                    case 0x7f: { imp(); reti(); last_cycles = 6; } break;
                    case 0x3c: { imp(); rol(); last_cycles = 2; } break;
                    case 0x2b: { dpg(); rolm(); last_cycles = 4; } break;
                    case 0x3b: { dpx(); rolm(); last_cycles = 5; } break;
                    case 0x2c: { abs(); rolm(); last_cycles = 5; } break;
                    case 0x7c: { imp(); ror(); last_cycles = 2; } break;
                    case 0x6b: { dpg(); rorm(); last_cycles = 4; } break;
                    case 0x7b: { dpx(); rorm(); last_cycles = 5; } break;
                    case 0x6c: { abs(); rorm(); last_cycles = 5; } break;
                    case 0xb9: { ixy(); sbcm(); last_cycles = 5; } break;
                    case 0xa8: { imm(); sbc(); last_cycles = 2; } break;
                    case 0xa6: { xin(); sbc(); last_cycles = 3; } break;
                    case 0xb7: { idy(); sbc(); last_cycles = 6; } break;
                    case 0xa7: { idx(); sbc(); last_cycles = 6; } break;
                    case 0xa4: { dpg(); sbc(); last_cycles = 3; } break;
                    case 0xb4: { dpx(); sbc(); last_cycles = 4; } break;
                    case 0xa5: { abs(); sbc(); last_cycles = 4; } break;
                    case 0xb5: { abx(); sbc(); last_cycles = 5; } break;
                    case 0xb6: { aby(); sbc(); last_cycles = 5; } break;
                    case 0xa9: { dp2(); sbcm(); last_cycles = 6; } break;
                    case 0xb8: { dpi(); sbcm(); last_cycles = 5; } break;
                    case 0x02: { dpg(); set1(0); last_cycles = 4; } break;
                    case 0x22: { dpg(); set1(1); last_cycles = 4; } break;
                    case 0x42: { dpg(); set1(2); last_cycles = 4; } break;
                    case 0x62: { dpg(); set1(3); last_cycles = 4; } break;
                    case 0x82: { dpg(); set1(4); last_cycles = 4; } break;
                    case 0xa2: { dpg(); set1(5); last_cycles = 4; } break;
                    case 0xc2: { dpg(); set1(6); last_cycles = 4; } break;
                    case 0xe2: { dpg(); set1(7); last_cycles = 4; } break;
                    case 0x80: { imp(); setc(); last_cycles = 2; } break;
                    case 0x40: { imp(); setp(); last_cycles = 2; } break;
                    case 0xef: { imp(); sleep(); last_cycles = 2; } break;
                    case 0xff: { imp(); stop(); last_cycles = 2; } break;
                    case 0x9a: { dpg(); subw(); last_cycles = 5; } break;
                    case 0x01: { imp(); tcall(0);  last_cycles = 8; } break;
                    case 0x11: { imp(); tcall(1);  last_cycles = 8; } break;
                    case 0x21: { imp(); tcall(2);  last_cycles = 8; } break;
                    case 0x31: { imp(); tcall(3);  last_cycles = 8; } break;
                    case 0x41: { imp(); tcall(4);  last_cycles = 8; } break;
                    case 0x51: { imp(); tcall(5);  last_cycles = 8; } break;
                    case 0x61: { imp(); tcall(6);  last_cycles = 8; } break;
                    case 0x71: { imp(); tcall(7);  last_cycles = 8; } break;
                    case 0x81: { imp(); tcall(8);  last_cycles = 8; } break;
                    case 0x91: { imp(); tcall(9);  last_cycles = 8; } break;
                    case 0xa1: { imp(); tcall(10); last_cycles = 8; } break;
                    case 0xb1: { imp(); tcall(11); last_cycles = 8; } break;
                    case 0xc1: { imp(); tcall(12); last_cycles = 8; } break;
                    case 0xd1: { imp(); tcall(13); last_cycles = 8; } break;
                    case 0xe1: { imp(); tcall(14); last_cycles = 8; } break;
                    case 0xf1: { imp(); tcall(15); last_cycles = 8; } break;
                    case 0x4e: { abs(); tclr1(); last_cycles = 6; } break;
                    case 0x0e: { abs(); tset1(); last_cycles = 6; } break;
                    case 0x9f: { imp(); xcn(); last_cycles = 5; } break;
                }

                last_cycles += additional_cycles;
            }
        }
    }
}
