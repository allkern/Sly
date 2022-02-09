# Sly
 SNES Emulator and Emulation API written in C++

## Current state
This emulator is in a **really** early stage of development. Only the basics have been implemented:

 - CPU
   - All instructions except `stp` and `wai`
   - Basic interrupt handling
   - No emulation mode
 - PPU
   - Basic 2 or 4 BPP rendering
   - Basic timing (NMI only)
   - Modes 0, 1, 3
 - Cartridge
   - LoROM supported only
 - DMA
   - All GPDMA modes supported
   - HDMA not supported
 - Input
   - Basic SNES controller (1 to 4 players)
   - Auto Joypad polling not supported
 - APU/SPC
   - Nothing!

The SPC700 core hasn't been implemented yet, so the emulator will just try to spoof presence of the SPC by returning expected values.
This doesn't always work, and some games won't boot even with these expected values (Super Mario World, for example)
