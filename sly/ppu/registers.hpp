#pragma once

#include "../global.hpp"

// This wont be here for longer
#include "../apu/apu.hpp"

#define LGW_FORMAT_RGBA8888
#include "lgw/framebuffer.hpp"

#define PPU_WIDTH 256
#define PPU_WIDTH_HIRES 512
#define PPU_HEIGHT 224

#define PPU_INIDISP     0x00
#define PPU_OBSEL       0x01
#define PPU_OAMADDL     0x02
#define PPU_OAMADDH     0x03
#define PPU_OAMDATA     0x04
#define PPU_BGMODE      0x05
#define PPU_MOSAIC      0x06
#define PPU_BG1SC       0x07
#define PPU_BG2SC       0x08
#define PPU_BG3SC       0x09
#define PPU_BG4SC       0x0a
#define PPU_BG12NBA     0x0b
#define PPU_BG34NBA     0x0c
#define PPU_BG1HOFS     0x0d
#define PPU_BG1VOFS     0x0e
#define PPU_BG2HOFS     0x0f
#define PPU_BG2VOFS     0x10
#define PPU_BG3HOFS     0x11
#define PPU_BG3VOFS     0x12
#define PPU_BG4HOFS     0x13
#define PPU_BG4VOFS     0x14
#define PPU_VMAIN       0x15
#define PPU_VMADDL      0x16
#define PPU_VMADDH      0x17
#define PPU_VMDATAL     0x18
#define PPU_VMDATAH     0x19
#define PPU_M7SEL       0x1a
#define PPU_M7A         0x1b
#define PPU_M7B         0x1c
#define PPU_M7C         0x1d
#define PPU_M7D         0x1e
#define PPU_M7X         0x1f
#define PPU_M7Y         0x20
#define PPU_CGADD       0x21
#define PPU_CGDATA      0x22
#define PPU_W12SEL      0x23
#define PPU_W34SEL      0x24
#define PPU_WOBJSEL     0x25
#define PPU_WH0         0x26
#define PPU_WH1         0x27
#define PPU_WH2         0x28
#define PPU_WH3         0x29
#define PPU_WBGLOG      0x2a
#define PPU_WOBJLOG     0x2b
#define PPU_TM          0x2c
#define PPU_TS          0x2d
#define PPU_TMW         0x2e
#define PPU_TSW         0x2f
#define PPU_CGWSEL      0x30
#define PPU_CGADSUB     0x31
#define PPU_COLDATA     0x32
#define PPU_SETINI      0x33
#define PPU_VMDATALREAD 0x39
#define PPU_VMDATAHREAD 0x3A
#define PPU_WMDATA      0x80
#define PPU_WMADDL      0x81
#define PPU_WMADDM      0x82
#define PPU_WMADDH      0x83

namespace snes {
    namespace ppu {
        std::array <u8, 0x10000> vram;
        std::array <u8, 0x200> cgram;
        std::array <u8, 0x220> oam;

        struct queued_sprite_t {
            int x1, x2, y, l;

            u16 t;

            u8 a;

            int s;
        };

        std::vector <queued_sprite_t> queued_sprites;

        lgw::framebuffer <PPU_WIDTH, PPU_HEIGHT> frame;
        lgw::framebuffer <PPU_WIDTH_HIRES, PPU_HEIGHT> hires;

        typedef void (*frame_ready_cb_t)(uint32_t*, int);
        typedef void (*nmi_cb_t)(void);
        typedef u8 (*bus_read_t)(u32);
        typedef void (*bus_write_t)(u32, u8);

        frame_ready_cb_t frame_ready_cb;
        nmi_cb_t nmi_cb;
        bus_read_t bus_read;
        bus_write_t bus_write;

        bool initialized = false;

        u8 inidisp,
           obsel,
           oamaddl,
           oamaddh,
           oamdata,
           bgmode,
           mosaic,
           bg1sc,
           bg2sc,
           bg3sc,
           bg4sc,
           bg12nba,
           bg34nba,
           w12sel,
           w34sel,
           wobjsel,
           wh0,
           wh1,
           wh2,
           wh3,
           wbglog,
           wobjlog,
           tm,
           ts,
           tmw,
           tsw,
           cgwsel,
           cgadsub,
           coldata,
           setini,
           vmain,
           vmaddl,
           vmaddh;
        
