#pragma once

#include <fstream>
#include <array>

#include "../global.hpp"
#include "registers.hpp"

#include "immintrin.h"

#include "omp.h"

#include <cmath>

#define BGPIXEL 10000

namespace snes {
    namespace ppu {
        void init(frame_ready_cb_t fr_cb, nmi_cb_t nmicb, bus_read_t br, bus_write_t bw) {
            frame_ready_cb = fr_cb;
            nmi_cb = nmicb;
            bus_read = br;
            bus_write = bw;

            queued_sprites.resize(32);

            frame.clear();
        }

        struct tile_t {
            int ix, iy, tm;
        };

        tile_t get_tile_index(int x, int y, int bg) {
            int size = get_bg_size(bg);

            bool tiles16x16 = get_bg_tile_size(bg); 
            bool tiles_wide = get_wide_mode();

            int tx = x >> ((tiles16x16 || tiles_wide) ? 4 : 3),
                ty = y >> (tiles16x16 ? 4 : 3);

            switch (size) {
                case BG_32x32: return { tx & 0x1f, ty & 0x1f, 0                                            };
                case BG_64x32: return { tx & 0x1f, ty & 0x1f,  ((tx >> 5) & 0x1)                           };
                case BG_32x64: return { tx & 0x1f, ty & 0x1f, (((ty >> 5) & 0x1) << 1)                     };
                case BG_64x64: return { tx & 0x1f, ty & 0x1f, (((ty >> 5) & 0x1) << 1) | ((tx >> 5) & 0x1) };
            }

            return { 0, 0, 0 };
        }

        u16 get_tile_addr(tile_t tile, int bg) {
            u16 base = get_bg_tilemap_addr(bg) + (tile.tm << 11);

            return base + (tile.iy << 6) + (tile.ix << 1);
        }

        u16 get_char_addr_impl(int n, int x, int y, bool flip, int bpp, u16 base, bool t16) {
            y = flip ? ((t16 ? 15 : 7) - y) : y;

            if (y > 7) n += 0x10;
            if (x > 7) n += 1;

            int shift = 4 + (bpp >> 2);

            return base + (n << shift) + ((y % 8) << 1);
        }

        u16 get_char_addr(int n, int x, int y, bool flip, int bg) {
            return get_char_addr_impl(n, x, y, flip, get_bg_bpp(bg), get_bg_char_addr(bg), get_bg_tile_size(bg));
        }

        u64 get_char_data(u16 addr, int bpp) {
            u64 data = 0;

            switch (bpp) {
                case F2BPP: { data = (vram[addr + 1] << 8) | vram[addr]; } break;
                case F4BPP: { data = (vram[addr + 1] << 8) | vram[addr] | (vram[addr + 0x10 + 1] << 24) | (vram[addr + 0x10] << 16); } break;
                case F8BPP: {
                    data =
                        ((u64)vram[addr + 0x0  + 0] << 0 ) |
                        ((u64)vram[addr + 0x0  + 1] << 8 ) |
                        ((u64)vram[addr + 0x10 + 0] << 16) |
                        ((u64)vram[addr + 0x10 + 1] << 24) |
                        ((u64)vram[addr + 0x20 + 0] << 32) |
                        ((u64)vram[addr + 0x20 + 1] << 40) |
                        ((u64)vram[addr + 0x30 + 0] << 48) |
                        ((u64)vram[addr + 0x30 + 1] << 56);
                }
            }

            return data;
        }

        int get_pixel_index(u64 data, int p, bool flip, int bpp) {
            u64 row = data,
                mask = flip ? (0x0101010101010101ull << p) : (0x8080808080808080ull >> p);

            return _pext_u64(row, mask) & ((1 << bpp) - 1);
        }

