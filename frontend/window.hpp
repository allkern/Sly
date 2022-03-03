#pragma once

#ifdef _WIN32
#include "dwmapi.h"
#include "SDL_syswm.h"
#endif

#include "SDL.h"

#include "../sly/types.hpp"
#include "../log.hpp"

#define SDL_SHADER_GL_STANDALONE
#include "SDL_shader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image_resize.h"

#include <chrono>
#include <unordered_map>
#include <vector>

std::vector <uint32_t> scaled;

void scale_and_flip_buf(uint32_t* buf, int scale) {
    size_t size = (PPU_WIDTH * scale) * (PPU_HEIGHT * scale);

    if (scaled.size() < size)
        scaled.resize(size);

    for (int y = (PPU_HEIGHT - 1); y != 0; y--) {
        for (int x = 0; x < PPU_WIDTH; x++) {
            uint32_t color = buf[x + (y * PPU_WIDTH)];
            uint32_t reordered =
                (((color >> 0) & 0xff) << 24) |
                (((color >> 8) & 0xff) << 16) |
                (((color >> 16) & 0xff) << 8) |
                (((color >> 24) & 0xff) << 0);
                

            int bx = x * scale,
                by = (PPU_HEIGHT - 1 - y) * scale;
            
            for (int iy = 0; iy < scale; iy++)
                for (int ix = 0; ix < scale; ix++)
                    scaled.at((bx + ix) + ((by + iy) * (PPU_WIDTH * scale))) = reordered;
        }
    }
}

namespace frontend {
    bool ntsc_codec_enabled = false;

    namespace window {
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;

        bool shader_stack_available = true;

        SDL_Shader* encoder = nullptr,
                  * decoder = nullptr,
                  * hermite = nullptr,
                  * process = nullptr,
                  * curvature = nullptr;

        std::array <std::vector <uint32_t>, 5> buffers;

        // FPS tracking stuff
        auto start = std::chrono::high_resolution_clock::now(), end = start;
        size_t frames_rendered = 0, fps = 0;

        bool open = true;

        bool is_open() {
            return open;
        }

        size_t get_fps() {
            return fps;
        }

        bool fps_ready() {
            end = std::chrono::high_resolution_clock::now();

            std::chrono::duration <double> d = end - start;

            return std::chrono::duration_cast<std::chrono::seconds>(d).count() == 1;
        }

        void close() {
            open = false;
            
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);

            SDL_Quit();
        }

        int window_scale = 0;

        void init(int scale) {
            uint32_t SDL_INIT_FLAGS = SDL_INIT_EVERYTHING;

            SDL_Init(SDL_INIT_FLAGS);

            window_scale = scale;

            window = SDL_CreateWindow(
                "Sly",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                PPU_WIDTH * scale, PPU_HEIGHT * scale,
                SDL_WINDOW_OPENGL
            );

            SDL_GL_CreateContext(window);

            renderer = SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_PRESENTVSYNC
            );

            texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_STREAMING,
                PPU_WIDTH, PPU_HEIGHT
            );

            encoder = SDL_CreateShader(window, "shaders/identity.vert", "shaders/encode.frag");
            decoder = SDL_CreateShader(window, "shaders/identity.vert", "shaders/decode.frag");
            hermite = SDL_CreateShader(window, "shaders/identity.vert", "shaders/hermite.frag");
            process = SDL_CreateShader(window, "shaders/identity.vert", "shaders/process.frag");
            curvature = SDL_CreateShader(window, "shaders/identity.vert", "shaders/curvature.frag");

            if (!encoder) { _log(warning, "Shader compilation error:\n%s", SDL_GetError()); shader_stack_available = false; goto unavailable; }
            if (!decoder) { _log(warning, "Shader compilation error:\n%%s", SDL_GetError()); shader_stack_available = false; goto unavailable; }
            if (!hermite) { _log(warning, "Shader compilation error:\n%%s", SDL_GetError()); shader_stack_available = false; goto unavailable; }
            if (!process) { _log(warning, "Shader compilation error:\n%%s", SDL_GetError()); shader_stack_available = false; goto unavailable; }
            if (!curvature) { _log(warning, "Shader compilation error:\n%%s", SDL_GetError()); shader_stack_available = false; goto unavailable; }

            unavailable:
            if (!shader_stack_available) {
                _log(warning, "Shader stack unavailable, continuing with default rendering");

                return;
            }

            for (std::vector <uint32_t>& b : buffers)
                b.resize((PPU_WIDTH * window_scale) * (PPU_HEIGHT * window_scale));

            SDL_ShaderBindRawTexture(encoder, nullptr, PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale);
            SDL_ShaderBindRawTexture(decoder, buffers[0].data(), PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale);
            SDL_ShaderBindRawTexture(hermite, buffers[1].data(), PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale);
            SDL_ShaderBindRawTexture(process, buffers[2].data(), PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale);
            SDL_ShaderBindRawTexture(curvature, buffers[3].data(), PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale);

            if (!SDL_CreateShaderFramebuffer(encoder)) { _log(debug, "encoder: %s", SDL_GetError()); }
            if (!SDL_CreateShaderFramebuffer(decoder)) { _log(debug, "decoder: %s", SDL_GetError()); }
            if (!SDL_CreateShaderFramebuffer(hermite)) { _log(debug, "hermite: %s", SDL_GetError()); }
            if (!SDL_CreateShaderFramebuffer(process)) { _log(debug, "process: %s", SDL_GetError()); }

            if (ntsc_codec_enabled)
                SDL_GL_SetSwapInterval(1);
            
            ntsc_codec_enabled = false;
        }

        float time = 0.0;

        void set_texture_xres(int xres) {
            int w = 0;

            SDL_QueryTexture(texture, nullptr, nullptr, &w, nullptr);

            if (w != xres) {
                SDL_DestroyTexture(texture);

                texture = SDL_CreateTexture(
                    renderer,
                    SDL_PIXELFORMAT_RGBA8888,
                    SDL_TEXTUREACCESS_STREAMING,
                    xres, PPU_HEIGHT
                );
            }
        }

        int old_hires = 0;

        bool lctrl_pressed = false;

        void update(uint32_t* buf, int hires) {
            if (old_hires != hires)
                set_texture_xres(hires ? PPU_WIDTH_HIRES : PPU_WIDTH);

            old_hires = hires;

            frames_rendered++;
            end = std::chrono::high_resolution_clock::now();

            std::chrono::duration <double> d = end - start;

            if (std::chrono::duration_cast<std::chrono::seconds>(d).count() == 1) {
                static char buf[100];

                std::sprintf(&buf[0], "Sly fps: %u", frontend::window::get_fps());

                SDL_SetWindowTitle(frontend::window::window, buf);

                fps = frames_rendered;
                frames_rendered = 0;
                start = std::chrono::high_resolution_clock::now();
                end = start;
            }

            SDL_RenderClear(renderer);

            if (shader_stack_available && ntsc_codec_enabled) {
                scale_and_flip_buf(buf, window_scale);

                SDL_ShaderBindRawTexture(encoder, scaled.data(), PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale);
                SDL_ShaderSetUniform1I(encoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(decoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(hermite, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(process, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1F(encoder, "iTime", time);
                SDL_ShaderSetUniform1F(decoder, "iTime", time);
                SDL_ShaderSetUniform1F(hermite, "iTime", time);
                SDL_ShaderSetUniform1F(process, "iTime", time);
                SDL_UseShaderWithFramebuffer(encoder, buffers[0].data());
                SDL_ShaderSetUniform1I(encoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(decoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(hermite, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(process, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1F(encoder, "iTime", time);
                SDL_ShaderSetUniform1F(decoder, "iTime", time);
                SDL_ShaderSetUniform1F(hermite, "iTime", time);
                SDL_ShaderSetUniform1F(process, "iTime", time);
                SDL_UseShaderWithFramebuffer(decoder, buffers[1].data());
                SDL_ShaderSetUniform1I(encoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(decoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(hermite, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(process, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1F(encoder, "iTime", time);
                SDL_ShaderSetUniform1F(decoder, "iTime", time);
                SDL_ShaderSetUniform1F(hermite, "iTime", time);
                SDL_ShaderSetUniform1F(process, "iTime", time);
                SDL_UseShaderWithFramebuffer(hermite, buffers[2].data());
                SDL_ShaderSetUniform1I(encoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(decoder, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(hermite, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1I(process, "iFrame", frames_rendered);
                SDL_ShaderSetUniform1F(encoder, "iTime", time);
                SDL_ShaderSetUniform1F(decoder, "iTime", time);
                SDL_ShaderSetUniform1F(hermite, "iTime", time);
                SDL_ShaderSetUniform1F(process, "iTime", time);
                SDL_UseShader(process);
                // SDL_UseShaderWithFramebuffer(process, buffers[3].data());
                // SDL_ShaderSetUniform1I(encoder, "iFrame", frames_rendered);
                // SDL_ShaderSetUniform1I(decoder, "iFrame", frames_rendered);
                // SDL_ShaderSetUniform1I(hermite, "iFrame", frames_rendered);
                // SDL_ShaderSetUniform1I(process, "iFrame", frames_rendered);
                // SDL_ShaderSetUniform1F(encoder, "iTime", time);
                // SDL_ShaderSetUniform1F(decoder, "iTime", time);
                // SDL_ShaderSetUniform1F(hermite, "iTime", time);
                // SDL_ShaderSetUniform1F(process, "iTime", time);
    
                // SDL_UseShader(curvature);
                //SDL_UseShaderWithFramebuffer(curvature, buffers[4].data());

                time += 1.0f / 60.0f;
            } else {
                SDL_UpdateTexture(
                    texture,
                    NULL,
                    buf,
                    (hires ? PPU_WIDTH_HIRES : PPU_WIDTH) * sizeof(uint32_t)
                );

                SDL_RenderCopy(renderer, texture, NULL, NULL);
            }

            if (ntsc_codec_enabled) {
                SDL_GL_SwapWindow(window);
            } else {
                SDL_RenderPresent(renderer);
            }

            SDL_Event event;

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT: { close(); } break;
                    case SDL_KEYDOWN: {
                        if (event.key.keysym.sym == SDLK_LCTRL) {
                            lctrl_pressed = true;
                        }
                        if ((event.key.keysym.sym == SDLK_r) && lctrl_pressed) {
                            snes::cpu::init();
                        }
                        if (event.key.keysym.sym == SDLK_F1) {
                            dump_vram = true;
                        }
                        if (event.key.keysym.sym == SDLK_F2) {
                            ntsc_codec_enabled = !ntsc_codec_enabled;
                        }
                        if (event.key.keysym.sym == SDLK_F3) {
                            std::vector <uint32_t> alt;

                            alt.resize((PPU_WIDTH * window_scale) * (PPU_HEIGHT * window_scale));

                            stbir_resize_uint8(
                                reinterpret_cast<unsigned char*>(buf),
                                PPU_WIDTH, PPU_HEIGHT,
                                PPU_WIDTH * 4,
                                reinterpret_cast<unsigned char*>(alt.data()),
                                PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale,
                                PPU_WIDTH * window_scale * 4,
                                4
                            );

                            stbi_write_png(
                                "screenshots/ss.png",
                                PPU_WIDTH * window_scale, PPU_HEIGHT * window_scale,
                                4,
                                reinterpret_cast<unsigned char*>(alt.data()),
                                PPU_WIDTH * window_scale * 4
                            );
                        }

                        switch (event.key.keysym.sym) {
                            case SDLK_a     : { snes::controllers::keydown(CTRL_A, 0); } break;
                            case SDLK_s     : { snes::controllers::keydown(CTRL_B, 0); } break;
                            case SDLK_z     : { snes::controllers::keydown(CTRL_X, 0); } break;
                            case SDLK_y     : { snes::controllers::keydown(CTRL_Y, 0); } break;
                            case SDLK_q     : { snes::controllers::keydown(CTRL_L, 0); } break;
                            case SDLK_w     : { snes::controllers::keydown(CTRL_R, 0); } break;
                            case SDLK_RETURN: { snes::controllers::keydown(CTRL_START, 0); } break;
                            case SDLK_e     : { snes::controllers::keydown(CTRL_SELECT, 0); } break;
                            case SDLK_UP    : { snes::controllers::keydown(CTRL_UP, 0); } break;
                            case SDLK_DOWN  : { snes::controllers::keydown(CTRL_DOWN, 0); } break;
                            case SDLK_LEFT  : { snes::controllers::keydown(CTRL_LEFT, 0); } break;
                            case SDLK_RIGHT : { snes::controllers::keydown(CTRL_RIGHT, 0); } break;
                        }
                    } break;

                    case SDL_KEYUP: {
                        switch (event.key.keysym.sym) {
                            case SDLK_LCTRL : { lctrl_pressed = false; } break;
                            case SDLK_a     : { snes::controllers::keyup(CTRL_A, 0); } break;
                            case SDLK_s     : { snes::controllers::keyup(CTRL_B, 0); } break;
                            case SDLK_z     : { snes::controllers::keyup(CTRL_X, 0); } break;
                            case SDLK_y     : { snes::controllers::keyup(CTRL_Y, 0); } break;
                            case SDLK_q     : { snes::controllers::keyup(CTRL_L, 0); } break;
                            case SDLK_w     : { snes::controllers::keyup(CTRL_R, 0); } break;
                            case SDLK_RETURN: { snes::controllers::keyup(CTRL_START, 0); } break;
                            case SDLK_e     : { snes::controllers::keyup(CTRL_SELECT, 0); } break;
                            case SDLK_UP    : { snes::controllers::keyup(CTRL_UP, 0); } break;
                            case SDLK_DOWN  : { snes::controllers::keyup(CTRL_DOWN, 0); } break;
                            case SDLK_LEFT  : { snes::controllers::keyup(CTRL_LEFT, 0); } break;
                            case SDLK_RIGHT : { snes::controllers::keyup(CTRL_RIGHT, 0); } break;
                        }
                    } break;
                }
            }
        }
    }
}