        u16 vmadd;

        u16 bg1hofs,
            bg1vofs,
            bg2hofs,
            bg2vofs,
            bg3hofs,
            bg3vofs,
            bg4hofs,
            bg4vofs;
        
        u16 coldatf;
        
        enum bg_size_t : int {
            BG_32x32,
            BG_64x32,
            BG_32x64,
            BG_64x64
        };

        enum bg_number_t : int {
            BG1,
            BG2,
            BG3,
            BG4
        };

        inline int get_bg_size(int bg) {
            switch (bg) {
                case BG1: return bg1sc & 0x3;
                case BG2: return bg2sc & 0x3;
                case BG3: return bg3sc & 0x3;
                case BG4: return bg4sc & 0x3;
            }

            return 0x0;
        }

        inline u16 get_bg_tilemap_addr(int bg) {
            switch (bg) {
                case BG1: return ((bg1sc >> 2) & 0x3f) << 11;
                case BG2: return ((bg2sc >> 2) & 0x3f) << 11;
                case BG3: return ((bg3sc >> 2) & 0x3f) << 11;
                case BG4: return ((bg4sc >> 2) & 0x3f) << 11;
            }

            return 0x0;
        }

        inline u16 get_bg_char_addr(int bg) {
            switch (bg) {
                case BG1: return ((bg12nba     ) & 0xf) << 13;
                case BG2: return ((bg12nba >> 4) & 0xf) << 13;
                case BG3: return ((bg34nba     ) & 0xf) << 13;
                case BG4: return ((bg34nba >> 4) & 0xf) << 13;
            }

            return 0x0;
        }

        inline u16 get_bg_hofs(int bg) {
            switch (bg) {
                case BG1: return bg1hofs & 0x3ff;
                case BG2: return bg2hofs & 0x3ff;
                case BG3: return bg3hofs & 0x3ff;
                case BG4: return bg4hofs & 0x3ff;
            }

            return 0x0;
        }

        inline u16 get_bg_vofs(int bg) {
            switch (bg) {
                case BG1: return (bg1vofs + 1) & 0x3ff;
                case BG2: return (bg2vofs + 1) & 0x3ff;
                case BG3: return (bg3vofs + 1) & 0x3ff;
                case BG4: return (bg4vofs + 1) & 0x3ff;
            }

            return 0x0;
        }

        enum bg_char_size_t : bool {
            BG_8X8,
            BG_16X16
        };

        inline bool get_bg_char_size(int bg) {
            return (bgmode >> (4 + bg)) & 0x1;
        }

        enum bg_mode_t : int {
            BG_MODE0,
            BG_MODE1,
            BG_MODE2,
            BG_MODE3,
            BG_MODE4,
            BG_MODE5,
            BG_MODE6,
            BG_MODE7,     // Not yet supported
            BG_MODE7EXTBG // Not yet supported
        };

        inline int get_bg_mode() {
            return bgmode & 0x7;
        }

        enum bpp_t {
            FNONE = 0,
            F2BPP = 2,
            F4BPP = 4,
            F8BPP = 8
        };

        bpp_t modes_bpp[] = {
        //  BG0    BG1    BG2    BG3
            F2BPP, F2BPP, F2BPP, F2BPP, // Mode 0
            F4BPP, F4BPP, F2BPP, FNONE, // Mode 1
            F4BPP, F4BPP, FNONE, FNONE, // Mode 2
            F8BPP, F4BPP, FNONE, FNONE, // Mode 3
            F8BPP, F2BPP, FNONE, FNONE, // Mode 4
            F4BPP, F2BPP, FNONE, FNONE, // Mode 5
            F4BPP, FNONE, FNONE, FNONE, // Mode 6
            F8BPP, FNONE, FNONE, FNONE  // Mode 7
        };

