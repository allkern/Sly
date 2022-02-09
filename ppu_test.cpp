#include "cpu/addressing_modes.hpp"
#include "ppu/ppu.hpp"
#include "log.hpp"

#include "frontend/window.hpp"

#undef main

int main() {
    _log::init("snes");

    std::ifstream vram_dump("vram-dump.bin", std::ios::binary),
                  cgram_dump("dumps/snes-cgram-smw-dump.bin", std::ios::binary),
                  oam_dump("dumps/snes-oam-smw-dump.bin", std::ios::binary);

    vram_dump.read(reinterpret_cast<char*>(snes::ppu::vram.data()), snes::ppu::vram.size());
    cgram_dump.read(reinterpret_cast<char*>(snes::ppu::cgram.data()), snes::ppu::cgram.size());
    oam_dump.read(reinterpret_cast<char*>(snes::ppu::oam.data()), snes::ppu::oam.size());

    // Parodius: Non-sense Fantasy
    snes::ppu::bgmode  = 0b00001001;
    snes::ppu::bg1hofs = 0x0000;
    snes::ppu::bg1vofs = 0x03ff;
    snes::ppu::bg2hofs = 0x0000;
    snes::ppu::bg2vofs = 0x03ff;
    snes::ppu::bg3hofs = 0x0000;
    snes::ppu::bg3vofs = 0x03ff;
    snes::ppu::bg4hofs = 0x0000;
    snes::ppu::bg4vofs = 0x0000;
    snes::ppu::bg1sc   = 0b00000011;
    snes::ppu::bg2sc   = 0b00110011;
    snes::ppu::bg3sc   = 0b01010011;
    snes::ppu::bg4sc   = 0b00000000;
    snes::ppu::bg12nba = 0b00000000;
    snes::ppu::bg34nba = 0b00000100;
    snes::ppu::obsel   = 0b00000011;
    snes::ppu::tm      = 0b00011111;

    // Super Mario World
    // snes::ppu::bgmode  = 0b00001001;
    // snes::ppu::bg1hofs = 0x0000;
    // snes::ppu::bg1vofs = 0x00c0;
    // snes::ppu::bg2hofs = 0x0000;
    // snes::ppu::bg2vofs = 0x00c0;
    // snes::ppu::bg3hofs = 0x0000;
    // snes::ppu::bg3vofs = 0x0000;
    // snes::ppu::bg4hofs = 0x0000;
    // snes::ppu::bg4vofs = 0x0000;
    // snes::ppu::bg1sc   = 0b00100011;
    // snes::ppu::bg2sc   = 0b00110011;
    // snes::ppu::bg3sc   = 0b01010011;
    // snes::ppu::bg4sc   = 0b00000000;
    // snes::ppu::bg12nba = 0b00000000;
    // snes::ppu::bg34nba = 0b00000100;
    // snes::ppu::obsel   = 0b00000011;
    // snes::ppu::coldatf = 0x7393;
    // snes::ppu::tm      = 0b00011111;
    
    //Donkey Kong Country
    // snes::ppu::bgmode  = 0b00000001;
    // snes::ppu::bg1hofs = 0x0000;
    // snes::ppu::bg1vofs = 0x00ff;
    // snes::ppu::bg2hofs = 0x0000;
    // snes::ppu::bg2vofs = 0x000f;
    // snes::ppu::bg3hofs = 0x00ef;
    // snes::ppu::bg3vofs = 0x0018;
    // snes::ppu::bg4hofs = 0x0000;
    // snes::ppu::bg4vofs = 0x0000;
    // snes::ppu::bg1sc   = 0b01111100;
    // snes::ppu::bg2sc   = 0b01111000;
    // snes::ppu::bg3sc   = 0b01110000;
    // snes::ppu::bg4sc   = 0b00000000;
    // snes::ppu::bg12nba = 0b00000000;
    // snes::ppu::bg34nba = 0b00000000;
    // snes::ppu::tm      = 0b00000001;

    // Super Mario World 2
    // snes::ppu::bgmode  = 0b00000000;
    // snes::ppu::bg1hofs = 0x0000;
    // snes::ppu::bg1vofs = 0x0000;
    // snes::ppu::bg2hofs = 0x0000;
    // snes::ppu::bg2vofs = 0x0000;
    // snes::ppu::bg3hofs = 0x0000;
    // snes::ppu::bg3vofs = 0x0000;
    // snes::ppu::bg4hofs = 0x0000;
    // snes::ppu::bg4vofs = 0x03ff;
    // snes::ppu::bg1sc   = 0b01100000;
    // snes::ppu::bg2sc   = 0b01101001;
    // snes::ppu::bg3sc   = 0b01110110;
    // snes::ppu::bg4sc   = 0b01111111;
    // snes::ppu::bg12nba = 0b00000000;
    // snes::ppu::bg34nba = 0b11101110;
    // snes::ppu::obsel   = 0b00000001;

    snes::ppu::render();

    frontend::window::init(3);

    while (frontend::window::is_open()) {
        snes::ppu::render();
        frontend::window::update(snes::ppu::frame.get_buffer());

        //if (frontend::window::fps_ready()) {
            char buf[128];

            std::sprintf(&buf[0], "SNES fps=%d", frontend::window::get_fps());

            SDL_SetWindowTitle(frontend::window::window, &buf[0]);
        //}

        SDL_Event event;

        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: { frontend::window::close(); } break;
                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP: { snes::ppu::bg1vofs++; } break;
                        case SDLK_DOWN: { snes::ppu::bg1vofs--; } break;
                        case SDLK_LEFT: { snes::ppu::bg1hofs++; } break;
                        case SDLK_RIGHT: { snes::ppu::bg1hofs--; } break;
                        case SDLK_RETURN: { frontend::ntsc_codec_enabled = !frontend::ntsc_codec_enabled; } break;
                    }

                    snes::ppu::bg1vofs &= 0x3ff;
                    snes::ppu::bg1hofs &= 0x3ff;
                } break;
            }
        }
    }
    
    return 0;
}