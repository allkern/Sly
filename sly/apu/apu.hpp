#pragma once

#include "ports.hpp"

namespace snes {
    namespace apu {
        u8 read(u16 addr) {
            return ports::read(addr);
        }

        void write(u16 addr, u8 value) {
            ports::write(addr, value);
        }
    }
}