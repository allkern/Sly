#pragma once

#include "addressing_modes.hpp"

namespace snes {
    namespace cpu {
        using namespace registers;

        bool branch_taken = false;

        void push(u16 value, bool mode) {
            bus::write(sp - (1 - mode), value, mode);

            sp -= mode ? 1 : 2;
        }

        void nmi() {
            if (!(nmitimen & 0x80)) { return; }

            waiting = false;

            push(p, true);

            bus::write24(sp - 2, pc);

            sp -= 3;

            pc = nmiv;
        }

        u16 pull(bool mode) {
            sp += mode ? 1 : 2;

            u16 value = bus::read(sp - (1 - mode), mode);

            return value;
        }

        void rti() {
            sp += 3;

            u32 addr = bus::read24(sp - 2);

            pc = addr;

            p = pull(true);
        }

        bool get_result_carry(u32 result) {
            return result > (test_flag(MF) ? 0xff : 0xffff); 
        }

        bool get_result_signed_overflow(bool op, u16 prev_a) {
            bool prevn = prev_a & (1 << (test_flag(MF) ? 7 : 15)),
                 newn  = a & (1 << (test_flag(MF) ? 7 : 15));

            return op ? (newn && !prevn) : (!newn && prevn);
        }

        void brk() {
            _log(warning, "Warning: BRK executed @ pc=%06x tc=%u", pc, total_cycles);
            push(p, true);

            waiting = false;

            pc += 1;
            bus::write24(sp - 2, pc);

            sp -= 3;

            pc = brkv;
        }

        void cop() {
            _log(warning, "Warning: COP executed @ pc=%06x tc=%u", pc, total_cycles);
            push(p, true);

            waiting = false;

            bus::write24(sp - 2, pc);

            sp -= 3;

            pc = copv;
        }

        void ora() {
            u16 value = bus::read(address, test_flag(MF));

            set_a(a | value);

            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }

        void eor() {
            u16 value = bus::read(address, test_flag(MF));

            set_a(a ^ value);

            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }

        void tsb() {
            u16 value = bus::read(address, test_flag(MF));

            set_flags(ZF, !(value & get_a()));

            bus::write(address, value | get_a(), test_flag(MF));

            if (!test_flag(MF)) additional_cycles += 2;
        }

        // ASL A
        void sla() {
            set_flags(CF, get_sign_a());

            u16 s = get_a() << 1;

            if (test_flag(MF)) {
                a &= 0xff00;
                a |= s & 0xff;
            } else {
                a = s;
            }
    
            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }

        void asl() {
            u16 value = bus::read(address, test_flag(MF));

            set_flags(CF, get_sign(value, test_flag(MF)));

            value <<= 1;
            if (test_flag(MF)) value &= 0xff;

            set_flags(ZF, get_zero(value, test_flag(MF)));
            set_flags(NF, get_sign(value, test_flag(MF)));

            bus::write(address, value, test_flag(MF));

            if (test_flag(MF)) additional_cycles += 2;
        }

        // LSR A
        void sra() {
            set_flags(CF, a & 0x1);

            u16 s = get_a() >> 1;

            if (test_flag(MF)) {
                a &= 0xff00;
                a |= s & 0xff;
            } else {
                a = s;
            }

            set_flags(ZF, get_zero_a());
            set_flags(NF, false);
        }

        void lsr() {
            u16 value = bus::read(address, test_flag(MF));

            set_flags(CF, value & 0x1);

            value >>= 1;
            if (test_flag(MF)) value &= 0xff;

            set_flags(ZF, get_zero(value, test_flag(MF)));
            set_flags(NF, false);

            bus::write(address, value, test_flag(MF));

            if (test_flag(MF)) additional_cycles += 2;
        }

