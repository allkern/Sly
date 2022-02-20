#include <cstdio>
#include <cstring>
#include <cstdint>

uint8_t* m_buf = nullptr;
bool m_relative = true,
     m_hex = true;
uint16_t m_pc = 0x0;

char m_ws[0x1000] = { 0 };

char* spc700_disasm_impl(uint8_t o, const char *s, uint8_t a = 0, uint8_t b = 0, uint8_t c = 0, uint8_t d = 0) {
    for (int i = 0; i < 0x1000; i++) m_ws[i] = 0;

    uint8_t copy_a = a,
            copy_b = b,
            copy_c = c,
            copy_d = d;

    char t[16] = { 0 };

    int i   = 0,
        x   = 0,
        pos = 0,
        z   = 0;

    unsigned int v = 0;

    // if (m_hex) {
    //     for (i = 0; i < o; i++) {
    //         std::sprintf(t, "%02x ", m_buf[i]);
    //         std::strcat(m_ws, t);
    //     }
    
    //     for (; i < 3; i++) std::strcat(m_ws, "   ");

    //     std::strcat(m_ws, " ");
    // }

    z = std::strlen(m_ws);

    uint8_t* l = nullptr;

    for (i = 0; i < (int)std::strlen(s); i++) {
        if (s[i] == '*') {
            x = s[++i];

            switch (pos) {
                case 0: l = &copy_a; break;
                case 1: l = &copy_b; break;
                case 2: l = &copy_c; break;
                case 3: l = &copy_d; break;
                default: printf("IMPOSSIBLE POS!!\n"); break;
            }

            switch (x) {
                case 'b': { v = m_buf[*l]; std::sprintf(t, "%02x", v & 0xff); } break;
                case 'w': { v = m_buf[*l] | (m_buf[*l + 1] << 8); std::sprintf(t, "%04x", v & 0xffff); } break;
                case 'm': { v = m_buf[*l] | (m_buf[*l + 1] << 8); std::sprintf(t, "%04x,%d", v & 0x1fff, v >> 13); } break;
                case 'r': {
                    // printf("*l=%02x\n", *l);
                    v = m_buf[*l];
    
                    if (m_relative) {
                        v = (m_pc + o) + (signed char)v;
                        std::sprintf(t, "%04x", v & 0xffff);
                    } else {
                        std::sprintf(t, "%02x", v & 0xff);
                    }
                } break;
            }

            m_ws[z] = 0;

            std::strcat(m_ws, t);

            z += std::strlen(t);
        } else {
            m_ws[z++] = s[i];
        }
    }

    m_ws[z] = 0;

    //std::fprintf(wr, "%s\r\n", m_ws);
    return m_ws;
}