        inline int get_bg_bpp(int bg) {
            int mode = get_bg_mode();

            return modes_bpp[(mode * 4) + bg];
        }

        struct sprite_size_t {
            int smallh, smallv, bigh, bigv;
        };

        enum sprite_sizes {
            SS_8X8_16X16,
            SS_8X8_32X32,
            SS_8X8_64X64,
            SS_16X16_32X32,
            SS_16X16_64X64,
            SS_32X32_64X64,
            SS_16X32_32X64,
            SS_16X32_32X32
        };

        inline sprite_size_t get_global_sprite_size() {
            switch (obsel >> 5) {
                case SS_8X8_16X16: return { 8, 8, 16, 16 };
                case SS_8X8_32X32: return { 8, 8, 32, 32 };
                case SS_8X8_64X64: return { 8, 8, 64, 64 };
                case SS_16X16_32X32: return { 16, 16, 32, 32 };
                case SS_16X16_64X64: return { 16, 16, 64, 64 };
                case SS_32X32_64X64: return { 32, 32, 64, 64 };
                case SS_16X32_32X64: return { 16, 32, 32, 64 };
                case SS_16X32_32X32: return { 16, 32, 32, 32 };
            }

            return { 0, 0, 0, 0 };
        }

        inline u16 get_primary_oam_addr() {
            return (obsel & 0x7) << 14;
        }

        inline bool get_bg_tile_size(int bg) {
            return (bgmode >> 4) & (1 << bg);
        }

        inline bool get_wide_mode() {
            return get_bg_mode() == BG_MODE5;
        }

        inline bool get_pseudo_hires() {
            return get_bg_mode() == BG_MODE5;
        }

        enum vmain_increment_mode_t : bool {
            IM_W18R39,
            IM_W19R3A
        };

        inline bool get_vmain_increment_mode() {
            return (vmain >> 7) & 0x1;
        }

        inline int get_vmain_increment() {
            static const int increments[] = { 1, 32, 128, 128 };

            return increments[vmain & 0x3];
        }

        inline u16 get_vmain_remapped_address(u16 addr) {
            switch ((vmain >> 2) & 0x3) {
                case 0: return addr << 1;
                case 1: {
                    u16 c = addr & 0x1f,
                        b = (addr >> 5) & 0x7;
                    
                    return ((addr & 0xff00) | (c << 3) | b) << 1;
                } break;
                case 2: {
                    u16 c = addr & 0x3f,
                        b = (addr >> 6) & 0x7;
                    
                    return ((addr & 0xfe00) | (c << 3) | b) << 1;
                } break;
                case 3: {
                    u16 c = addr & 0x7f,
                        b = (addr >> 7) & 0x7;
                    
                    return ((addr & 0xfc00) | (c << 3) | b) << 1;
                } break;
            }

            return addr << 1;
        }

        bool bg1hofs_latch = false,
             bg1vofs_latch = false,
             bg2hofs_latch = false,
             bg2vofs_latch = false,
             bg3hofs_latch = false,
             bg3vofs_latch = false,
             bg4hofs_latch = false,
             bg4vofs_latch = false;
        
        inline bool& toggle(bool& value) {
            return value = !value;
        }

        u16 cgadd = 0x0;
        u16 oamadd = 0x0;
        u32 wmadd = 0x0;
        u8 oam_buf = 0x0;
        int oam_inc = 0x0;