        void pha() { push(a, test_flag(MF)); if (!test_flag(MF)) additional_cycles += 1; }
        void phb() { push(db, true); }
        void phd() { push(d, false); }
        void phk() { push((pc >> 16) & 0xff, true); }
        void php() { push(p, true); }
        void phx() { push(x, test_flag(XF)); if (!test_flag(XF)) additional_cycles += 1; }
        void phy() { push(y, test_flag(XF)); if (!test_flag(XF)) additional_cycles += 1; }
        void pla() { a = pull(test_flag(MF)); if (!test_flag(MF)) additional_cycles += 1; set_flags(ZF, get_zero_a()); set_flags(NF, get_sign_a()); }
        void plb() { db = pull(true); set_flags(ZF, get_zero(db, true)); set_flags(NF, get_sign(db, true)); }
        void pld() { d = pull(false); set_flags(ZF, get_zero(d, false)); set_flags(NF, get_sign(d, false));}
        void plp() { p = pull(true); }
        void plx() { x = pull(test_flag(XF)); if (!test_flag(XF)) additional_cycles += 1; set_flags(ZF, get_zero_x()); set_flags(NF, get_sign_x()); }
        void ply() { y = pull(test_flag(XF)); if (!test_flag(XF)) additional_cycles += 1; set_flags(ZF, get_zero_y()); set_flags(NF, get_sign_y()); }
        
        void branch_relative8(u8 offset) {
            pc += ((i32)(i8)offset);
            additional_cycles += 1;
        }
        void bcc() { if (!test_flag(CF)) { pc = address; additional_cycles += 1; } }
        void bcs() { if ( test_flag(CF)) { pc = address; additional_cycles += 1; } }
        void bne() { if (!test_flag(ZF)) { pc = address; additional_cycles += 1; } }
        void beq() { if ( test_flag(ZF)) { pc = address; additional_cycles += 1; } }
        void bpl() { if (!test_flag(NF)) { pc = address; additional_cycles += 1; } }
        void bmi() { if ( test_flag(NF)) { pc = address; additional_cycles += 1; } }
        void bvc() { if (!test_flag(VF)) { pc = address; additional_cycles += 1; } }
        void bvs() { if ( test_flag(VF)) { pc = address; additional_cycles += 1; } }
        void bra() { pc = address; }
        void brl() { pc = address; }

        void trb() {
            u16 value = bus::read(address, test_flag(MF));

            set_flags(ZF, !(value & get_a()));

            bus::write(address, value & ~get_a(), test_flag(MF));

            if (!test_flag(MF)) additional_cycles += 2;
        }

        // INC A
        void ina() {
            u16 s = get_a() + 1;

            if (test_flag(MF)) {
                a &= 0xff00;
                a |= s & 0xff;
            } else {
                a = s;
            }
    
            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());

            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }

        void inc() {
            u16 value = bus::read(address, test_flag(MF));

            value += 1;

            bus::write(address, value, test_flag(MF));

            set_flags(ZF, get_zero(value, test_flag(MF)));
            set_flags(NF, get_sign(value, test_flag(MF)));
        }

        // DEC A
        void dea() {
            u16 s = get_a() - 1;

            if (test_flag(MF)) {
                a &= 0xff00;
                a |= s & 0xff;
            } else {
                a = s;
            }
    
            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }

        void dec() {
            u16 value = bus::read(address, test_flag(MF));

            value -= 1;

            bus::write(address, value, test_flag(MF));

            set_flags(ZF, get_zero(value, test_flag(MF)));
            set_flags(NF, get_sign(value, test_flag(MF)));
        }

        void jsr() {
            //_log(debug, "jsr from %06x to %06x y=%04x", pc, address, y);
            pc -= 1;
            push(pc & 0xffff, false);
            pc = address;
        }

        void jsl() {
            //_log(debug, "jsr from %06x to %06x", pc, address);
            pc -= 1;
            bus::write24(sp - 2, pc);

            sp -= 3;
            pc = address;
        }

        void bit() {
            u16 value = bus::read(address, test_flag(MF));

            if (!immediate_mode) {
                set_flags(NF, get_sign(value, test_flag(MF)));
                set_flags(VF, get_sign(value << 1, test_flag(MF)));
            }

            set_flags(ZF, get_zero(a & value, test_flag(MF)));

            if (test_flag(MF)) additional_cycles += 1;
        }

        // ROL A
        void rla() {
            bool carry = get_sign_a();

            u16 r = get_a();

            r <<= 1;
            r |= test_flag(CF) ? 1 : 0;

            if (test_flag(MF)) {
                a &= 0xff00;
                a |= r & 0xff;
            } else {
                a = r;
            }

            set_flags(CF, carry);
            set_flags(NF, get_sign_a());
            set_flags(ZF, get_zero_a());
        }