char* spc700_disasm(uint8_t* buf, uint16_t pc, bool relative = true, bool hex = true) {
    m_buf      = buf;
    m_pc       = pc;
    m_relative = relative;
    m_hex      = hex;

    switch (m_buf[0]) {
        case 0x00: return spc700_disasm_impl(1, "nop");                      break;
        case 0x01: return spc700_disasm_impl(1, "tcall 0");                  break;
        case 0x02: return spc700_disasm_impl(2, "set0  $*b", 2);             break;
        case 0x03: return spc700_disasm_impl(3, "bbs0  $*b, $*r",2, 2);      break;
        case 0x04: return spc700_disasm_impl(2, "or    a, $*b", 2);          break;
        case 0x05: return spc700_disasm_impl(3, "or    a, $*w", 2);          break;
        case 0x06: return spc700_disasm_impl(1, "or    a, (x)");             break;
        case 0x07: return spc700_disasm_impl(2, "or    a, ($*b+x)", 2);      break;
        case 0x08: return spc700_disasm_impl(2, "or    a, #$*b", 2);         break;
        case 0x09: return spc700_disasm_impl(3, "or    ($*b), ($*b)", 2, 2); break;
        case 0x0a: return spc700_disasm_impl(3, "or1   c, $*m", 2);          break;
        case 0x0b: return spc700_disasm_impl(2, "asl   $*b", 2);             break;
        case 0x0c: return spc700_disasm_impl(3, "asl   $*w", 2);             break;
        case 0x0d: return spc700_disasm_impl(1, "push  psw");                break;
        case 0x0e: return spc700_disasm_impl(3, "tset1 $*w", 2);             break;
        case 0x0f: return spc700_disasm_impl(1, "brk");                      break;
        case 0x10: return spc700_disasm_impl(2, "bpl   $*r", 2);             break;
        case 0x11: return spc700_disasm_impl(1, "tcall2");                   break;
        case 0x12: return spc700_disasm_impl(2, "clr0  $*b", 2);             break;
        case 0x13: return spc700_disasm_impl(3, "bbc0  $*b, $*r", 2, 2);     break;
        case 0x14: return spc700_disasm_impl(2, "or    a, $*b+x", 2);        break;
        case 0x15: return spc700_disasm_impl(3, "or    a, $*w+x", 2);        break;
        case 0x16: return spc700_disasm_impl(3, "or    a, $*w+y", 2);        break;
        case 0x17: return spc700_disasm_impl(2, "or    a, ($*b)+y", 2);      break;
        case 0x18: return spc700_disasm_impl(3, "or    $*b, #$*b", 2, 2);    break;
        case 0x19: return spc700_disasm_impl(1, "or    (x), (y)");           break;
        case 0x1a: return spc700_disasm_impl(2, "decw  $*b", 2);             break;
        case 0x1b: return spc700_disasm_impl(2, "asl   $*b+x", 2);           break;
        case 0x1c: return spc700_disasm_impl(1, "asl   a");                  break;
        case 0x1d: return spc700_disasm_impl(1, "dec   x");                  break;
        case 0x1e: return spc700_disasm_impl(3, "cmp   x, $*w", 2);          break;
        case 0x1f: return spc700_disasm_impl(3, "jmp   ($*w+x)", 2);         break;
        case 0x20: return spc700_disasm_impl(1, "clrp");                     break;
        case 0x21: return spc700_disasm_impl(1, "tcall2");                   break;
        case 0x22: return spc700_disasm_impl(2, "set1  $*b", 2);             break;
        case 0x23: return spc700_disasm_impl(3, "bbs1  $*b, $*r", 2, 2);     break;
        case 0x24: return spc700_disasm_impl(2, "and   a, $*b", 2);          break;
        case 0x25: return spc700_disasm_impl(3, "and   a, $*w", 2);          break;
        case 0x26: return spc700_disasm_impl(1, "and   a, (x)");             break;
        case 0x27: return spc700_disasm_impl(2, "and   a, ($*b+x)", 2);      break;
        case 0x28: return spc700_disasm_impl(2, "and   a, #$*b", 2);         break;
        case 0x29: return spc700_disasm_impl(3, "and   ($*b), ($*b)", 2, 2); break;
        case 0x2a: return spc700_disasm_impl(3, "or1   c, !($*m)", 2);       break;
        case 0x2b: return spc700_disasm_impl(2, "rol   $*b", 2);             break;
        case 0x2c: return spc700_disasm_impl(3, "rol   $*w", 2);             break;
        case 0x2d: return spc700_disasm_impl(1, "push  a");                  break;
        case 0x2e: return spc700_disasm_impl(3, "cbne  $*b, $*r", 2, 2);     break;
        case 0x2f: return spc700_disasm_impl(2, "bra   $*r", 2);             break;
        case 0x30: return spc700_disasm_impl(2, "bmi   $*r", 2);             break;
        case 0x31: return spc700_disasm_impl(1, "tcall 3");                  break;
        case 0x32: return spc700_disasm_impl(2, "clr1  $*b", 2);             break;
        case 0x33: return spc700_disasm_impl(3, "bbc1  $*b, $*r", 2, 2);     break;
        case 0x34: return spc700_disasm_impl(2, "and   a, $*b+x", 2);        break;
        case 0x35: return spc700_disasm_impl(3, "and   a, $*w+x", 2);        break;
        case 0x36: return spc700_disasm_impl(3, "and   a, $*w+y", 2);        break;
        case 0x37: return spc700_disasm_impl(2, "and   a, ($*b)+y", 2);      break;
        case 0x38: return spc700_disasm_impl(3, "and   $*b, #$*b", 2, 2);    break;
        case 0x39: return spc700_disasm_impl(1, "and   (x), (y)");           break;
        case 0x3a: return spc700_disasm_impl(2, "incw  $*b", 2);             break;
        case 0x3b: return spc700_disasm_impl(2, "rol   $*b+x", 2);           break;
        case 0x3c: return spc700_disasm_impl(1, "rol   a");                  break;
        case 0x3d: return spc700_disasm_impl(1, "inc   x");                  break;
        case 0x3e: return spc700_disasm_impl(2, "cmp   x, $*b", 2);          break;
        case 0x3f: return spc700_disasm_impl(3, "call  $*w", 2);             break;
        case 0x40: return spc700_disasm_impl(1, "setp");                     break;
        case 0x41: return spc700_disasm_impl(1, "tcall 4");                  break;
        case 0x42: return spc700_disasm_impl(2, "set2  $*b", 2);             break;
        case 0x43: return spc700_disasm_impl(3, "bbs2  $*b, $*r", 2, 2);     break;
        case 0x44: return spc700_disasm_impl(2, "eor   a, $*b", 2);          break;
        case 0x45: return spc700_disasm_impl(3, "eor   a, $*w", 2);          break;
        case 0x46: return spc700_disasm_impl(1, "eor   a, (x)");             break;
        case 0x47: return spc700_disasm_impl(2, "eor   a, ($*b+x)", 2);      break;
        case 0x48: return spc700_disasm_impl(2, "eor   a, #$*b", 2);         break;
        case 0x49: return spc700_disasm_impl(3, "eor   ($*b), ($*b)", 2, 2); break;
        case 0x4a: return spc700_disasm_impl(3, "and1  c, $*m", 2);          break;
        case 0x4b: return spc700_disasm_impl(2, "lsr   $*b", 2);             break;
        case 0x4c: return spc700_disasm_impl(3, "lsr   $*w", 2);             break;
        case 0x4d: return spc700_disasm_impl(1, "push  x");                  break;
        case 0x4e: return spc700_disasm_impl(3, "tclr1 $*w", 2);             break;
        case 0x4f: return spc700_disasm_impl(2, "pcall $*b", 2);             break;
        case 0x50: return spc700_disasm_impl(2, "bvc   $*r", 2);             break;
        case 0x51: return spc700_disasm_impl(1, "tcall 5");                  break;
        case 0x52: return spc700_disasm_impl(2, "clr2  $*b", 2);             break;
        case 0x53: return spc700_disasm_impl(3, "bbc2  $*b, $*r", 2, 2);     break;
        case 0x54: return spc700_disasm_impl(2, "eor   a, $*b+x", 2);        break;
        case 0x55: return spc700_disasm_impl(3, "eor   a, $*w+x", 2);        break;
        case 0x56: return spc700_disasm_impl(3, "eor   a, $*w+y", 2);        break;
        case 0x57: return spc700_disasm_impl(2, "eor   a, ($*b)+y", 2);      break;
        case 0x58: return spc700_disasm_impl(3, "eor   $*b, #$*b", 2, 2);    break;
        case 0x59: return spc700_disasm_impl(1, "eor   (x), (y)");           break;
        case 0x5a: return spc700_disasm_impl(2, "cmpw  ya, $*b", 2);         break;
        case 0x5b: return spc700_disasm_impl(2, "lsr   $*b+x", 2);           break;
        case 0x5c: return spc700_disasm_impl(1, "lsr   a");                  break;
        case 0x5d: return spc700_disasm_impl(1, "mov   x, a");               break;
        case 0x5e: return spc700_disasm_impl(3, "cmp   y, $*w", 2);          break;
        case 0x5f: return spc700_disasm_impl(3, "jmp   $*w", 2);             break;
        case 0x60: return spc700_disasm_impl(1, "clrc");                     break;
        case 0x61: return spc700_disasm_impl(1, "tcall 6");                  break;
        case 0x62: return spc700_disasm_impl(2, "set3  $*b", 2);             break;
        case 0x63: return spc700_disasm_impl(3, "bbs3  $*b, $*r", 2, 2);     break;
        case 0x64: return spc700_disasm_impl(2, "cmp   a, $*b", 2);          break;
        case 0x65: return spc700_disasm_impl(3, "cmp   a, $*w", 2);          break;
        case 0x66: return spc700_disasm_impl(1, "cmp   a, (x)");             break;
        case 0x67: return spc700_disasm_impl(2, "cmp   a, ($*b+x)", 2);      break;
        case 0x68: return spc700_disasm_impl(2, "cmp   a, #$*b", 2);         break;
        case 0x69: return spc700_disasm_impl(3, "cmp   ($*b), ($*b)", 2, 2); break;
        case 0x6a: return spc700_disasm_impl(3, "and1  c, !($*m)", 2);       break;
        case 0x6b: return spc700_disasm_impl(2, "ror   $*b", 2);             break;
        case 0x6c: return spc700_disasm_impl(3, "ror   $*w", 2);             break;
        case 0x6d: return spc700_disasm_impl(1, "push  y");                  break;
        case 0x6e: return spc700_disasm_impl(3, "dbnz  $*b, $*r", 2, 2);     break;
        case 0x6f: return spc700_disasm_impl(1, "ret");                      break;
        case 0x70: return spc700_disasm_impl(2, "bvs   $*r", 2);             break;
        case 0x71: return spc700_disasm_impl(1, "tcall 7");                  break;
        case 0x72: return spc700_disasm_impl(2, "clr3  $*b", 2);             break;
        case 0x73: return spc700_disasm_impl(3, "bbc3  $*b, $*r", 2, 2);     break;
        case 0x74: return spc700_disasm_impl(2, "cmp   a, $*b+x", 2);        break;
        case 0x75: return spc700_disasm_impl(3, "cmp   a, $*w+x", 2);        break;
        case 0x76: return spc700_disasm_impl(3, "cmp   a, $*w+y", 2);        break;
        case 0x77: return spc700_disasm_impl(2, "cmp   a, ($*b)+y", 2);      break;
        case 0x78: return spc700_disasm_impl(3, "cmp   $*b, #$*b", 2, 2);    break;
        case 0x79: return spc700_disasm_impl(1, "cmp   (x), (y)");           break;
        case 0x7a: return spc700_disasm_impl(2, "addw  ya, $*b", 2);         break;
        case 0x7b: return spc700_disasm_impl(2, "ror   $*b+x", 2);           break;
        case 0x7c: return spc700_disasm_impl(1, "ror   a");                  break;
        case 0x7d: return spc700_disasm_impl(1, "mov   a, x");               break;
        case 0x7e: return spc700_disasm_impl(2, "cmp   y, $*b", 2);          break;
        case 0x7f: return spc700_disasm_impl(1, "reti");                     break;
        case 0x80: return spc700_disasm_impl(1, "setc");                     break;
        case 0x81: return spc700_disasm_impl(1, "tcall 8");                  break;
        case 0x82: return spc700_disasm_impl(2, "set4  $*b", 2);             break;
        case 0x83: return spc700_disasm_impl(3, "bbs4  $*b, $*r", 2, 2);     break;
        case 0x84: return spc700_disasm_impl(2, "adc   a, $*b", 2);          break;
        case 0x85: return spc700_disasm_impl(3, "adc   a, $*w", 2);          break;
        case 0x86: return spc700_disasm_impl(1, "adc   a, (x)");             break;
        case 0x87: return spc700_disasm_impl(2, "adc   a, ($*b+x)", 2);      break;
        case 0x88: return spc700_disasm_impl(2, "adc   a, #$*b", 2);         break;
        case 0x89: return spc700_disasm_impl(3, "adc   ($*b), ($*b)", 2, 2); break;
        case 0x8a: return spc700_disasm_impl(3, "eor1  c, $*m", 2);          break;
        case 0x8b: return spc700_disasm_impl(2, "dec   $*b", 2);             break;
        case 0x8c: return spc700_disasm_impl(3, "dec   $*w", 2);             break;
        case 0x8d: return spc700_disasm_impl(2, "mov   y, #$*b", 2);         break;
        case 0x8e: return spc700_disasm_impl(1, "pop   psw");                break;
        case 0x8f: return spc700_disasm_impl(3, "mov   $*b, #$*b", 2, 2);    break;
        case 0x90: return spc700_disasm_impl(2, "bcc   $*r", 2);             break;
        case 0x91: return spc700_disasm_impl(1, "tcall 9");                  break;
        case 0x92: return spc700_disasm_impl(2, "clr4  $*b", 2);             break;
        case 0x93: return spc700_disasm_impl(3, "bbc4  $*b, $*r", 2, 2);     break;
        case 0x94: return spc700_disasm_impl(2, "adc   a, $*b+x", 2);        break;
        case 0x95: return spc700_disasm_impl(3, "adc   a, $*w+x", 2);        break;
        case 0x96: return spc700_disasm_impl(3, "adc   a, $*w+y", 2);        break;
        case 0x97: return spc700_disasm_impl(2, "adc   a, ($*b)+y", 2);      break;
        case 0x98: return spc700_disasm_impl(3, "adc   $*b, #$*b", 2, 2);    break;
        case 0x99: return spc700_disasm_impl(1, "adc   (x), (y)");           break;
        case 0x9a: return spc700_disasm_impl(2, "subw  ya, $*b", 2);         break;
        case 0x9b: return spc700_disasm_impl(2, "dec   $*b+x", 2);           break;
        case 0x9c: return spc700_disasm_impl(1, "dec   a");                  break;
        case 0x9d: return spc700_disasm_impl(1, "mov   x, sp");              break;
        case 0x9e: return spc700_disasm_impl(1, "div   ya, x");              break;
        case 0x9f: return spc700_disasm_impl(1, "xcn   a");                  break;
        case 0xa0: return spc700_disasm_impl(1, "ei");                       break;
        case 0xa1: return spc700_disasm_impl(1, "tcall 10");                 break;
        case 0xa2: return spc700_disasm_impl(2, "set5  $*b", 2);             break;
        case 0xa3: return spc700_disasm_impl(3, "bbs5  $*b, $*r", 2, 2);     break;
        case 0xa4: return spc700_disasm_impl(2, "sbc   a, $*b", 2);          break;
        case 0xa5: return spc700_disasm_impl(3, "sbc   a, $*w", 2);          break;
        case 0xa6: return spc700_disasm_impl(1, "sbc   a, (x)");             break;
        case 0xa7: return spc700_disasm_impl(2, "sbc   a, ($*b+x)", 2);      break;
        case 0xa8: return spc700_disasm_impl(2, "sbc   a, #$*b", 2);         break;
        case 0xa9: return spc700_disasm_impl(3, "sbc   ($*b), ($*b)", 2, 2); break;
        case 0xaa: return spc700_disasm_impl(3, "mov1  c, $*m", 2);          break;
        case 0xab: return spc700_disasm_impl(2, "inc   $*b", 2);             break;
        case 0xac: return spc700_disasm_impl(3, "inc   $*w", 2);             break;
        case 0xad: return spc700_disasm_impl(2, "cmp   y, #$*b", 2);         break;
        case 0xae: return spc700_disasm_impl(1, "pop   a");                  break;
        case 0xaf: return spc700_disasm_impl(1, "mov   (x)+, a");            break;
        case 0xb0: return spc700_disasm_impl(2, "bcs   $*r", 2);             break;
        case 0xb1: return spc700_disasm_impl(1, "tcall 11");                 break;
        case 0xb2: return spc700_disasm_impl(2, "clr5  $*b", 2);             break;
        case 0xb3: return spc700_disasm_impl(3, "bbc5  $*b, $*r", 2, 2);     break;
        case 0xb4: return spc700_disasm_impl(2, "sbc   a, $*b+x", 2);        break;
        case 0xb5: return spc700_disasm_impl(3, "sbc   a, $*w+x", 2);        break;
        case 0xb6: return spc700_disasm_impl(3, "sbc   a, $*w+y", 2);        break;
        case 0xb7: return spc700_disasm_impl(2, "sbc   a, ($*b)+y", 2);      break;
        case 0xb8: return spc700_disasm_impl(3, "sbc   $*b, #$*b", 2, 2);    break;
        case 0xb9: return spc700_disasm_impl(1, "sbc   (x), (y)");           break;
        case 0xba: return spc700_disasm_impl(2, "movw  ya, $*b", 2);         break;
        case 0xbb: return spc700_disasm_impl(2, "inc   $*b+x", 2);           break;
        case 0xbc: return spc700_disasm_impl(1, "inc   a");                  break;
        case 0xbd: return spc700_disasm_impl(1, "mov   sp, x");              break;
        case 0xbe: return spc700_disasm_impl(1, "das   a");                  break;
        case 0xbf: return spc700_disasm_impl(1, "mov   a, (x)+");            break;
        case 0xc0: return spc700_disasm_impl(1, "di");                       break;
        case 0xc1: return spc700_disasm_impl(1, "tcall 12");                 break;
        case 0xc2: return spc700_disasm_impl(2, "set6  $*b", 2);             break;
        case 0xc3: return spc700_disasm_impl(3, "bbs6  $*b, $*r", 2, 2);     break;
        case 0xc4: return spc700_disasm_impl(2, "mov   $*b, a", 2);          break;
        case 0xc5: return spc700_disasm_impl(3, "mov   $*w, a", 2);          break;
        case 0xc6: return spc700_disasm_impl(1, "mov   (x), a");             break;
        case 0xc7: return spc700_disasm_impl(2, "mov   ($*b+x), a", 2);      break;
        case 0xc8: return spc700_disasm_impl(2, "cmp   x, #$*b", 2);         break;
        case 0xc9: return spc700_disasm_impl(3, "mov   $*w, x", 2);          break;
        case 0xca: return spc700_disasm_impl(3, "mov1  $*m, c", 2);          break;
        case 0xcb: return spc700_disasm_impl(2, "mov   $*b, y", 2);          break;
        case 0xcc: return spc700_disasm_impl(3, "mov   $*w, y", 2);          break;
        case 0xcd: return spc700_disasm_impl(2, "mov   x, #$*b", 2);         break;
        case 0xce: return spc700_disasm_impl(1, "pop   x");                  break;
        case 0xcf: return spc700_disasm_impl(1, "mul   ya");                 break;
        case 0xd0: return spc700_disasm_impl(2, "bne   $*r", 2);             break;
        case 0xd1: return spc700_disasm_impl(1, "tcall 13");                 break;
        case 0xd2: return spc700_disasm_impl(2, "clr6  $*b", 2);             break;
        case 0xd3: return spc700_disasm_impl(3, "bbc6  $*b, $*r", 2, 2);     break;
        case 0xd4: return spc700_disasm_impl(2, "mov   $*b+x, a", 2);        break;
        case 0xd5: return spc700_disasm_impl(3, "mov   $*w+x, a", 2);        break;
        case 0xd6: return spc700_disasm_impl(3, "mov   $*w+y, a", 2);        break;
        case 0xd7: return spc700_disasm_impl(2, "mov   ($*b)+y, a", 2);      break;
        case 0xd8: return spc700_disasm_impl(2, "mov   $*b, x", 2);          break;
        case 0xd9: return spc700_disasm_impl(2, "mov   $*b+y, x", 2);        break;
        case 0xda: return spc700_disasm_impl(2, "movw  $*b, ya", 2);         break;
        case 0xdb: return spc700_disasm_impl(2, "mov   $*b+x, y", 2);        break;
        case 0xdc: return spc700_disasm_impl(1, "dec   y");                  break;
        case 0xdd: return spc700_disasm_impl(1, "mov   a, y");               break;
        case 0xde: return spc700_disasm_impl(3, "cbne  $*b+x, $*r", 2, 2);   break;
        case 0xdf: return spc700_disasm_impl(1, "daa   a");                  break;
        case 0xe0: return spc700_disasm_impl(1, "clrv");                     break;
        case 0xe1: return spc700_disasm_impl(1, "tcall 14");                 break;
        case 0xe2: return spc700_disasm_impl(2, "set7  $*b", 2);             break;
        case 0xe3: return spc700_disasm_impl(3, "bbs7  $*b, $*r", 2, 2);     break;
        case 0xe4: return spc700_disasm_impl(2, "mov   a, $*b", 2);          break;
        case 0xe5: return spc700_disasm_impl(3, "mov   a, $*w", 2);          break;
        case 0xe6: return spc700_disasm_impl(1, "mov   a, (x)");             break;
        case 0xe7: return spc700_disasm_impl(2, "mov   a, ($*b+x)", 2);      break;
        case 0xe8: return spc700_disasm_impl(2, "mov   a, #$*b", 2);         break;
        case 0xe9: return spc700_disasm_impl(3, "mov   x, $*w", 2);          break;
        case 0xea: return spc700_disasm_impl(3, "not1  $*m", 2);             break;
        case 0xeb: return spc700_disasm_impl(2, "mov   y, $*b", 2);          break;
        case 0xec: return spc700_disasm_impl(3, "mov   y, $*w", 2);          break;
        case 0xed: return spc700_disasm_impl(1, "notc");                     break;
        case 0xee: return spc700_disasm_impl(1, "pop   y");                  break;
        case 0xef: return spc700_disasm_impl(1, "sleep");                    break;
        case 0xf0: return spc700_disasm_impl(2, "beq   $*r", 2);             break;
        case 0xf1: return spc700_disasm_impl(1, "tcall 15");                 break;
        case 0xf2: return spc700_disasm_impl(2, "clr7  $*b", 2);             break;
        case 0xf3: return spc700_disasm_impl(3, "bbc7  $*b, $*r", 2, 2);     break;
        case 0xf4: return spc700_disasm_impl(2, "mov   a, $*b+x", 2);        break;
        case 0xf5: return spc700_disasm_impl(3, "mov   a, $*w+x", 2);        break;
        case 0xf6: return spc700_disasm_impl(3, "mov   a, $*w+y", 2);        break;
        case 0xf7: return spc700_disasm_impl(2, "mov   a, ($*b)+y", 2);      break;
        case 0xf8: return spc700_disasm_impl(2, "mov   x, $*b", 2);          break;
        case 0xf9: return spc700_disasm_impl(2, "mov   x, $*b+y", 2);        break;
        case 0xfa: return spc700_disasm_impl(3, "mov   ($*b), ($*b)", 2, 2); break;
        case 0xfb: return spc700_disasm_impl(2, "mov   y, $*b+x", 2);        break;
        case 0xfc: return spc700_disasm_impl(1, "inc   y");                  break;
        case 0xfd: return spc700_disasm_impl(1, "mov   y, a");               break;
        case 0xfe: return spc700_disasm_impl(2, "dbnz  y, $*r", 2);          break;
        case 0xff: return spc700_disasm_impl(1, "stop");                     break;
    }
}