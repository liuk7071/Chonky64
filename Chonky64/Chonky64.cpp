#include <iostream>
#include "n64.h"
#include "SDL.h"
#undef main

int main() {
    n64 N64;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    const auto window = SDL_CreateWindow("Chonky64", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, SDL_WINDOW_SHOWN);
    const auto renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderClear(renderer);

    auto frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA5551, SDL_TEXTUREACCESS_STATIC, 1024, 1024 * 0.75);

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        if (N64.Cpu.cycles >= 10000) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                }
            }
            if (N64.Memory.VI->update_texture) {
                SDL_DestroyTexture(frame);
                frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, N64.VI.width, N64.VI.width * 0.75);
                N64.Memory.VI->update_texture = false;
            }
            SDL_UpdateTexture(frame, NULL, &N64.Memory.rdram[N64.VI.origin & 0x7fffff], N64.VI.width * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, frame, NULL, NULL);
            SDL_RenderPresent(renderer);
            N64.Cpu.cycles = 0;
            //printf("pc: 0x%08x\n", N64.Cpu.pc);
            //std::ofstream file("rdram.bin", std::ios::binary);
            //file.write((const char*)N64.Memory.rdram, 0x800000);
        }
        N64.Cpu.step();
    }
}