        void write(u16 addr, u8 value) {
            if (addr == 0x2140) return apu::write(addr, value);
            if (addr == 0x2141) return apu::write(addr, value);
            if (addr == 0x2142) return apu::write(addr, value);
            if (addr == 0x2143) return apu::write(addr, value);

            switch (addr & 0xff) {
                case PPU_INIDISP: { inidisp = value; initialized = true; return; } break;
                case PPU_OBSEL  : { obsel   = value; return; } break;
                case PPU_OAMADDL: { oamadd &= 0xff00; oamadd |= value; } break;
                case PPU_OAMADDH: { oamadd &= 0x00ff; oamadd |= value << 8; } break;
                case PPU_OAMDATA: {
                    if (oamadd >= 0x220) return;
                    if (oamadd & 0x1) {
                        oam.at(oamadd - 1) = oam_buf;
                        oam.at(oamadd    ) = value;
                        oamadd++;
                    } else {
                        oam_buf = value;
                        oamadd++;
                    }
                } break;
                case PPU_BGMODE : { bgmode  = value; return; } break;
                case PPU_MOSAIC : { mosaic  = value; return; } break;
                case PPU_BG1SC  : { bg1sc   = value; return; } break;
                case PPU_BG2SC  : { bg2sc   = value; return; } break;
                case PPU_BG3SC  : { bg3sc   = value; return; } break;
                case PPU_BG4SC  : { bg4sc   = value; return; } break;
                case PPU_BG12NBA: { bg12nba = value; return; } break;
                case PPU_BG34NBA: { bg34nba = value; return; } break;
                case PPU_BG1HOFS: { int s = bg1hofs_latch * 8; bg1hofs &= 0xff << (8 - s); bg1hofs |= value << s; toggle(bg1hofs_latch); return; } break;
                case PPU_BG1VOFS: { int s = bg1vofs_latch * 8; bg1vofs &= 0xff << (8 - s); bg1vofs |= value << s; toggle(bg1vofs_latch); return; } break;
                case PPU_BG2HOFS: { int s = bg2hofs_latch * 8; bg2hofs &= 0xff << (8 - s); bg2hofs |= value << s; toggle(bg2hofs_latch); return; } break;
                case PPU_BG2VOFS: { int s = bg2vofs_latch * 8; bg2vofs &= 0xff << (8 - s); bg2vofs |= value << s; toggle(bg2vofs_latch); return; } break;
                case PPU_BG3HOFS: { int s = bg3hofs_latch * 8; bg3hofs &= 0xff << (8 - s); bg3hofs |= value << s; toggle(bg3hofs_latch); return; } break;
                case PPU_BG3VOFS: { int s = bg3vofs_latch * 8; bg3vofs &= 0xff << (8 - s); bg3vofs |= value << s; toggle(bg3vofs_latch); return; } break;
                case PPU_BG4HOFS: { int s = bg4hofs_latch * 8; bg4hofs &= 0xff << (8 - s); bg4hofs |= value << s; toggle(bg4hofs_latch); return; } break;
                case PPU_BG4VOFS: { int s = bg4vofs_latch * 8; bg4vofs &= 0xff << (8 - s); bg4vofs |= value << s; toggle(bg4vofs_latch); return; } break;
                case PPU_VMAIN  : { vmain = value; return; } break;
                case PPU_VMADDL : { vmadd &= 0xff00; vmadd |= value; return; } break;
                case PPU_VMADDH : { vmadd &= 0x00ff; vmadd |= value << 8; return; } break;
                case PPU_VMDATAL: {
                    u16 addr = get_vmain_remapped_address(vmadd);
                    vram.at(addr) = value;

                    bool mode = get_vmain_increment_mode();
 
                    if (mode == IM_W18R39)
                        vmadd += get_vmain_increment();
                    return;
                } break;
                case PPU_VMDATAH: {
                    u16 addr = get_vmain_remapped_address(vmadd) + 1;
                    vram.at(addr) = value;

                    bool mode = get_vmain_increment_mode();

                    if (mode == IM_W19R3A)
                        vmadd += get_vmain_increment();
                    return;
                } break;
                case PPU_CGADD  : { cgadd   = value; return; } break;
                case PPU_CGDATA : { cgram.at(cgadd++) = value; return; } break;
                case PPU_W12SEL : { w12sel  = value; return; } break;
                case PPU_W34SEL : { w34sel  = value; return; } break;
                case PPU_WOBJSEL: { wobjsel = value; return; } break;
                case PPU_WH0    : { wh0     = value; return; } break;
                case PPU_WH1    : { wh1     = value; return; } break;
                case PPU_WH2    : { wh2     = value; return; } break;
                case PPU_WH3    : { wh3     = value; return; } break;
                case PPU_WBGLOG : { wbglog  = value; return; } break;
                case PPU_WOBJLOG: { wobjlog = value; return; } break;
                case PPU_TM     : { tm      = value; return; } break;
                case PPU_TS     : { ts      = value; return; } break;
                case PPU_TMW    : { tmw     = value; return; } break;
                case PPU_TSW    : { tsw     = value; return; } break;
                case PPU_CGWSEL : { cgwsel  = value; return; } break;
                case PPU_CGADSUB: { cgadsub = value; return; } break;
                case PPU_COLDATA: {
                    u8 color = value & 0x1f;

                    if (value & 0x20) { coldatf &= 0x1f      ; coldatf |= color      ; }
                    if (value & 0x40) { coldatf &= 0x1f << 5 ; coldatf |= color << 5 ; }
                    if (value & 0x80) { coldatf &= 0x1f << 10; coldatf |= color << 10; }
                    return;
                } break;
                case PPU_SETINI : { setini  = value; return; } break;
                case PPU_WMDATA : { bus_write(wmadd++, value); return; }
                case PPU_WMADDL : { wmadd &= 0xffff00; wmadd |= value; return; }
                case PPU_WMADDM : { wmadd &= 0xff00ff; wmadd |= value << 8; return; }
                case PPU_WMADDH : { wmadd &= 0x00ffff; wmadd |= (value << 16) & 0x1; return; }
            }

            //_log(debug, "unhandled PPU write %04x:%02x", addr, value);
        }

