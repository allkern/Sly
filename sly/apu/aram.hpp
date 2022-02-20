#pragma once

#include "../global.hpp"

#include <array>

#define PG0_BEGIN 0x0000
#define PG0_END   0x00ef
#define PG1_BEGIN 0x0100
#define PG1_END   0x01ff
#define RAM_BEGIN 0x0200
#define RAM_END   0xffbf

namespace snes {
    namespace apu {
        namespace aram {
            std::array <u8, 0xf0>   page0;
            std::array <u8, 0x100>  page1;
            std::array <u8, 0xfdc0> main;

            u8 read(u16 addr) {
                if ((addr <= 0xef)) return page0[addr];
                if ((addr >= 0x100) && (addr <= 0x1ff)) return page1[addr];
                if ((addr >= 0x200) && (addr <= 0xffbf)) return main[addr];

                return 0x0;
            }

            void write(u16 addr, u8 value) {
                //_log(debug, "ARAM write addr=%04x, value=%02x", addr, value);
                if ((addr <= 0xef)) { page0[addr] = value; return; }
                if ((addr >= 0x100) && (addr <= 0x1ff)) { page1[addr] = value; return; }
                if ((addr >= 0x200) && (addr <= 0xffbf)) { main[addr] = value; return; }
            }
        }
    }
}