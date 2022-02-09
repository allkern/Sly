#pragma once

#include "../global.hpp"

namespace snes {
    namespace dma {
        typedef u8 (*bus_read_t)(u32);
        typedef void (*bus_write_t)(u32, u8);

        bus_read_t bus_read;
        bus_write_t bus_write;

        struct channel_t {
            u8 control,
               dest,
               srcl,
               srch,
               srcb,
               sizel,
               sizeh,
               pad[9];
        };

        channel_t channel[8];

        inline u16 get_channel_count(int ch) {
            return (channel[ch].sizeh << 8) | channel[ch].sizel;
        }

        inline void decrement_channel_count(int ch) {
            u16 count = get_channel_count(ch) - 1;

            channel[ch].sizeh = (count >> 8) & 0xff;
            channel[ch].sizel = count & 0xff;
        }

        inline u32 get_channel_source(int ch) {
            return (channel[ch].srcb << 16) | (channel[ch].srch << 8) | channel[ch].srcl;
        }
        
        inline u16 get_channel_dest(int ch) {
            return 0x2100 | channel[ch].dest;
        }

        void init(bus_read_t rd, bus_write_t wr) {
            bus_read = rd;
            bus_write = wr;
        }

        bool write_byte(int ch, bool ppu_to_cpu, bool fixed, bool increment, u16 dest, u32& source, int offset) {
            u8 byte = bus_read(ppu_to_cpu ? (dest + offset) : source);
            
            bus_write(ppu_to_cpu ? source : (dest + offset), byte);

            if (!fixed) {
                u32 bank = source & 0xff0000;
                
                // Adjust AAddress
                if (increment) source -= 1; else source += 1;

                // Restore bank
                source &= 0xffff;
                source |= bank;
            }

            decrement_channel_count(ch);

            return get_channel_count(ch);
        }

        void do_dma(int ch) {
            u8 control = channel[ch].control;

            bool ppu_to_cpu = control & 0x80,
                 increment  = control & 0x10,
                 fixed      = control & 0x8;
            
            u32 source = get_channel_source(ch);
            u16 dest = get_channel_dest(ch);

            // Block WMDATA DMAs
            if (dest == 0x2180) return;

            _log(debug, "Doing DMA: mode=%u, ppu_to_cpu=%u, increment=%u, fixed=%u, source=%06x, dest=%04x, size=%u (%04x)",
                control & 0x7,
                ppu_to_cpu,
                increment,
                fixed,
                source,
                dest,
                get_channel_count(0),
                get_channel_count(0)
            );

            // Use do to emulate the count wrapping behavior
            do {
                switch (control & 0x7) {
                    case 0: { // 1 register write once
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                    } break;
                    case 1: { // 2 registers write once
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                    } break;
                    case 2: { // 1 register write twice
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                    } break;
                    case 3: { // 2 registers write twice each
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                    } break;
                    case 4: { // 4 registers write once
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 2)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 3)) break;
                    } break;
                    case 5: { // 2 registers write twice interleaved
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                    } break;
                    case 6: { // 1 register write twice
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                    } break;
                    case 7: { // 2 registers write twice each
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 0)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                        if (!write_byte(ch, ppu_to_cpu, fixed, increment, dest, source, 1)) break;
                    } break;
                }
            } while (get_channel_count(ch));
        }

        u8 read(u16 addr) {
            if ((addr >= 0x4300) && (addr <= 0x437f)) {
                // _log(debug, "DMA channel %u write reg %u %02x", (addr >> 4) & 0xf, addr & 0xf, value);
                u8* ptr = reinterpret_cast<u8*>(&channel[(addr >> 4) & 0xf]);

                return *(ptr + (addr & 0xf));
            }
            return 0xff;
        }

        void write(u16 addr, u8 value) {
            if (addr == 0x420b) {
                if (!value) return;
                // _log(debug, "DMA fire %02x", value);

                for (int c = 0; c < 8; c++) if (value & (1 << c)) do_dma(c);
            }

            if ((addr >= 0x4300) && (addr <= 0x437f)) {
                // _log(debug, "DMA channel %u write reg %u %02x", (addr >> 4) & 0xf, addr & 0xf, value);
                u8* ptr = reinterpret_cast<u8*>(&channel[(addr >> 4) & 0xf]);

                *(ptr + (addr & 0xf)) = value;
            }
        }
    }
}