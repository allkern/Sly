# Sly
 SNES Emulator and Emulation API written in C++
 
<p align="center">
  <img src="https://user-images.githubusercontent.com/15825466/153125129-b0b89f73-2236-46c1-8bee-01f4b7d8cf8b.png" width="49%" height="49%">
  <img src="https://user-images.githubusercontent.com/15825466/153455436-a752dfe2-aa66-4c96-a01e-a5d81c1adf8e.png" width="49%" height="49%">
</p>

## Current state
This emulator is in a **really** early stage of development. Only the basics have been implemented:

 - CPU
   - All instructions except `stp`
   - Basic interrupt handling
   - No emulation mode
 - PPU
   - 2, 4 and 8 BPP rendering
   - Basic timing (NMI only)
   - All modes except mode 7
 - Cartridge
   - LoROM, broken HiROM support
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
