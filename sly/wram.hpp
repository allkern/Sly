#pragma once

#include "global.hpp"

#include <array>

namespace snes {
    namespace wram {
        std::array <u8, 0x20000> buf;

        void init() {
            srand(time(NULL));

            //for (u8& b : buf) b = rand() & 0xff;
        }

        u8 read(u32 addr, int size = 1) {
            u8 bank = (addr >> 16) & 0xff,
               page = (addr >> 8) & 0xff;
            u16 pb = addr & 0xffff;

            if (BANKS(0x7e, 0x7f)) return buf.at(((bank & 0x1) << 16) | pb);
            if (BANKS(0x00, 0x3f)) return buf.at(pb);
            if (BANKS(0x80, 0xbf)) return buf.at(pb);

            return 0xff;
        }

        void write(u32 addr, u8 value) {
            u8 bank = (addr >> 16) & 0xff,
               page = (addr >> 8) & 0xff;
            u16 pb = addr & 0xffff;

            if (BANKS(0x7e, 0x7f)) { buf.at(((bank & 0x1) << 16) | pb) = value; return; }
            if (BANKS(0x00, 0x3f)) { buf.at(pb) = value; return; }
            if (BANKS(0x80, 0xbf)) { buf.at(pb) = value; return; }
        }
    }
}