        void rol() {
            u16 value = bus::read(address, test_flag(MF));

            bool carry = value & (test_flag(MF) ? 0x80 : 0x8000);

            value <<= 1;
            value |= test_flag(CF);

            set_flags(CF, carry);

            bus::write(address, value, test_flag(MF));

            set_flags(NF, get_sign(value, test_flag(MF)));
            set_flags(ZF, get_zero(value, test_flag(MF)));
        }

        // ROR A
        void rra() {
            bool carry = a & 0x1;

            u16 r = get_a();

            r >>= 1;
            r |= test_flag(CF) << (test_flag(MF) ? 7 : 15);

            if (test_flag(MF)) {
                a &= 0xff00;
                a |= r & 0xff;
            } else {
                a = r;
            }

            set_flags(CF, carry);
            set_flags(NF, get_sign_a());
            set_flags(ZF, get_zero_a());
        }

        void ror() {
            u16 value = bus::read(address, test_flag(MF));

            bool carry = value & 0x1;

            value >>= 1;
            value |= test_flag(CF) << (test_flag(MF) ? 7 : 15);

            set_flags(CF, carry);

            bus::write(address, value, test_flag(MF));

            set_flags(NF, get_sign(value, test_flag(MF)));
            set_flags(ZF, get_zero(value, test_flag(MF)));
        }

        void sec() { set_flags(CF, true); }
        void sei() { set_flags(IF, true); }
        void sed() { set_flags(DF, true); }
        void clc() { set_flags(CF, false); }
        void cli() { set_flags(IF, false); }
        void cld() { set_flags(DF, false); }
        void clv() { set_flags(VF, false); }

        void wdm() {}

        void mvp() {
            while (a != 0xffff) {
                u8 srcb = bus::read8(src | x--);

                bus::write8(dst | y--, srcb);

                a--;

                additional_cycles += 7;
            }
        }

        void mvn() {
            while (a != 0xffff) {
                u8 srcb = bus::read8(src | x++);

                bus::write8(dst | y++, srcb);

                a--;

                additional_cycles += 7;
            }
        }

        void jmp() { pc = address; }

        void rts() {
            u16 addr = pull(false) + 1;
            //_log(debug, "rts from %06x to %06x", pc, addr);
            
            pc &= 0xff0000;
            pc |= addr;
        }

        void rtl() {
            sp += 3;

            u32 addr = bus::read24(sp - 2) + 1;
            //_log(debug, "rtl from %06x to %06x", pc, addr);

            pc = addr;
        }

        void per() { push(address, false); } // To-do
        void pei() { push(address, false); } // To-do
        void pea() { push(address, false); } // To-do

        void stp() { stopped = true; } // To-do

        void dex() {
            u16 s = get_x() - 1;

            if (test_flag(XF)) {
                x &= 0xff00;
                x |= s & 0xff;
            } else {
                x = s;
            }
    
            set_flags(ZF, get_zero_x());
            set_flags(NF, get_sign_x());
        }

        void dey() {
            u16 s = get_y() - 1;

            if (test_flag(XF)) {
                y &= 0xff00;
                y |= s & 0xff;
            } else {
                y = s;
            }
    
            set_flags(ZF, get_zero_y());
            set_flags(NF, get_sign_y());
        }

        void xba() {
            u8 t = a & 0xff;
            a >>= 8;
            a |= t << 8;

            set_flags(ZF, !(a & 0xff));
            set_flags(NF, a & 0x80);
        }

        void xce() {
            if (test_flag(MF)) {
                x &= 0xff;
                y &= 0xff;
            }
        }

        void cpy() {
            u16 value = bus::read(address, test_flag(XF));
            u16 result = y - value;

            set_flags(ZF, get_y() == (value & (test_flag(XF) ? 0xff : 0xffff)));
            set_flags(NF, get_sign(result, test_flag(XF)));
            set_flags(CF, get_y() >= (value & (test_flag(XF) ? 0xff : 0xffff)));

            if (test_flag(XF)) additional_cycles += 1;
        }
    