        u32 rgb555_to_rgba8888(u16 color) {
            int r = (int)std::round((double)(color & 0x1f) * 8.0),
                g = (int)std::round((double)((color >> 5) & 0x1f) * 8.0),
                b = (int)std::round((double)((color >> 10) & 0x1f) * 8.0);

            double brightness = (inidisp & 0xf) / 16.0;
            
            r *= brightness;
            g *= brightness;
            b *= brightness;
            
            return (r << 24) | (g << 16) | (b << 8) | 0xff;
        }

        struct snes_pixel_t {
            int index, palette, bpp;

            bool priority;
            int sprite_priority;
        };

        u16 get_pixel_snes_color(snes_pixel_t pixel) {
            u16 offset = (pixel.palette << (1 + pixel.bpp)) + (pixel.index << 1);

            return (cgram[offset + 1] << 8) | cgram[offset];
        }

        namespace current {
            tile_t tile;
            u16 addr, prev;
            u16 data;
            bool fliph,
                 flipv,
                 priority;
            int  palette,
                 number;
            u16  char_addr;
            u64  char_data;
        }

        void refetch(int sx, int sy, int px, int py, int bg) {
            current::tile      = get_tile_index(sx, sy, bg);
            current::prev      = current::addr;
            current::addr      = get_tile_addr(current::tile, bg);

            if (current::addr == current::prev) return;

            current::data      = (vram[current::addr + 1] << 8) | vram[current::addr];
            current::fliph     = (current::data >> 14) & 0x1,
            current::flipv     = (current::data >> 15) & 0x1,
            current::priority  = (current::data >> 13) & 0x1;
            current::palette   = (current::data >> 10) & 0x7,
            current::number    = current::data & 0x3ff;
            current::char_addr = get_char_addr(current::number, px, py, current::flipv, bg);
            current::char_data = get_char_data(current::char_addr, get_bg_bpp(bg));
        }

        snes_pixel_t render_bg_pixel(int x, int y, int bg) {
            int sx = x + get_bg_hofs(bg),
                sy = y + get_bg_vofs(bg);

            bool t16 = get_bg_tile_size(bg);

            int px = sx % ((t16 || get_pseudo_hires()) ? 16 : 8),
                py = sy % (t16 ? 16 : 8);

            refetch(sx, sy, px, py, bg);

            int pidx = get_pixel_index(current::char_data, px % 8, current::fliph, get_bg_bpp(bg));

            return { pidx, current::palette, get_bg_bpp(bg), current::priority };
        }

        snes_pixel_t render_sprite_pixel(int);

        snes_pixel_t render_final_pixel(int x, int y, int mode) { 
            bool bg1en = tm & 0b00000001,
                 bg2en = tm & 0b00000010,
                 bg3en = tm & 0b00000100,
                 bg4en = tm & 0b00001000,
                 spren = tm & 0b00010000;

            switch (mode) {
                case BG_MODE0: {
                    snes_pixel_t sprp = render_sprite_pixel(x);
                    if ((sprp.sprite_priority == 3) && sprp.index && spren) return sprp;
                    snes_pixel_t bg1p = render_bg_pixel(x, y, BG1);
                    if (bg1p.priority && bg1p.index && bg1en) return bg1p;
                    snes_pixel_t bg2p = render_bg_pixel(x, y, BG2);
                    if (bg2p.priority && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 2) && sprp.index && spren) return sprp;
                    if (!bg1p.priority && bg1p.index && bg1en) return bg1p;
                    if (!bg2p.priority && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 1) && sprp.index && spren) return sprp;
                    snes_pixel_t bg3p = render_bg_pixel(x, y, BG3);
                    if (bg3p.priority && bg3p.index && bg3en) return bg3p;
                    snes_pixel_t bg4p = render_bg_pixel(x, y, BG4);
                    if (bg4p.priority && bg4p.index && bg4en) return bg4p;
                    if ((sprp.sprite_priority == 0) && sprp.index && spren) return sprp;
                    if (!bg3p.priority && bg3p.index && bg3en) return bg3p;
                    if (!bg4p.priority && bg4p.index && bg4en) return bg4p;

                    return { 10000, 0, 0, 0 };
                } break;
                case BG_MODE1: {
                    snes_pixel_t bg3p = render_bg_pixel(x, y, BG3);
                    if (bg3p.priority && ((bgmode >> 3) & 0x1) && bg3p.index && bg3en) return bg3p;
                    snes_pixel_t sprp = render_sprite_pixel(x);
                    if ((sprp.sprite_priority == 3) && sprp.index && spren) return sprp;
                    snes_pixel_t bg1p = render_bg_pixel(x, y, BG1);
                    if (bg1p.priority && bg1p.index && bg1en) return bg1p;
                    snes_pixel_t bg2p = render_bg_pixel(x, y, BG2);
                    if (bg2p.priority && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 2) && sprp.index && spren) return sprp;
                    if ((!bg1p.priority) && bg1p.index && bg1en) return bg1p;
                    if ((!bg2p.priority) && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 1) && sprp.index && spren) return sprp;
                    if (bg3p.priority && (!((bgmode >> 3) & 0x1)) && bg3p.index && bg3en) return bg3p;
                    if ((sprp.sprite_priority == 0) && sprp.index && spren) return sprp;
                    if ((!bg3p.priority) && bg3p.index && bg3en) return bg3p;

