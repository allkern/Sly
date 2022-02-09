#pragma once

#include "global.hpp"

#include "cart/cart.hpp"
#include "wram.hpp"
#include "ppu/ppu.hpp"
#include "internal_regs.hpp"

namespace snes {
    namespace bus {
        u8 mdr = 0x0;

        u8 read8(u32 addr) {
            u8 bank = (addr >> 16) & 0xff,
               page = (addr >> 8) & 0xff;
            u16 pb = addr & 0xffff;

            if (BANKS(0x7e, 0x7f)                  ) { return wram::read(addr); }
            if (BANKS(0x00, 0x3f) && (page <= 0x1f)) { return wram::read(addr); }
            if (BANKS(0x80, 0xbf) && (page <= 0x1f)) { return wram::read(addr); }
            if (BANKS(0x00, 0x3f) && (page == 0x21)) { return ppu::read(addr); }
            if (BANKS(0x80, 0xbf) && (page == 0x21)) { return ppu::read(addr & 0xffff); }
            if (BANKS(0x00, 0x3f) && (page == 0x40)) { return 0xff; } /* JOYP read */
            if (BANKS(0x00, 0x3f) && (page == 0x42)) { return internal::read(addr); }
            if (BANKS(0x00, 0x3f) && (page == 0x43)) { return internal::read(addr); }
            if (BANKS(0x80, 0xbf) && (page == 0x42)) { return internal::read(addr & 0xffff); }
            if (BANKS(0x80, 0xbf) && (page == 0x43)) { return internal::read(addr & 0xffff); }
            if (BANKS(0x80, 0xbf) && (page <= 0x7f)) { return 0xff; } /* AUX read */
            if (BANKS(0x00, 0x3f) && (page <= 0x7f)) { return 0xff; } /* AUX read */

            // ROMSEL
            return cart::read(addr);
        }

        void write8(u32 addr, u8 value) {
            u8 bank = (addr >> 16) & 0xff,
               page = (addr >> 8) & 0xff;
            u16 pb = addr & 0xffff;

            if (BANKS(0x7e, 0x7f)                  ) { wram::write(addr, value); return; }
            if (BANKS(0x00, 0x3f) && (page <= 0x1f)) { wram::write(addr, value); return; }
            if (BANKS(0x80, 0xbf) && (page <= 0x1f)) { wram::write(addr, value); return; }
            if (BANKS(0x00, 0x3f) && (page == 0x21)) { ppu::write(addr, value); return; }
            if (BANKS(0x80, 0xbf) && (page == 0x21)) { ppu::write(addr & 0xffff, value); return; }
            if (BANKS(0x00, 0x3f) && (page == 0x40)) { return; } /* JOYP write */
            if (BANKS(0x00, 0x3f) && (page == 0x42)) { internal::write(addr, value); return; }
            if (BANKS(0x00, 0x3f) && (page == 0x43)) { internal::write(addr, value); return; }
            if (BANKS(0x80, 0xbf) && (page == 0x42)) { internal::write(addr & 0xffff, value); return; }
            if (BANKS(0x80, 0xbf) && (page == 0x43)) { internal::write(addr & 0xffff, value); return; }
            if (BANKS(0x80, 0xbf) && (page <= 0x7f)) { return; } /* AUX write */
            if (BANKS(0x00, 0x3f) && (page <= 0x7f)) { return; } /* AUX write */

            // ROMSEL
            return cart::write(addr, value);
        }

        void write16(u32 addr, u16 value) {
            write8(addr    , value        & 0xff);
            write8(addr + 1, (value >> 8) & 0xff);
        }

        u16 read16(u32 addr, bool wrap = false) {
            return read8(addr) | (read8(addr + 1) << 8);
        }

        u32 read24(u32 addr, bool wrap = false) {
            return (u32)read8(addr) | ((u32)read8(addr + 1) << 8) | ((u32)read8(addr + 2) << 16);
        }

        u16 read(u32 addr, bool mode, bool wrap = false) {
            return mode ? read8(addr) : read16(addr, wrap);
        }

        void write24(u32 addr, u32 value) {
            write8(addr + 2, (value >> 16) & 0xff);
            write8(addr + 1, (value >> 8 ) & 0xff);
            write8(addr + 0, (value >> 0 ) & 0xff);
        }

        void write(u32 addr, u16 value, bool mode) {
            if (mode) write8(addr, value & 0xff); else write16(addr, value); 
        }
    }
}