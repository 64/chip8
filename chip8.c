#include <SDL2/SDL.h>
#include <stdio.h>
#include "macros.h"

//gcc -I/usr/local/include/SDL2 -D_THREAD_SAFE -L/usr/local/lib -lSDL2 chip8.c -o chip8 && ./chip8

SDL_Window *window;
SDL_Renderer *renderer;

unsigned short opcode;
unsigned char memory[4096];
unsigned char regs[16];
unsigned short reg_i;
unsigned short reg_pc;
unsigned char gfx[64 * 32];
unsigned char delay_timer;
unsigned char sound_timer;
unsigned char stack[16];
unsigned char reg_sp;
unsigned char draw_flag;

short running;

int init_graphics();
int destroy_graphics();
void input_loop();

void stack_init();
void stack_push();
void stack_clear();

void chip8_cycle();
void chip8_initmem();
void chip8_runop();

int main(int argc, char *argv[]) {
    log_info("Booting emulator...");
    if (!init_graphics()) {
        log_err("Graphics initialisation failed. Exiting");
        return 1;
    }
    log_info("Graphics successfully initialised.");

    running = 1;
    while(running) {
        input_loop();
        chip8_cycle();
    }

    if (!destroy_graphics()) {
        log_err("Graphics destruction failed. Exiting");
        return 1;
    }
    return 0;
}

int init_graphics() {
    check_sdl(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) >= 0, "Failed to initialise SDL");

    window = SDL_CreateWindow("CHIP-8 Emulator",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    check_sdl(window != NULL, "Window creation failed.");

    renderer = SDL_CreateRenderer(window, -1, 0);
    check_sdl(renderer != NULL, "Renderer creation failed.");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return 1;
error:
    return 0;
}

void input_loop() {
    SDL_Event ev;

    while (SDL_PollEvent(&ev) != 0) {
        switch(ev.type) {
            case SDL_QUIT:
                running = 0;
                break;
            default:
                break;
        }
    }
}

void chip8_initmem() {
    reg_pc = 0x200;
    opcode = 0x0;
    reg_i = 0x0;
    reg_sp = 0x0;
    // All registers and other memory is set to zero as they are globals
}

void chip8_cycle() {
    opcode = memory[reg_pc] << 8 || memory[reg_pc + 1];
    chip8_runop();
    if (delay_timer > 0)
        --delay_timer;
    if (sound_timer > 0) {
        if (sound_timer == 1) {
            // Beep
        }
        --sound_timer;
    }
}

void chip8_runop() {
    // Function pointer table stuff
}

int destroy_graphics() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
}
