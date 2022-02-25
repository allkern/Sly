#pragma once

#define BANKS(low, high) ((bank >= low) && (bank <= high))

bool stop_cpu = false, start_logging = false;

#include "types.hpp"
#include "../log.hpp"

namespace snes {
    namespace cpu {
        snes::u64 total_cycles = 0;
    }
}

snes::u8 nmitimen = 0x0;
bool fired_nmi = 0x0;
bool dump_vram = false;