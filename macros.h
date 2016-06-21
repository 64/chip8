#ifndef __macros_h__
#define __macros_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH 640
#define NUM_REGS 16
#define FONTSET_SIZE 80
#define FONTSET_HEIGHT 5
#define GFX_MAX (64 * 32)
#define TILE_WIDTH 10
#define TILE_HEIGHT 10


#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d) errno: %s " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d) errno: %s " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_sdl(A, M, ...) check(A, M ": %s", ##__VA_ARGS__, SDL_GetError())

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#define KEYSWITCH(t) switch(ev.key.keysym.sym) {  \
        case SDLK_1:        \
            keys[0x1] = t;  \
            if (t)          \
                last_pressed = 0x1; \
            break;          \
        case SDLK_2:        \
            keys[0x2] = t;  \
            if (t)          \
                last_pressed = 0x2; \
            break;          \
        case SDLK_3:        \
            keys[0x3] = t;  \
            if (t)          \
                last_pressed = 0x3; \
            break;          \
        case SDLK_4:        \
            keys[0xC] = t;  \
            if (t)          \
                last_pressed = 0xC; \
            break;          \
        case SDLK_q:        \
            keys[0x4] = t;  \
            if (t)          \
                last_pressed = 0x4; \
            break;          \
        case SDLK_w:        \
            keys[0x5] = t;  \
            if (t)          \
                last_pressed = 0x5; \
            break;          \
        case SDLK_e:        \
            keys[0x6] = t;  \
            if (t)          \
                last_pressed = 0x6; \
            break;          \
        case SDLK_r:        \
            keys[0xD] = t;  \
            if (t)          \
                last_pressed = 0xD; \
            break;          \
        case SDLK_a:        \
            keys[0x7] = t;  \
            if (t)          \
                last_pressed = 0x7; \
            break;          \
        case SDLK_s:        \
            keys[0x8] = t;  \
            if (t)          \
                last_pressed = 0x8; \
            break;          \
        case SDLK_d:        \
            keys[0x9] = t;  \
            if (t)          \
                last_pressed = 0x9; \
            break;          \
        case SDLK_f:        \
            keys[0xE] = t;  \
            if (t)          \
                last_pressed = 0xE; \
            break;          \
        case SDLK_z:        \
            keys[0xA] = t;  \
            if (t)          \
                last_pressed = 0xA; \
            break;          \
        case SDLK_x:        \
            keys[0x0] = t;  \
            if (t)          \
                last_pressed = 0x0; \
            break;          \
        case SDLK_c:        \
            keys[0xB] = t;  \
            if (t)          \
                last_pressed = 0xB; \
            break;          \
        case SDLK_v:        \
            keys[0xF] = t;  \
            if (t)          \
                last_pressed = 0xF; \
            break;          \
    };                      \

#endif
