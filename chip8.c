#include <SDL2/SDL.h>
#include <stdio.h>
#include "macros.h"

//gcc -I/usr/local/include/SDL2 -D_THREAD_SAFE -L/usr/local/lib -lSDL2 chip8.c -o chip8 && ./chip8
//gcc chip8.c -o chip8 -I/usr/include/SDL2 -D_REENTRANT -L/usr/lib/x86_64-linux-gnu -lSDL2 && ./chip8

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
unsigned short stack[16];
unsigned short reg_sp;
unsigned char draw_flag;

short running;

int init_graphics();
int destroy_graphics();
void input_loop();

void stack_push(unsigned short x);
unsigned short stack_pop();

void chip8_cycle();
void chip8_initmem();
void chip8_runop();
void chip8_draw();

int main(int argc, char *argv[]) {
    log_info("Booting emulator...");
    if (!init_graphics()) {
        log_err("Graphics initialisation failed. Exiting");
        return 1;
    }
    log_info("Graphics successfully initialised.");

    chip8_initmem();
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

void chip8_draw() {

}

int destroy_graphics() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
}

unsigned char chip8_fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void chip8_initmem() {
    reg_pc = 0x200;
    opcode = 0x0;
    reg_i = 0x0;
    reg_sp = 0x0;
    // All registers and other memory is set to zero as they are globals

    int i;
    for(i = 0x50; i < FONTSET_SIZE + 0x50; i++)
        memory[i] = chip8_fontset[i];

    memory[reg_pc] = 0xA4;
    memory[reg_pc + 1] = 0x28;
}

void chip8_cycle() {
    opcode = (memory[reg_pc] << 8) | memory[reg_pc + 1];
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

void stack_push(unsigned short x) {
    stack[reg_sp++] = x;
}

unsigned short stack_pop() {
    if (reg_sp == 0)
        return stack[reg_sp];
    return stack[--reg_sp];
}

void opcode0() {

}

void opcode1() {

}

void opcode2() {

}

void opcode3() {

}

void opcode4() {

}

void opcode5() {

}

void opcode6() {

}

void opcode7() {

}

void opcode9() {

}

void opcodeA() {
    reg_i = (opcode & 0x0FFF);
    reg_pc += 2;
}

void opcodeB() {

}

void opcodeC() {

}

void opcodeD() {

}

void opcodeE() {

}

void opcodeF() {

}

// Placeholders
void cpuNULL() {
    // This should not be called
}

// Starting with 0x8...
void (*chip8_arithmetic[16])() =  {
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL,
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL
};

void cpuARITHMETIC() {
    chip8_arithmetic[opcode & 0x000F]();
}

void (*chip8_table[17])() = {
	opcode0      , opcode1, opcode2, opcode3, opcode4, opcode5, opcode6, opcode7,
	cpuARITHMETIC, opcode9, opcodeA, opcodeB, opcodeC, opcodeD, opcodeE, opcodeF,
	cpuNULL
};

void chip8_runop() {
    // Function pointer table stuff
    chip8_table[(opcode & 0xF000) >> 12]();
}
