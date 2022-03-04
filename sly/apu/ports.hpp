#pragma once

#include "../global.hpp"

namespace snes {
    namespace apu {
        namespace ports {
            u8 port0i,
               port0o,
               port1i,
               port1o,
               port2i,
               port2o,
               port3i,
               port3o;
            
            // cpu
            u8 read(u16 addr) {
                //start_logging = true;
                //_log(debug, "CPU<-SPC port read addr=%04x, port0=%02x, port1=%02x, port2=%02x, port3=%02x", addr, port0o, port1o, port2o, port3o);
                switch (addr) {
                    case 0x2140: return port0o;
                    case 0x2141: return port1o;
                    case 0x2142: return port2o;
                    case 0x2143: return port3o;
                }

                _log(debug, "spu unhandled cpu port read %04x", addr);

                return 0x0;
            }
            
            void write(u16 addr, u8 value) {
                //_log(debug, "SPC<-CPU port write %02x addr=%04x", value, addr);
                switch (addr) {
                    case 0x2140: { port0i = value; return; }
                    case 0x2141: { port1i = value; return; }
                    case 0x2142: { port2i = value; return; }
                    case 0x2143: { port3i = value; return; }
                }
            }

            // apu
            u8 read(u8 addr) {
                //_log(debug, "SPC<-CPU read port %u (%02x)", addr - 0xf4, addr);
                switch (addr) {
                    case 0xf4: return port0i;
                    case 0xf5: return port1i;
                    case 0xf6: return port2i;
                    case 0xf7: return port3i;
                }

                //_log(debug, "spu unhandled port read %04x", addr);

                return 0x0;
            }
            
            void write(u8 addr, u8 value) {
                //_log(debug, "CPU<-SPU write %02x to port %u (%02x)", value, addr - 0xf4, addr);
                switch (addr) {
                    case 0xf4: { port0o = value; return; }
                    case 0xf5: { port1o = value; return; }
                    case 0xf6: { port2o = value; return; }
                    case 0xf7: { port3o = value; return; }
                }

                //_log(debug, "spu unhandled port write %04x, value %02x", addr, value);
            }
        }
    }
}