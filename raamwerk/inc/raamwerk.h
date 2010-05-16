#ifndef RAAMWERK_H
#define RAAMWERK_H



#ifdef _WIN32
    #define RW_WINDOWS 1
	#include <AL/al.h>
	#include <AL/alc.h>
	#include <GL/gl.h>
	#include <GL/glext.h>
    #include <windows.h>
    #include <gl/wglext.h>
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#define RW_IPHONE 1
		#import <UIKit/UIKit.h>
		#import <OpenAl/al.h>
		#import <OpenAL/alc.h>
		//#import <OpenAL/oalStaticBufferExtension.h>
		#import <OpenGLES/EAGL.h>
		#import <OpenGLES/ES1/gl.h>
		#import <OpenGLES/ES1/glext.h>
	#else
		#define RW_MAC 1
		//TODO Apple Mac
	#endif
#else
	#define RW_LINUX 1
	#include <AL/al.h>
	#include <AL/alc.h>
	#include <GL/gl.h>
	#include <GL/glx.h>
	#include <GL/glext.h>
    #include <stdio.h>
    #include <string.h>
    #include <X11/X.h>
    #include <X11/keysym.h>
#endif



#define RW_INSTANCE_CLASS "Raamwerk" // Windows only

#define RW_NULL 0
#define RW_FALSE 0
#define RW_TRUE 1

#define RW_VSYNC 0x1001

#define RW_JOY_TOTAL_BUTTONS 32
#define RW_KB_TOTAL_BUTTONS 100
#define RW_MOUSE_TOTAL_BUTTONS 3
#define RW_TOUCH_TOTAL_TOUCHES 5
#define RW_HIT_TOTAL_AREAS 10



typedef struct raamwerk_t {
	int is_active;	// iPhone: application(DidBecome|WillResign)Active
	
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

	struct touch_t {
		int num_touches;
		struct point_t {
			int x, y;
		} point[RW_TOUCH_TOTAL_TOUCHES];
	} touch;

    struct hit_t {
		int num_areas;
		struct area_t {
			int x, y, width, height;
			char down, pressed, released, last;
		} area[RW_HIT_TOTAL_AREAS];
	} hit;

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


#if defined(__cplusplus)
extern "C" {
#endif

void rwAudio();
void rwDestroy();
void rwDestroyFunc( void (*fp)() );
void rwDisable( int var );
void rwDisplay( const char *title, int width, int height, int fullscreen, int colorbits, int alphabits, int depthbits, int stencilbits, int multisample );
void rwEnable( int var );
void rwInit();
void rwInitFunc( void (*fp)( int argc, char **argv ) );
void rwLoop();
void rwLoopFunc( void (*fp)() );
void rwResize();
void rwResizeFunc( void (*fp)() );
int rwRun();
raamwerk_t *rwStat();

#if defined(__cplusplus)
}
#endif



#endif
