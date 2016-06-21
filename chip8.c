#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include "macros.h"

//OSX: gcc -I/usr/local/include/SDL2 -D_THREAD_SAFE -L/usr/local/lib -lSDL2 chip8.c -o chip8 && ./chip8
//LINUX: gcc chip8.c -o chip8 -I/usr/include/SDL2 -D_REENTRANT -L/usr/lib/x86_64-linux-gnu -lSDL2 && ./chip8

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
unsigned char keys[16];
unsigned short reg_sp;
unsigned char draw_flag;

short running;
short last_pressed;

int init_graphics();
int destroy_graphics();
int input_loop();

void stack_push(unsigned short x);
unsigned short stack_pop();

void chip8_cycle();
void chip8_initmem();
void chip8_runop();
void chip8_draw();
void chip8_clear();

int main(int argc, char *argv[]) {
    log_info("Booting emulator...");
    if (!init_graphics()) {
        log_err("Graphics initialisation failed. Exiting");
        return 1;
    }
    log_info("Graphics successfully initialised.");

    srand(time(NULL));
    chip8_initmem();
    running = 1;

    while(running) {
        input_loop();
        chip8_cycle();
        chip8_draw();
    }

    log_info("Shutting down...");
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

int input_loop() {
    SDL_Event ev;

    while (SDL_PollEvent(&ev) != 0) {
        switch(ev.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                KEYSWITCH(1);
                return 1;
            case SDL_KEYUP:
                KEYSWITCH(0);
                break;
        }
    }
    return 0;
}

void chip8_draw() {
    if (draw_flag != 1)
        return;

    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Loop through each gfx, update pixel on screen
    int i;
    SDL_Rect r = { 0, 0, TILE_WIDTH, TILE_HEIGHT };
    for (i = 0; i < GFX_MAX; i++) {
        if (gfx[i] != 1)
            continue;
        log_info("Read 1 value");
        r.x = (i * 10) % WINDOW_WIDTH;
        r.y = (i / (WINDOW_WIDTH / 10)) * 10;
        SDL_RenderFillRect(renderer, &r);
    }
    SDL_RenderPresent(renderer);
    draw_flag = 0;
}

void chip8_clear() {
    int i;
    for (i = 0; i < GFX_MAX; i++)
        gfx[i] = 0;
    draw_flag = 1;
}

int destroy_graphics() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
}

unsigned char chip8_fontset[FONTSET_SIZE] = {
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
    if (opcode == 0x00E0) {
        // Clear screen
        chip8_clear();
        reg_pc += 2;
    } else if (opcode == 0x00EE) {
        reg_pc = stack_pop();
    } else {
        // Ignored, 0x0nnn
    }
}

void opcode1() {
    reg_pc = (opcode & 0x0FFF);
}

void opcode2() {
    stack_push(reg_pc);
    reg_pc = (opcode & 0x0FFF);
}

void opcode3() {
    if (regs[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        reg_pc += 4;
    else
        reg_pc += 2;
}

void opcode4() {
    if (regs[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        reg_pc += 4;
    else
        reg_pc += 2;
}

void opcode5() {
    if (regs[(opcode & 0x0F00) >> 8] == regs[(opcode & 0x00F0) >> 4])
        reg_pc += 4;
    else
        reg_pc += 2;
}

void opcode6() {
    regs[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
    reg_pc += 2;
}

void opcode7() {
    regs[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
    reg_pc += 2;
}

// Arithmetic here:

void opcode9() {
    if (regs[(opcode & 0x0F00) >> 8] != regs[(opcode & 0x00F0) >> 4])
        reg_pc += 4;
    else
        reg_pc += 2;
}

void opcodeA() {
    reg_i = (opcode & 0x0FFF);
    reg_pc += 2;
}

void opcodeB() {
    reg_pc = (opcode & 0x0FFF) + regs[0];
}

void opcodeC() {
    regs[(opcode & 0x0F00) >> 8] = rand() & (opcode & 0x00FF);
    reg_pc += 2;
}

void opcodeD() {

}

void opcodeE() {
    if ((opcode & 0x00FF) == 0x9E && keys[regs[(opcode & 0x0F00) >> 8]] == 1)
        reg_pc += 4;
    else if ((opcode & 0x00FF) == 0xA1 && keys[regs[(opcode & 0x0F00) >> 8]] == 0)
        reg_pc += 4;
    else
        reg_pc += 2;
}

void opcodeF() {
    int i;
    switch(opcode & 0x00FF) {
        case 0x0007:
            regs[(opcode & 0x0F00) >> 8] = delay_timer;
            break;
        case 0x000A:
            while (input_loop() != 1)
                ;
            regs[(opcode & 0x0F00) >> 8] = last_pressed;
            break;
        case 0x0015:
            delay_timer = regs[(opcode & 0x0F00) >> 8];
            break;
        case 0x0018:
            sound_timer = regs[(opcode & 0x0F00) >> 8];
            break;
        case 0x001E:
            reg_i += regs[(opcode & 0x0F00) >> 8];
            break;
        case 0x0029:
            reg_i = 0x50 + (regs[(opcode & 0x0F00) >> 8] * FONTSET_HEIGHT);
            break;
        case 0x0033:
            memory[reg_i] = regs[(opcode & 0x0F00) >> 8] % 1000 / 100;
            memory[reg_i + 1] = regs[(opcode & 0x0F00) >> 8] % 100 / 10;
            memory[reg_i + 2] = regs[(opcode & 0x0F00) >> 8] % 10;
            break;
        case 0x0055:
            for (i = 0; i < ((opcode & 0x0F00) >> 8); i++)
                memory[reg_i + i] = regs[i];
            break;
        case 0x0065:
            for (i = 0; i < ((opcode & 0x0F00) >> 8); i++)
                regs[i] = memory[reg_i + i];
            break;
    }
    reg_pc += 2;
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
