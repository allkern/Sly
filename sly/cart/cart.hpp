#pragma once

#include <fstream>
#include <vector>

#include "../global.hpp"

namespace snes {
    namespace cart {
        std::vector <u8> rom, sram;

        enum rom_type_t {
            ROM_UNKNOWN = 0x0,
            ROM_LOROM = 0x20,
            ROM_HIROM = 0x31
        } rom_type = ROM_LOROM;

        bool load(std::string name) {
            std::ifstream file(name, std::ios::binary | std::ios::ate);

            if ((!file.is_open()) || (!file.good())) {
                _log(debug, "Couldn't load ROM file \"%s\"", name.c_str());
                return false;
            }

            rom.resize(file.tellg());

            file.seekg(0);
            file.read((char*)rom.data(), rom.size());

            if ((rom.at(0x7fd5) & 0x21) == ROM_LOROM) rom_type = ROM_LOROM;
            if (rom.at(0xffd5) == ROM_HIROM) rom_type = ROM_HIROM;

            _log(debug, "ROM type: %s", (rom_type == ROM_LOROM) ? "LoROM" : (rom_type == ROM_HIROM) ? "HiROM" : "Unknown");

            u16 header = 0x7fc0 | ((rom_type & 0x1) << 15);
            _log(debug, "header=%04x", header);

            char* rom_name = reinterpret_cast<char*>(&rom.at(header + 0x0));

            _log(debug, "name=%s", rom_name);

            return true;
        }

        void write(u32 addr, u8 value) {};

        u8 read(u32 addr, int size = 1) {
            u8 bank = (addr >> 16) & 0xff,
               page = (addr >> 8) & 0xff;
            u16 pb = addr & 0xffff;
            
            bool sram_read = false;

            switch (rom_type) {
                case ROM_LOROM: {
                    // Main
                    if (BANKS(0x80, 0xff) && (page >= 0x80)) { addr = ((bank - 0x80) << 15) | (pb - 0x8000); sram_read = false; };
                    if (BANKS(0xf0, 0xff) && (page <= 0x80)) { addr = ((bank - 0xf0) << 15) |  pb          ; sram_read = true;  };
                    if (BANKS(0xc0, 0xff) && (page <= 0x80)) { addr = ((bank - 0xc0) << 15) |  pb          ; sram_read = false; };

                    // Mirror
                    if (BANKS(0x00, 0x7f) && (page >= 0x80)) { addr = ((bank       ) << 15) | (pb - 0x8000); sram_read = false; };
                    if (BANKS(0x70, 0x7f) && (page <= 0x80)) { addr = ((bank - 0x70) << 15) |  pb          ; sram_read = true;  };
                    if (BANKS(0x40, 0x7f) && (page <= 0x80)) { addr = ((bank - 0x40) << 15) |  pb          ; sram_read = false; };

                    return sram_read ? sram.at(addr % sram.size()) : rom.at(addr % rom.size());
                } break;
                case ROM_HIROM: {
                    // Mirror Q1, Q2 to Q3, Q4
                    if (BANKS(0x00, 0x7f)) bank += 0x80;
            
                    // Main
                    if (BANKS(0xc0, 0xff)) { addr = (bank - 0xc0) << 15 | pb; sram_read = false; }

                    // Mirror
                    if (BANKS(0x80, 0xbf) && (page >= 0x80)) {
                        addr = ((bank - 0x80) << 16) | pb;
                        sram_read = false;
                    }

                    return sram_read ? sram.at(addr % sram.size()) : rom.at(addr % rom.size());
                } break;
            }

            return 0xff;
        }
    }
}