                    return { BGPIXEL, 0, 0, 0 };
                } break;
                case BG_MODE2: {
                    snes_pixel_t sprp = render_sprite_pixel(x);
                    if ((sprp.sprite_priority == 3) && sprp.index && spren) return sprp;
                    snes_pixel_t bg1p = render_bg_pixel(x, y, BG1);
                    if (bg1p.priority && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 2) && sprp.index && spren) return sprp;
                    snes_pixel_t bg2p = render_bg_pixel(x, y, BG2);
                    if (bg2p.priority && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 1) && sprp.index && spren) return sprp;
                    if ((!bg1p.priority) && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 0) && sprp.index && spren) return sprp;
                    if ((!bg2p.priority) && bg2p.index && bg2en) return bg2p;

                    return { BGPIXEL, 0, 0, 0 };
                } break;
                case BG_MODE3: {
                    snes_pixel_t sprp = render_sprite_pixel(x);
                    if ((sprp.sprite_priority == 3) && sprp.index && spren) return sprp;
                    snes_pixel_t bg1p = render_bg_pixel(x, y, BG1);
                    if (bg1p.priority && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 2) && sprp.index && spren) return sprp;
                    snes_pixel_t bg2p = render_bg_pixel(x, y, BG2);
                    if (bg2p.priority && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 1) && sprp.index && spren) return sprp;
                    if ((!bg1p.priority) && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 0) && sprp.index && spren) return sprp;
                    if ((!bg2p.priority) && bg2p.index && bg2en) return bg2p;

                    return { BGPIXEL, 0, 0, 0 };
                } break;
                case BG_MODE4: {
                    snes_pixel_t sprp = render_sprite_pixel(x);
                    if ((sprp.sprite_priority == 3) && sprp.index && spren) return sprp;
                    snes_pixel_t bg1p = render_bg_pixel(x, y, BG1);
                    if (bg1p.priority && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 2) && sprp.index && spren) return sprp;
                    snes_pixel_t bg2p = render_bg_pixel(x, y, BG2);
                    if (bg2p.priority && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 1) && sprp.index && spren) return sprp;
                    if ((!bg1p.priority) && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 0) && sprp.index && spren) return sprp;
                    if ((!bg2p.priority) && bg2p.index && bg2en) return bg2p;

                    return { BGPIXEL, 0, 0, 0 };
                } break;
                case BG_MODE5: {
                    snes_pixel_t sprp = render_sprite_pixel(x);
                    if ((sprp.sprite_priority == 3) && sprp.index && spren) return sprp;
                    snes_pixel_t bg1p = render_bg_pixel(x, y, BG1);
                    if (bg1p.priority && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 2) && sprp.index && spren) return sprp;
                    snes_pixel_t bg2p = render_bg_pixel(x, y, BG2);
                    if (bg2p.priority && bg2p.index && bg2en) return bg2p;
                    if ((sprp.sprite_priority == 1) && sprp.index && spren) return sprp;
                    if ((!bg1p.priority) && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 0) && sprp.index && spren) return sprp;
                    if ((!bg2p.priority) && bg2p.index && bg2en) return bg2p;

                    return { BGPIXEL, 0, 0, 0 };
                } break;
                case BG_MODE6: {
                    snes_pixel_t sprp = render_sprite_pixel(x);
                    if ((sprp.sprite_priority == 3) && sprp.index && spren) return sprp;
                    snes_pixel_t bg1p = render_bg_pixel(x, y, BG1);
                    if (bg1p.priority && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 2) && sprp.index && spren) return sprp;
                    if ((sprp.sprite_priority == 1) && sprp.index && spren) return sprp;
                    if ((!bg1p.priority) && bg1p.index && bg1en) return bg1p;
                    if ((sprp.sprite_priority == 0) && sprp.index && spren) return sprp;

                    return { BGPIXEL, 0, 0, 0 };
                } break;

                default: break;
            }

            return { 0, 0 };
        }

        snes_pixel_t render_sprite_pixel(int sx) {
            u16 primary_oam_addr = get_primary_oam_addr();

            if (!queued_sprites.size()) return { 0 };

            for (int index = 0; index < queued_sprites.size(); index++) {
                auto sprite = queued_sprites.at(index);

                if (!(sx >= sprite.x1)) continue;
                if (!(sx < sprite.x2)) continue;

                int px = sx - sprite.x1,
                    py = sprite.y;

                if ((sprite.a & 0b01000000) && (sprite.s >= 16))
                    px = (sprite.s - 1) - px;

                u16 caddr = get_char_addr_impl(
                    sprite.t + (px >> 3) + ((py >> 3) << 4),
                    px % 8,
                    py % 8,
                    sprite.a & 0b10000000,
                    F4BPP,
                    primary_oam_addr,
                    false
                );

                int pidx = get_pixel_index(get_char_data(caddr, F4BPP), px, (sprite.a & 0b01000000) && (sprite.s == 8), F4BPP);

                if (!pidx) continue;

                return { pidx, 8 + ((sprite.a >> 1) & 0x7), F4BPP, false, ((sprite.a >> 4) & 0x3) };
            }

            return { 0, 0, 0, 0, 0 };
        }

        void queue_scanline_sprites(int l) {
            //_log(debug, "queuing scanline sprites for scanline %u", l);
            queued_sprites.clear();
            queued_sprites.reserve(33);

            sprite_size_t gss = get_global_sprite_size();

            for (u16 index = 0; index < 128; index++) {
                if (queued_sprites.size() == 32) return;

                u16 offset = index << 2;

                u16 x = oam[offset + 0],
                    y = oam[offset + 1],
                    t = oam[offset + 2];
                u8  a = oam[offset + 3],
                    e = oam[0x200 + (index >> 2)];

                // Calculate full X
                int b = (index << 1) % 8;
                bool s = (e >> (b + 1)) & 0x1;

                x |= ((e >> b) & 0x1) << 8;

                // If offscreen, discard
                if (x > PPU_WIDTH) continue;
                
                // Calculate bottom and right borders
                int x2 = x + (s ? gss.bigh : gss.smallh),
                    y2 = y + (s ? gss.bigv : gss.smallv);
                
                // if this line doesn't intersect with this sprite
                // discard it
                //_log(debug, "sprite y1=%u, y2=%u, l=%u", y, y2, l);
                if (!((l >= y) && (l < y2))) continue;

                // Append high tile bit
                t |= (a & 0x1) << 8;

                queued_sprites.push_back(queued_sprite_t{ x, x2, l - y, l, t, a, s ? gss.bigh : gss.smallh });
            }
        }
        
        inline void draw(int x, int y, uint32_t color, bool hi) {
            if (hi) {
                hires.draw(x, y, color);
            } else {
                frame.draw(x, y, color);
            }
        }

        void render() {
            int mode = get_bg_mode();

            bool pseudo_hires = get_pseudo_hires();

            #pragma omp parallel for
            for (int y = 0; y < PPU_HEIGHT; y++) {
                queue_scanline_sprites(y);
                
                for (int x = 0; x < (pseudo_hires ? PPU_WIDTH_HIRES : PPU_WIDTH); x++) {
                    snes_pixel_t fp = render_final_pixel(x, y, mode);

                    u16 color = get_pixel_snes_color(fp);

                    if (fp.index == BGPIXEL) color = (cgram[1] << 8) | cgram[0];

                    draw(x, y, rgb555_to_rgba8888(color), pseudo_hires);
                }
            }
        }

        int h = 0;
        int px = 0, py = 0;
        int v = 0;
        int frameskip = 2;

        void tick(int lc) {
            if (dump_vram) {
                std::ofstream dump("vram_dump.bin", std::ios::binary);

                dump.write((char*)vram.data(), vram.size());

                _log(debug, "PPU registers:\n\tmode=%u\n\tbg1ofs={h:%04x, v:%04x}\n\tbg2ofs={h:%04x, v:%04x}\n\tbg3ofs={h:%04x, v:%04x}\n\tbg4ofs={h:%04x, v:%04x}\n\t"
                            "bg1addrs={tm:%04x, ts:%04x}\n\tbg2addrs={tm:%04x, ts:%04x}\n\tbg3addrs={tm:%04x, ts:%04x}\n\tbg4addrs={tm:%04x, ts:%04x}\n\t"
                            "obsel=%02x, tm=%02x",
                    get_bg_mode(),
                    get_bg_hofs(BG1), get_bg_vofs(BG1),
                    get_bg_hofs(BG2), get_bg_vofs(BG2),
                    get_bg_hofs(BG3), get_bg_vofs(BG3),
                    get_bg_hofs(BG4), get_bg_vofs(BG4),
                    get_bg_tilemap_addr(BG1), get_bg_char_addr(BG1),
                    get_bg_tilemap_addr(BG2), get_bg_char_addr(BG2),
                    get_bg_tilemap_addr(BG3), get_bg_char_addr(BG3),
                    get_bg_tilemap_addr(BG4), get_bg_char_addr(BG4),
                    obsel, tm
                );

                tile_t t { 15, 4, 0 };
                u16 addr = get_tile_addr(t, BG2);
                u16 td = (vram[addr + 1] << 8) | vram[addr];

                bool fliph = (td >> 14) & 0x1,
                    flipv = (td >> 15) & 0x1,
                    priority = (td >> 13) & 0x1;
                int  palette = (td >> 10) & 0x7,
                    number = td & 0x3ff;
                
                u16 caddr = get_char_addr(number, 0, 0, flipv, BG2);
                _log(debug, "tile_addr=%04x, td=%04x, number=%04x, caddr=%04x", addr, td, number, caddr);

                dump_vram = false;
            };

            int xb = px;
            h += lc << 3;
            px += lc >> 2;

            if (h >= 1364) {
                px = 0;
                h -= 1364;

                v++;
                py++;

                if (v == 225 && (!fired_nmi)) {
                    bool pseudo_hires = get_pseudo_hires();
                    fired_nmi = true;
                    render();
                    frame_ready_cb(pseudo_hires ? hires.get_buffer() : frame.get_buffer(), pseudo_hires);
                    nmi_cb();
                    v = 0;
                    py = 0;
                }
                if (v == 263) {
                    fired_nmi = false;
                    v = 0;
                    py = 0;
                }
            }

            //_log(debug, "ppu tick h=%u, v=%u", h, v);
        }
    }
}