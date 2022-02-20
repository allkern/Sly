#pragma once

#include "../global.hpp"

#include "ipl.hpp"
#include "aram.hpp"
#include "ports.hpp"

namespace snes {
    namespace apu {
        namespace bus {
            u8 read(u16 addr) {
                // _log(debug, "SPU bus read addr=%04x", addr);
                if ((addr >= PG0_BEGIN) && (addr <= PG0_END)) return aram::read(addr);
                if ((addr >= 0xf0     ) && (addr <= 0xff   )) return ports::read((u8)(addr & 0xff));
                if ((addr >= PG1_BEGIN) && (addr <= PG1_END)) return aram::read(addr);
                if ((addr >= RAM_BEGIN) && (addr <= RAM_END)) return aram::read(addr);
                if ((addr >= IPL_BEGIN) && (addr <= IPL_END)) return ipl::read(addr);

                return 0x0;
            }

            u16 read16(u16 addr) { return read(addr) | (read(addr + 1) << 8); }

            void write(u16 addr, u8 value) {
                if ((addr >= PG0_BEGIN) && (addr <= PG0_END)) { aram::write(addr, value); return; }
                if ((addr >= 0xf0     ) && (addr <= 0xff   )) { ports::write((u8)(addr & 0xff), value); return; }
                if ((addr >= PG1_BEGIN) && (addr <= PG1_END)) { aram::write(addr, value); return; }
                if ((addr >= RAM_BEGIN) && (addr <= RAM_END)) { aram::write(addr, value); return; }
            }

            void write16(u16 addr, u16 value) { write(addr, value & 0xff); write(addr + 1, (value >> 8) & 0xff); }
        }
    }
}