        u8 read(u16 addr) {
            if (addr == 0x2140) return apu::read(addr);
            if (addr == 0x2141) return apu::read(addr);
            if (addr == 0x2142) return apu::read(addr);
            if (addr == 0x2143) return apu::read(addr);

            switch (addr & 0xff) {
                case PPU_INIDISP: return inidisp;
                case PPU_OBSEL  : return obsel;
                case PPU_OAMADDL: return oamadd & 0xff;
                case PPU_OAMADDH: return (oamadd >> 8) & 0xff;
                case PPU_OAMDATA: return oamadd < 0x220 ? oam.at(oamadd++) : 0xff;
                case PPU_BGMODE : return bgmode;
                case PPU_MOSAIC : return mosaic;
                case PPU_BG1SC  : return bg1sc;
                case PPU_BG2SC  : return bg2sc;
                case PPU_BG3SC  : return bg3sc;
                case PPU_BG4SC  : return bg4sc;
                case PPU_BG12NBA: return bg12nba;
                case PPU_BG34NBA: return bg34nba;
                case PPU_W12SEL : return w12sel;
                case PPU_W34SEL : return w34sel;
                case PPU_WOBJSEL: return wobjsel;
                case PPU_WH0    : return wh0;
                case PPU_WH1    : return wh1;
                case PPU_WH2    : return wh2;
                case PPU_WH3    : return wh3;
                case PPU_WBGLOG : return wbglog;
                case PPU_WOBJLOG: return wobjlog;
                case PPU_TM     : return tm;
                case PPU_TS     : return ts;
                case PPU_TMW    : return tmw;
                case PPU_TSW    : return tsw;
                case PPU_CGWSEL : return cgwsel;
                case PPU_CGADSUB: return cgadsub;
                case PPU_COLDATA: return coldata;
                case PPU_SETINI : return setini;
                case PPU_WMDATA : return bus_read(wmadd++);
                case PPU_VMDATALREAD: {
                    u8 data = vram.at(vmadd);

                    bool mode = get_vmain_increment_mode();

                    if (mode == IM_W18R39)
                        vmadd += get_vmain_increment();
                    
                    return data;
                }
                case PPU_VMDATAHREAD: {
                    u8 data = vram.at(vmadd + 1);

                    bool mode = get_vmain_increment_mode();

                    if (mode == IM_W19R3A)
                        vmadd += get_vmain_increment();
                    
                    return data;
                }
            }

            return 0xff;

            //_log(debug, "unhandled PPU read %04x", addr);
        }
    }
}