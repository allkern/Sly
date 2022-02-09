#pragma once

#include "dma/dma.hpp"
#include "controller.hpp"

#include "global.hpp"

namespace snes {
    namespace internal {
        void write(u16 addr, u8 value) {
            //_log(debug, "CPU regs write addr=%04x, value=%02x", addr, value);
            switch (addr) {
                case 0x4200: { nmitimen = value; return; } break;
                case 0x420b: { dma::write(addr, value); return; } break;
            }

            if ((addr >= 0x4300) && (addr <= 0x437f)) { dma::write(addr, value); return; }

            //_log(debug, "unhandled CPU regs write %04x:%02x", addr, value);
        }

        u8 read(u16 addr) {
            //_log(debug, "CPU regs read addr=%04x", addr);
            switch (addr) {
                case 0x4200: { }
                case 0x420b: { return dma::read(addr); }
                case 0x4210: { return fired_nmi << 7; }
                case 0x4212: { /*start_logging = true;*/ return 0x82; }
            }

            if ((addr >= 0x4300) && (addr <= 0x437f)) { return dma::read(addr); }
            if ((addr >= 0x4218) && (addr <= 0x421f)) { return controllers::read(addr); }

            //_log(debug, "unhandled CPU regs read %04x", addr);
            return 0x00;
        }
    }
}