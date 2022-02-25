#pragma once

#include "../../global.hpp"

#include "registers.hpp"
#include "../bus.hpp"

namespace snes {
    namespace apu {
        namespace spc {
            using namespace registers;

            void lda() { a = bus::read(address); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
            void ldx() { x = bus::read(address); set_flags(ZF, !x); set_flags(NF, x & 0x80); }
            void ldy() { y = bus::read(address); set_flags(ZF, !y); set_flags(NF, y & 0x80); }
            void ldc() {
                u16 addr = address & 0x1fff;
                int b = (address >> 13) & 0x7;

                set_flags(CF, bus::read(addr) & (1 << b));
            }
            
            void sta() { bus::write(address, a); }
            void stx() { bus::write(address, x); }
            void sty() { bus::write(address, y); }
            void stc() {
                u16 addr = address & 0x1fff;
                int b = (address >> 13) & 0x7;

                bus::write(addr, (bus::read(addr) & (~(u8)(1 << b))) | (1 << b));
            }
            void notc() { set_flags(CF, !test_flag(CF)); }

            void txa() { a = x; set_flags(ZF, !a); set_flags(NF, a & 0x80); }
            void tya() { a = y; set_flags(ZF, !a); set_flags(NF, a & 0x80); }
            void tax() { x = a; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
            void tay() { y = a; set_flags(ZF, !y); set_flags(NF, y & 0x80); }
            void txs() { x = sp; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
            void tsx() { sp = x; }
            void mov() { bus::write(dst, bus::read(src)); }
            void ldw() { ya = bus::read16(address); set_flags(ZF, !(u16)ya); set_flags(NF, (u16)ya & 0x8000); }
            void stw() { bus::write16(address, (u16)ya); }

            void band() {
                u8 value = bus::read(address);

                a &= value;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            }

            void not1() {
                u16 addr = address & 0x1fff;
                int b = (address >> 13) & 0x7;
                u8 value = bus::read(addr);
                bool bit = value & (1 << b);

                bus::write(addr, (value & ~(1 << b)) | ((!value) << b));
            }

            void or1(bool n) {
                u16 addr = address & 0x1fff;
                int b = (address >> 13) & 0x7;
                u8 value = bus::read(addr);
                bool bit = value & (1 << b);

                set_flags(CF, test_flag(CF) || (n ? !bit : bit));
            }

            void eor1(bool n) {
                u16 addr = address & 0x1fff;
                int b = (address >> 13) & 0x7;
                u8 value = bus::read(addr);
                bool bit = value & (1 << b);

                set_flags(CF, test_flag(CF) ^ (n ? !bit : bit));
            }

            void and1(bool n) {
                u16 addr = address & 0x1fff;
                int b = (address >> 13) & 0x7;
                u8 value = bus::read(addr);
                bool bit = value & (1 << b);

                bus::write(addr, test_flag(CF) && (n ? !bit : bit));
            }

            void andm() {
                u8 source = bus::read(src),
                   value = bus::read(dst);

                source &= value;

                set_flags(ZF, !source);
                set_flags(NF, source & 0x80);

                bus::write(src, source);
            }

            void ora() {
                u8 value = bus::read(address);

                a |= value;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            }
            
            void orm() {
                u8 source = bus::read(src),
                   value = bus::read(dst);

                source |= value;

                set_flags(ZF, !source);
                set_flags(NF, source & 0x80);

                bus::write(src, source);
            }

            void eor() {
                u8 value = bus::read(address);

                a ^= value;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            }
            
            void eorm() {
                u8 source = bus::read(src),
                   value = bus::read(dst);

                source ^= value;

                set_flags(ZF, !source);
                set_flags(NF, source & 0x80);

                bus::write(src, source);
            }

            void adc() {
                u8 value = bus::read(address);

                u16 result = a + value + test_flag(CF);

                set_flags(HF, (a < 0x10) && (result >= 0x10));
                set_flags(VF, ~(a ^ value) & (a ^ result) & 0x80);

                a = result & 0xff;

                set_flags(NF, a & 0x80);
                set_flags(ZF, !a);
                set_flags(CF, result > 0xff);
            }

            void clrc() { set_flags(CF, false); }
            void clrp() { set_flags(PF, false); }
            void clrv() { set_flags(VF, false); }
            void setc() { set_flags(CF, true); }
            void setp() { set_flags(PF, true); }
            void setv() { set_flags(VF, true); }
            void clr1(int b) { bus::write(address, bus::read(address) & (~(u8)(1 << b))); }

            void nop() {}

            void adcm() {
                u8 source = bus::read(src),
                   value = bus::read(dst);

                u16 result = source + value + test_flag(CF);
                
                set_flags(HF, (source < 0x10) && (result >= 0x10));
                set_flags(VF, ~(source ^ value) & (source ^ result) & 0x80);

                bus::write(src, result & 0xff);

                set_flags(NF, (result & 0xff) & 0x80);
                set_flags(ZF, !(result & 0xff));
                set_flags(CF, result > 0xff);
            }

            void sbc() {
                u8 value = ~bus::read(address);

                i16 result = a + value + test_flag(CF);

                set_flags(HF, (a < 0x10) && (result >= 0x10));
                set_flags(VF, ~(a ^ value) & (a ^ result) & 0x80);

                a = result & 0xff;

                set_flags(NF, a & 0x80);
                set_flags(ZF, !a);
                set_flags(CF, result > 0xff);
            }

            void sbcm() {
                i8 source = bus::read(src),
                   value = ~bus::read(dst);

                i16 result = source + value + test_flag(CF);
                
                set_flags(HF, (source < 0x10) && (result >= 0x10));
                set_flags(VF, ~(source ^ value) & (source ^ result) & 0x80);

                bus::write(src, result & 0xff);

                set_flags(NF, (result & 0xff) & 0x80);
                set_flags(ZF, !(result & 0xff));
                set_flags(CF, result > 0xff);
            }

            void cmp() {
                u8 source = a, value = bus::read(address);

                u16 result = source - value;

                set_flags(ZF, !(result & 0xff));
                set_flags(NF, result & 0x80);
                set_flags(CF, result > 0xff);
            }

            // Compare Memory
            void cmpm() {
                u8 source = bus::read(src),
                   value = bus::read(dst);
                
                //_log(debug, "dst=%02x, src=%02x (%02x)", dst, src, source);

                u16 result = value - source;

                set_flags(ZF, !(result & 0xff));
                set_flags(NF, result & 0x80);
                set_flags(CF, result > 0xff);
            }

            // Compare Y
            void cpy() {
                u8 source = y, value = bus::read(address);

                u16 result = source - value;

                set_flags(ZF, !(result & 0xff));
                set_flags(NF, result & 0x80);
                set_flags(CF, result > 0xff);
            }

            // Compare X
            void cpx() {
                u8 source = x, value = bus::read(address);

                u16 result = source - value;

                set_flags(ZF, !(result & 0xff));
                set_flags(NF, result & 0x80);
                set_flags(CF, result > 0xff);
            }

            void call(u16 addr) {
                bus::write(sp--, pc & 0xff);
                bus::write(sp--, (pc >> 8) & 0xff);

                pc = addr;
            }

            void tcall(int v) {
                static const u8 vectors[] = {
                    0xde, 0xdc, 0xda, 0xd8, 0xd6, 0xd4, 0xd2, 0xd0,
                    0xce, 0xcc, 0xca, 0xc8, 0xc6, 0xc4, 0xc2, 0xc0
                };

                call(0xff00 | vectors[v]);
            }
            
            void pcall() { call(0xff00 | bus::read(address)); }
            void jmp() { pc = address; }

            void set1(int b) { bus::write(address, bus::read(address) | (1 << b)); }

            // Increment
            void inc() { a++; set_flags(ZF, !a); set_flags(NF, a & 0x80); }
            void inx() { x++; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
            void iny() { y++; set_flags(ZF, !y); set_flags(NF, y & 0x80); }
            void incm() { u8 v = bus::read(address) + 1; bus::write(address, v); set_flags(ZF, !v); set_flags(NF, v & 0x80); }

            // Decrement
            void dec() { a--; set_flags(ZF, !a); set_flags(NF, a & 0x80); }
            void dex() { x--; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
            void dey() { y--; set_flags(ZF, !y); set_flags(NF, y & 0x80); }
            void decm() { u8 v = bus::read(address) - 1; bus::write(address, v); set_flags(ZF, !v); set_flags(NF, v & 0x80); }

            // BRA
            void bbc(int b) { u8 d = bus::read(address); rel(); if (!(d & (1 << b))) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bbs(int b) { u8 d = bus::read(address); rel(); if (  d & (1 << b) ) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void beq() { if ( test_flag(ZF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bne() { if (!test_flag(ZF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bcs() { if ( test_flag(CF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bcc() { if (!test_flag(CF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bvs() { if ( test_flag(VF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bvc() { if (!test_flag(VF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bmi() { if ( test_flag(NF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bpl() { if (!test_flag(NF)) { pc += (i16)(i8)address; additional_cycles = 2; } }
            void bra() { pc += address; }

            void rol() {
                bool c = test_flag(CF);

                set_flags(CF, a & 0x80);

                a <<= 1;
                a |= c;

                set_flags(NF, a & 0x80);
                set_flags(ZF, !a);
            }

            void rolm() {
                u8 value = bus::read(address);

                bool c = test_flag(CF);

                set_flags(CF, value & 0x80);

                value <<= 1;
                value |= c;

                set_flags(NF, value & 0x80);
                set_flags(ZF, !value);

                bus::write(address, value);
            }

            void ror() {
                bool c = test_flag(CF);

                set_flags(CF, a & 0x1);

                a >>= 1;
                a |= c << 7;

                set_flags(NF, a & 0x80);
                set_flags(ZF, !a);
            }

            void rorm() {
                u8 value = bus::read(address);

                bool c = test_flag(CF);

                set_flags(CF, value & 0x1);

                value >>= 1;
                value |= c << 7;

                set_flags(NF, value & 0x80);
                set_flags(ZF, !value);

                bus::write(address, value);
            }

            void lsr() {
                set_flags(CF, a & 0x1);

                a >>= 1;

                set_flags(NF, false);
                set_flags(ZF, !a);
            }

            void lsrm() {
                u8 v = bus::read(address);

                set_flags(CF, v & 0x1);

                v >>= 1;

                set_flags(NF, false);
                set_flags(ZF, !v);

                bus::write(address, v);
            }

            void asl() {
                set_flags(CF, a & 0x80);

                a <<= 1;

                set_flags(NF, a & 0x80);
                set_flags(ZF, !a);
            }

            void aslm() {
                u8 v = bus::read(address);

                set_flags(CF, v & 0x80);

                v <<= 1;

                set_flags(NF, v & 0x80);
                set_flags(ZF, !v);

                bus::write(address, v);
            }

            void tclr1() {
                u8 value = bus::read(address) & ~a;
                u8 s = a - value;

                set_flags(NF, s & 0x80);
                set_flags(ZF, !s);

                bus::write(address, value);
            }

            void sleep() { pc--; }
            void stop() { pc--; }

            void pusha() { bus::write(sp--, a); }
            void pushp() { bus::write(sp--, psw); }
            void pushx() { bus::write(sp--, x); }
            void pushy() { bus::write(sp--, y); }
            void popa() { a = bus::read(++sp); }
            void popp() { psw = bus::read(++sp); }
            void popx() { x = bus::read(++sp); }
            void popy() { y = bus::read(++sp); }

            void di() { set_flags(IF, false); }
            void ei() { set_flags(IF, true); }

            void ret() {
                pc &= 0xff; pc |= bus::read(++sp) << 8;
                pc &= 0xff00; pc |= bus::read(++sp);
            }
            
            void reti() {
                psw = bus::read(++sp);
                pc &= 0xff; pc |= bus::read(++sp) << 8;
                pc &= 0xff00; pc |= bus::read(++sp);
            }

            void brk() {
                bus::write(sp--, pc & 0xff);
                bus::write(sp--, (pc >> 8) & 0xff);
                bus::write(sp--, psw);

                pc = 0xffde;
            }

            void tset1() {
                u8 value = bus::read(address) | a;
                u8 s = a - value;

                set_flags(NF, s & 0x80);
                set_flags(ZF, !s);

                bus::write(address, value);
            }

            void xcn() {
                a = (a >> 4) | (a << 4);

                set_flags(NF, a & 0x80);
                set_flags(ZF, !a);
            }

            void subw() {
                u16 value = bus::read16(address);
                u32 result = (u16)ya - value;

                ya = result & 0xffff;
                
                set_flags(ZF, !(u16)ya);
                set_flags(CF, result > 0xffff);
                set_flags(NF, (u16)ya & 0x8000);
            }

            void cmpw() {
                u16 value = bus::read16(address);
                u32 result = (u16)ya - value;

                set_flags(ZF, !(result & 0xffff));
                set_flags(CF, result > 0xffff);
                set_flags(NF, result & 0x8000);
            }

            void addw() {
                u16 value = bus::read16(address);
                int hcr = ((a + (value & 0xff)) > 0xff) ? 1 : 0;
                u32 result = (u16)ya + value;

                set_flags(VF, ~(ya ^ value) & (ya ^ result) & 0x8000);
                set_flags(HF, ((y & 0x0f) + ((value >> 8) & 0xff) + hcr) & 0x10);

                ya = result & 0xffff;

                set_flags(ZF, !(u16)ya);
                set_flags(CF, result > 0xffff);
                set_flags(NF, (u16)ya & 0x8000);
            }

            void incw() { bus::write16(address, bus::read16(address) + 1); }
            void decw() { bus::write16(address, bus::read16(address) - 1); }

            void div() {
                set_flags(HF, (y & 0xf) >= (x & 0xf));
                set_flags(VF, y >= x);

                if (y < (x << 1)) {
                    //if quotient is <= 511 (will fit into 9-bit result)
                    a = (u16)ya / x;
                    y = (u16)ya % x;
                } else {
                    //otherwise, the quotient won't fit into VF + A
                    //this emulates the odd behavior of the S-SMP in this case
                    a = 255 - ((u16)ya - (x << 9)) / (256 - x);
                    y = x   + ((u16)ya - (x << 9)) % (256 - x);
                }
                //result is set based on a (quotient) only
                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            }

            void mul() {
                u16 result = y * a;
                a = result >> 0;
                y = result >> 8;
                //result is set based on y (high-byte) only
                set_flags(ZF, !y);
                set_flags(NF, y & 0x80);
            }
        }
    }
}