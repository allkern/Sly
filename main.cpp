#include "cpu/cpu.hpp"
#include "ppu/ppu.hpp"
#include "log.hpp"

#include "bus.hpp"

#include "frontend/window.hpp"

#include "disasm.hpp"

#include <chrono>
#include <thread>

#undef main

void log_cpu_state() {
    char disassembled[100];

    disasm(reinterpret_cast<unsigned char*>(&snes::cpu::full_opcode), 0, &snes::cpu::registers::p, &disassembled[0], 1);

    size_t len = std::strlen(disassembled);

    if (disassembled[len - 1] == 0x20) len--;

    if (len < 6) {
        _log(debug, "%s\t\t\tpc=%06x, a=%04x, x=%04x, y=%04x, d=%04x, db=%02x, p=%02x, sp=%04x, stack=%02x %02x %02x %02x %02x %02x %02x %02x, lc=%u, tc=%u",
            &disassembled[0],
            snes::cpu::base_pc,
            snes::cpu::registers::a,
            snes::cpu::registers::x,
            snes::cpu::registers::y,
            snes::cpu::registers::d,
            snes::cpu::registers::db,
            snes::cpu::registers::p,
            snes::cpu::registers::sp,
            snes::bus::read8(snes::cpu::sp + 1),
            snes::bus::read8(snes::cpu::sp + 2),
            snes::bus::read8(snes::cpu::sp + 3),
            snes::bus::read8(snes::cpu::sp + 4),
            snes::bus::read8(snes::cpu::sp + 5),
            snes::bus::read8(snes::cpu::sp + 6),
            snes::bus::read8(snes::cpu::sp + 7),
            snes::bus::read8(snes::cpu::sp + 8),
            snes::cpu::last_cycles,
            snes::cpu::total_cycles
        );
    } else {
        _log(debug, "%s\t\tpc=%06x, a=%04x, x=%04x, y=%04x, d=%04x, db=%02x, p=%02x, sp=%04x, stack=%02x %02x %02x %02x %02x %02x %02x %02x, lc=%u, tc=%u",
            &disassembled[0],
            snes::cpu::base_pc,
            snes::cpu::registers::a,
            snes::cpu::registers::x,
            snes::cpu::registers::y,
            snes::cpu::registers::d,
            snes::cpu::registers::db,
            snes::cpu::registers::p,
            snes::cpu::registers::sp,
            snes::bus::read8(snes::cpu::sp + 1),
            snes::bus::read8(snes::cpu::sp + 2),
            snes::bus::read8(snes::cpu::sp + 3),
            snes::bus::read8(snes::cpu::sp + 4),
            snes::bus::read8(snes::cpu::sp + 5),
            snes::bus::read8(snes::cpu::sp + 6),
            snes::bus::read8(snes::cpu::sp + 7),
            snes::bus::read8(snes::cpu::sp + 8),
            snes::cpu::last_cycles,
            snes::cpu::total_cycles
        );
    }
}

int main(int argc, const char* argv[]) {
    using namespace snes;

    _log::init("snes");

    bool loaded = false;

    if (argv[1]) {
        loaded = cart::load(argv[1]);
    } else {
        loaded = cart::load("roms/parodius.sfc");
    }

    //_log(debug, "[0a84c6]=%02x", bus::read8(0x0a84c6));

    //return 0;

    if (!loaded) return 0;

    cpu::init();
    dma::init(bus::read8, bus::write8);

    frontend::window::init(3);

    //for (u8& b : ppu::vram) b = rand() & 0xff;
    //for (u8& b : ppu::cgram) b = rand() & 0xff;

    ppu::init(frontend::window::update, cpu::nmi, bus::read8, bus::write8);

    while (frontend::window::is_open()) {
        if (start_logging) { log_cpu_state(); }
        cpu::fetch();
        cpu::execute();
        ppu::tick(cpu::last_cycles);
        if ((cpu::base_pc == 0x00a94d) && (cpu::registers::d == 0x100)) start_logging = true;

        if (stop_cpu) break;

        //start_logging = true;
        //if (cpu::base_pc == 0x00813c) start_logging = true;
        //if (cpu::base_pc == 0x008438) start_logging = true;
        //if (cpu::total_cycles >= 82883) start_logging = true;

        //if (start_logging) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    while (frontend::window::is_open()) {
        ppu::tick(cpu::last_cycles);
    }

    return 0;
}