        void cpx() {
            u16 value = bus::read(address, test_flag(XF));
            u16 result = x - value;

            set_flags(ZF, get_x() == (value & (test_flag(XF) ? 0xff : 0xffff)));
            set_flags(NF, get_sign(result, test_flag(XF)));
            set_flags(CF, get_x() >= (value & (test_flag(XF) ? 0xff : 0xffff)));

            if (test_flag(XF)) additional_cycles += 1;
        }

        void cmp() {
            u16 value = bus::read(address, test_flag(MF));
            u16 result = a - value;

            set_flags(ZF, get_a() == (value & (test_flag(MF) ? 0xff : 0xffff)));
            set_flags(NF, get_sign(result, test_flag(MF)));
            set_flags(CF, get_a() >= (value & (test_flag(MF) ? 0xff : 0xffff)));
        
            if (test_flag(MF)) additional_cycles += 1;
        }

        void rep() { set_flags(bus::read(address, true), false); }
        void sep() {
            set_flags(bus::read(address, true), true);

            if (test_flag(XF)) {
                x &= 0xff;
                y &= 0xff;
            }
        }

        void inx() {
            x++;

            set_flags(ZF, get_zero_x());
            set_flags(NF, get_sign_x());
        }

        void iny() {
            y++;

            set_flags(ZF, get_zero_y());
            set_flags(NF, get_sign_y());
        }

        void wai() { waiting = true; } // To-do

        void adc8() {
            u8 value = bus::read(address, test_flag(MF));
            u8 pa = get_a();

            u32 result = 0;

            if (test_flag(DF)) {
                result = (a & 0x0f) + (value & 0x0f) + test_flag(CF);

                if (result > 0x09) result += 0x06;

                result = (a & 0xf0) + (value & 0xf0) + (result > 0x0f ? 0x10 : 0) + (result & 0x0f);
            } else {
                result = (u32)get_a() + value + test_flag(CF);
            }

            set_flags(VF, ~(a ^ value) & (a ^ result) & 0x80);

            if (test_flag(DF) && (result > 0x9f))
                result += 0x60;
            
            set_a(result);

            u8 na = get_a();
            
            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
            set_flags(CF, result > 0xff);
        }

        void adc16() {
            u16 value = bus::read(address, test_flag(MF));
            u16 pa = get_a();

            u32 result = 0;
    
            if (test_flag(DF)) {
                result = (a & 0x0f) + (value & 0x0f) + test_flag(CF);

                if(result > 0x09) result += 0x06;
                result = (a & 0xf0) + (value & 0xf0) + (result > 0x0f ? 0x10 : 0) + (result & 0x0f);

                if(result > 0x9f) result += 0x60;
                result = (a & 0xf00) + (value & 0xf00) + (result > 0xff ? 0x100 : 0) + (result & 0xff);

                if(result > 0x9ff) result += 0x600;
                result = (a & 0xf000) + (value & 0xf000) + (result > 0xfff ? 0x1000 : 0) + (result & 0xfff);
            } else {
                result = (u32)get_a() + value + test_flag(CF);
            }

            set_flags(VF, ~(a ^ value) & (a ^ result) & 0x8000);

            if (test_flag(DF) && (result > 0x9fff))
                result += 0x6000;

            set_a(result);

            u16 na = get_a();
            
            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
            set_flags(CF, result > 0xffff);
        }

        void adc() {
            if (test_flag(MF)) adc8(); else adc16();
        }

        void sbc8() {
            u8 value = bus::read(address, test_flag(MF));
            u8 pa = get_a();
            i32 result = 0;

            value = ~value;
            value &= 0xff;

            if (test_flag(DF)) {
                result = (a & 0x0f) + (value & 0x0f) + test_flag(CF);
                if (result <= 0x0f) result -= 0x06;
                result = (a & 0xf0) + (value & 0xf0) + (result > 0x0f ? 0x10 : 0) + (result & 0x0f);
            } else {
                result = (a & 0xff) + (value & 0xff) + test_flag(CF);
            }

            set_flags(VF, ~(a ^ value) & (a ^ result) & 0x80);

            if(test_flag(DF) && (result <= 0xff))
                result -= 0x60;

            set_a(result);

            u8 na = get_a();

            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
            set_flags(CF, result > 0xff);
        }

