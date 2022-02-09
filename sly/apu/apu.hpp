#pragma once

namespace snes {
    namespace apu {
        bool ready = false,
             ready2 = false,
             ready3 = false,
             ready4 = false,
             ready5 = false,
             ready6 = false,
             ready7 = false,
             ready8 = false,
             ready9 = false,
             readya = false;

        u8 r[4] = { 0 };

        u8 read(u16 addr) {
            if ((addr == 0x2140) || (addr == 0x2141)) {
                if (ready2) { return r[addr & 0xf]; }
                if (ready) { ready = false; return 0xcc; }
                if (addr == 0x2140) return 0xaa;
                if (addr == 0x2141) return 0xbb;
            }
            if (addr == 0x2143) { return r[3]; }

            return 0xff;
        }

        void write(u16 addr, u8 value) {
            if ((addr == 0x2140) && (value == 0xcc)) ready = true;
            if ((addr == 0x2140) && (value)) ready2 = true;

            if (ready2) r[(addr & 0xf) & 0x3] = value;
            if (addr == 0x2143) r[3] = value;
        }

    }
}