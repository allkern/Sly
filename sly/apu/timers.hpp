#pragma once

#include "../global.hpp"
#include "registers.hpp"

namespace snes {
    namespace apu {
        namespace timers {
            u8 tdiv[3], tout[3], t[3];

            int f8khz_counter = 0,
                f64khz_counter = 0;
            
            enum timer_freq_t {
                F8KHZ = 128 << 0,
                F64KHZ = 16 << 0
            };

            int get_timer_increments(int last_cycles, int clocks) {
                int& counter = clocks == F8KHZ ? f8khz_counter : f64khz_counter;

                counter += last_cycles;

                if (counter <= clocks) {
                    return 0;
                }

                if (counter == clocks) {
                    counter = 0;

                    return 1;
                }

                if (counter >= clocks) {
                    int increments = (int)std::floor((double)counter / (double)clocks);

                    counter -= clocks * increments;

                    return increments;
                }
 
                return 0;
            }

            void tick(int last_cycles) {
                int f8khz_increments = get_timer_increments(last_cycles, F8KHZ),
                    f64khz_increments = get_timer_increments(last_cycles, F64KHZ);

                if (control & 0x1) t[0] += f8khz_increments;
                if (control & 0x2) t[1] += f8khz_increments;
                if (control & 0x4) t[2] += f64khz_increments;
                
                //_log(debug, "last_cycles=%u, f8khzi=%u, f64khzi=%u, t0out=%02x, t1out=%02x, t2out=%02x",
                //    last_cycles,
                //    f8khz_increments,
                //    f64khz_increments,
                //    tout[0],
                //    tout[1],
                //    tout[2]
                //);

                if ((t[0] == tdiv[0]) && (control & 0x1)) { tout[0]++; t[0] = 0; }
                if ((t[1] == tdiv[1]) && (control & 0x2)) { tout[1]++; t[1] = 0; }
                if ((t[2] == tdiv[2]) && (control & 0x4)) { tout[2]++; t[2] = 0; }

                tout[0] &= 0xf;
                tout[1] &= 0xf;
                tout[2] &= 0xf;
            }
        }
    }
}