        void sbc16() {
            u16 value = bus::read(address, test_flag(MF));
            u16 pa = get_a();
            i32 result = 0;

            value = ~value;
            value &= 0xffff;

            if (test_flag(DF)) {
                result = (a & 0x0f) + (value & 0x0f) + test_flag(CF);

                if(result <= 0x0f) result -= 0x06;
                result = (a & 0xf0) + (value & 0xf0) + (result > 0x0f ? 0x10 : 0) + (result & 0x0f);

                if(result <= 0xff) result -= 0x60;
                result = (a & 0xf00) + (value & 0xf00) + (result > 0xff ? 0x100 : 0) + (result & 0xff);

                if(result <= 0xfff) result -= 0x600;
                result = (a & 0xf000) + (value & 0xf000) + (result > 0xfff ? 0x1000 : 0) + (result & 0xfff);
            } else {
                result = a + value + test_flag(CF);
            }

            set_flags(VF, ~(a ^ value) & (a ^ result) & 0x8000);

            if(test_flag(DF) && (result <= 0xffff))
                result -= 0x6000;

            set_a(result);

            u16 na = get_a();

            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
            set_flags(CF, result > 0xffff);
        }

        void sbc() {
            if (test_flag(MF)) sbc8(); else sbc16();
        }
        
        // AND
        void bnd() {
            u16 value = bus::read(address, test_flag(MF));

            set_a(a & value);

            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }

        void lda() { set_a(bus::read(address, false)); set_flags(NF, get_sign_a()); set_flags(ZF, get_zero_a()); }
        void ldx() { set_x(bus::read(address, false)); set_flags(NF, get_sign_x()); set_flags(ZF, get_zero_x()); }
        void ldy() { set_y(bus::read(address, false)); set_flags(NF, get_sign_y()); set_flags(ZF, get_zero_y()); }
        void sta() { bus::write(address, a, test_flag(MF)); }
        void stx() { bus::write(address, x, test_flag(XF)); }
        void sty() { bus::write(address, y, test_flag(XF)); }
        void stz() { bus::write(address, 0, test_flag(MF)); }
        void tax() {
            if (test_flag(XF)) {
                x &= 0xff00;
                x |= a & 0xff;
            } else {
                x = a;
            }
            set_flags(ZF, get_zero_x());
            set_flags(NF, get_sign_x());
        }

        void tay() {
            if (test_flag(XF)) {
                y &= 0xff00;
                y |= a & 0xff;
            } else {
                y = a;
            }
            set_flags(ZF, get_zero_y());
            set_flags(NF, get_sign_y());
        }
        void tcd() { d = a; set_flags(ZF, !d); set_flags(NF, d & 0x8000); }
        void tcs() { sp = a; }
        void tdc() { a = d; set_flags(ZF, !a); set_flags(NF, a & 0x8000); }
        void tsc() { a = sp; set_flags(ZF, !a); set_flags(NF, a & 0x8000); }
        void tsx() { x = sp; set_flags(ZF, get_zero_x()); set_flags(NF, get_sign_x()); }
        void txa() {
            if (test_flag(MF)) {
                a &= 0xff00;
                a |= x & 0xff;
            } else {
                a = x;
            }
            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }
        void txs() { sp = x; }
        void txy() {
            if (test_flag(XF)) {
                y &= 0xff00;
                y |= x & 0xff;
            } else {
                y = x;
            }
            set_flags(ZF, get_zero_y());
            set_flags(NF, get_sign_y());
        }
        void tya() {
            if (test_flag(MF)) {
                a &= 0xff00;
                a |= y & 0xff;
            } else {
                a = y;
            }
            set_flags(ZF, get_zero_a());
            set_flags(NF, get_sign_a());
        }
        void tyx() {
            if (test_flag(XF)) {
                x &= 0xff00;
                x |= y & 0xff;
            } else {
                x = y;
            }
            set_flags(ZF, get_zero_x());
            set_flags(NF, get_sign_x());
        }

        void nop() {}
    }
}