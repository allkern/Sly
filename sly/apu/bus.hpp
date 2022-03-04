#pragma once

#include "../global.hpp"

#include "ipl.hpp"
#include "aram.hpp"
#include "ports.hpp"
#include "registers.hpp"
#include "timers.hpp"

namespace snes {
    namespace apu {
        namespace bus {
            u8 read(u16 addr) {
                // _log(debug, "SPU bus read addr=%04x", addr);
                if ((addr >= PG0_BEGIN) && (addr <= PG0_END)) return aram::read(addr);
                
                switch (addr) {
                    case 0xf1: return control;
                    case 0xfa: case 0xfb: case 0xfc: return timers::tdiv[addr - 0xfa];
                    case 0xfd: case 0xfe: case 0xff: {
                        u8 out = timers::tout[addr - 0xfd];

                        timers::tout[addr - 0xfd] = 0;
                        
                        return out;
                    }
                }

                if ((addr >= 0xf4     ) && (addr <= 0xf7   )) return ports::read((u8)(addr & 0xff));
                if ((addr >= PG1_BEGIN) && (addr <= PG1_END)) return aram::read(addr);
                if ((addr >= RAM_BEGIN) && (addr <= RAM_END)) return aram::read(addr);
                if ((addr >= IPL_BEGIN) && (addr <= IPL_END)) return ipl::read(addr);

                return 0x0;
            }

            u16 read16(u16 addr) { return read(addr) | (read(addr + 1) << 8); }

            void write(u16 addr, u8 value) {
                if ((addr >= PG0_BEGIN) && (addr <= PG0_END)) { aram::write(addr, value); return; }
                
                switch (addr) {
                    case 0xf1: {
                        control = value;

                        for (int t = 0; t < 3; t++) {
                            timers::tout[t] = 0;
                            timers::t[t] = 0;
                        }

                        return;
                    }
                    case 0xfa: case 0xfb: case 0xfc: { timers::tdiv[addr - 0xfa] = value; return; }
                    //case 0xfd: case 0xfe: case 0xff: { timers::tout[addr - 0xfd] = 0; return; }
                }

                if ((addr >= 0xf4     ) && (addr <= 0xf7   )) { ports::write((u8)(addr & 0xff), value); return; }
                if ((addr >= PG1_BEGIN) && (addr <= PG1_END)) { aram::write(addr, value); return; }
                if ((addr >= RAM_BEGIN) && (addr <= RAM_END)) { aram::write(addr, value); return; }
            }

            void write16(u16 addr, u16 value) { write(addr, value & 0xff); write(addr + 1, (value >> 8) & 0xff); }
        }
    }
}