//
//  GLView.h
//  Raamwerk
//
//  Created by abitvin on 5/13/10.
//  Copyright Vincent van Ingen 2010. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>
#include <raamwerk.h>


/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface GLView : UIView
{    
	@private
		GLint back_width, back_height;
        EAGLContext *context;
        GLuint view_renderbuffer, view_framebuffer, depth_renderbuffer;
        NSTimer *animation_timer;
		NSTimeInterval animation_interval;
		CGPoint touch_pos;
}

@property NSTimeInterval animation_interval;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView;

@end
