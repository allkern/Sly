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

            if (BANKS(0x7e, 0x7f)                  ) { /*_log(debug, "addr=%06x, WRAM read", addr);*/ return wram::read(addr); } /* WRAM read */
            if (BANKS(0x00, 0x3f) && (page <= 0x1f)) { /*_log(debug, "addr=%06x, WRAM mirror", addr);*/ return wram::read(addr); } /* WRAM mirror read */
            if (BANKS(0x80, 0xbf) && (page <= 0x1f)) { /*_log(debug, "addr=%06x, WRAM mirror", addr);*/ return wram::read(addr); } /* WRAM mirror read */
            if (BANKS(0x00, 0x3f) && (page == 0x21)) { /*_log(debug, "addr=%06x, PPU read", addr);*/ return ppu::read(addr); } /* PPU read */
            if (BANKS(0x00, 0x3f) && (page == 0x40)) { /*_log(debug, "addr=%06x, JOYP read", addr);*/ return 0xff; } /* JOYP read */
            if (BANKS(0x00, 0x3f) && (page == 0x42)) { /*_log(debug, "addr=%06x, CPU read", addr);*/ return internal::read(addr); } /* CPU read */
            if (BANKS(0x00, 0x3f) && (page == 0x43)) { /*_log(debug, "addr=%06x, CPU read", addr);*/ return internal::read(addr); } /* CPU read */
            if (BANKS(0x80, 0xbf) && (page <= 0x7f)) { /*_log(debug, "addr=%06x, AUX read", addr);*/ return 0xff; } /* AUX read */
            if (BANKS(0x00, 0x3f) && (page <= 0x7f)) { /*_log(debug, "addr=%06x, AUX read", addr);*/ return 0xff; } /* AUX read */

            //_log(debug, "addr=%06x, cart read", addr);
            // ROMSEL
            return cart::read(addr);
        }

        void write8(u32 addr, u8 value) {
            u8 bank = (addr >> 16) & 0xff,
               page = (addr >> 8) & 0xff;
            u16 pb = addr & 0xffff;

            if (BANKS(0x7e, 0x7f)                  ) { /*_log(debug, "addr=%06x, WRAM write", addr); */ wram::write(addr, value); return; } /* WRAM read */
            if (BANKS(0x00, 0x3f) && (page <= 0x1f)) { /*_log(debug, "addr=%06x, WRAM write", addr); */ wram::write(addr, value); return; } /* WRAM mirror read */
            if (BANKS(0x80, 0xbf) && (page <= 0x1f)) { /*_log(debug, "addr=%06x, WRAM write", addr); */ wram::write(addr, value); return; } /* WRAM mirror read */
            if (BANKS(0x00, 0x3f) && (page == 0x21)) { /*_log(debug, "addr=%06x, PPU write", addr);*/ ppu::write(addr, value); return; } /* PPU read */
            if (BANKS(0x00, 0x3f) && (page == 0x40)) { /*_log(debug, "addr=%06x, JOYP write", addr); */ return; } /* JOYP read */
            if (BANKS(0x00, 0x3f) && (page == 0x42)) { /*_log(debug, "addr=%06x, CPU write", addr); */ internal::write(addr, value); return; } /* CPU read */
            if (BANKS(0x00, 0x3f) && (page == 0x43)) { /*_log(debug, "addr=%06x, CPU write", addr); */ internal::write(addr, value); return; } /* CPU read */
            if (BANKS(0x80, 0xbf) && (page <= 0x7f)) { /*_log(debug, "addr=%06x, AUX write", addr); */ return; } /* AUX read */
            if (BANKS(0x00, 0x3f) && (page <= 0x7f)) { /*_log(debug, "addr=%06x, AUX write", addr); */ return; } /* AUX read */

            //_log(debug, "addr=%06x, cart write", addr);

            // ROMSEL
            return cart::write(addr, value);
        }

        void write16(u32 addr, u16 value) {
            write8(addr    , value        & 0xff);
            write8(addr + 1, (value >> 8) & 0xff);
        }

        u16 read16(u32 addr, bool wrap = false) {
            // if (wrap) {
            //     u8 bank = addr & 0xff0000;
            //     u16 addr1 = (addr + 1) & 0xffff;

            //     return (read8(addr) << 8) | read8(bank | addr1);
            // } else {
            return read8(addr) | (read8(addr + 1) << 8);
            //}
        }

        u32 read24(u32 addr, bool wrap = false) {
            // if (wrap) {
            //     u8 bank = addr & 0xff0000;
            //     u16 addr2 = (addr + 2) & 0xffff;

            //     return (read8(bank | addr2) << 16) | read16(addr);
            // } else {
                //if (addr == 0x1a03) _log(debug, "%02x, %02x, %02x", bus::read8(addr-1), bus::read8(addr), bus::read8(addr+1)); 
            return (u32)read8(addr) | ((u32)read8(addr + 1) << 8) | ((u32)read8(addr + 2) << 16);
            //}
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