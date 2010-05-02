#ifndef RAAMWERK_H
#define RAAMWERK_H



#include <windows.h>
#include <al/al.h>
#include <al/alc.h>
#include <gl/gl.h>
#include <gl/glext.h>
#include <gl/wglext.h>



#define RW_INSTANCE_CLASS "Raamwerk"

#define RW_NULL 0
#define RW_FALSE 0
#define RW_TRUE 1

#define RW_VSYNC 0x1001

#define RW_JOY_TOTAL_BUTTONS 32
#define RW_KB_TOTAL_BUTTONS 100
#define RW_MOUSE_TOTAL_BUTTONS 3



typedef struct raamwerk_t {
    struct joy_t {
        int x, y;
        char down[RW_JOY_TOTAL_BUTTONS];
        char pressed[RW_JOY_TOTAL_BUTTONS];
        char released[RW_JOY_TOTAL_BUTTONS];
        char last[RW_JOY_TOTAL_BUTTONS];
    } joy;

    struct kb_t {
        char down[RW_KB_TOTAL_BUTTONS];
        char pressed[RW_KB_TOTAL_BUTTONS];
        char released[RW_KB_TOTAL_BUTTONS];
        char last[RW_KB_TOTAL_BUTTONS];
    } kb;

    struct mouse_t {
        int x, y;
        char down[RW_MOUSE_TOTAL_BUTTONS];
        char pressed[RW_MOUSE_TOTAL_BUTTONS];
        char released[RW_MOUSE_TOTAL_BUTTONS];
        char last[RW_MOUSE_TOTAL_BUTTONS];
    } mouse;

    struct display_t {
        int width, height, fullscreen;
        int colorbits, alphabits, depthbits, stencilbits;
        int multisample;
        int is_active;
    } display;

    struct sound_t {
        ALfloat list_pos[3], list_ori[6];
        int is_active;
    } sound;
} raamwerk_t;



void rwAudio();
void rwCloseFunc( void (*fp)() );
void rwDisable( int var );
void rwDisplay( const char *title, int width, int height, int fullscreen, int colorbits, int alphabits, int depthbits, int stencilbits, int multisample );
void rwEnable( int var );
void rwInitFunc( void (*fp)( int argc, char **argv ) );
void rwLoopFunc( void (*fp)() );
void rwResizeFunc( void (*fp)() );
int rwRun();
raamwerk_t *rwStat();



#endif
