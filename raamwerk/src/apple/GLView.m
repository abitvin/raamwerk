//
//  GLView.m
//  Raamwerk
//
//  Created by abitvin on 5/13/10.
//  Copyright Vincent van Ingen 2010. All rights reserved.
//



#import "GLView.h"

#define USE_DEPTH_BUFFER 1
//#define DEGREES_TO_RADIANS(__ANGLE) ((__ANGLE) / 180.0 * M_PI)

// A class extension to declare private methods
@interface GLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animation_timer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end


@implementation GLView

@synthesize context;
@synthesize animation_timer;
@synthesize animation_interval;



- (BOOL)createFramebuffer
{    
    glGenFramebuffersOES( 1, &view_framebuffer );
    glGenRenderbuffersOES( 1, &view_renderbuffer );
    
    glBindFramebufferOES( GL_FRAMEBUFFER_OES, view_framebuffer );
    glBindRenderbufferOES( GL_RENDERBUFFER_OES, view_renderbuffer );
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, view_renderbuffer );
    
    glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &back_width );
    glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &back_height );
    
    if( USE_DEPTH_BUFFER )
	{
        glGenRenderbuffersOES( 1, &depth_renderbuffer );
        glBindRenderbufferOES( GL_RENDERBUFFER_OES, depth_renderbuffer );
        glRenderbufferStorageOES( GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, back_width, back_height );
        glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depth_renderbuffer );
    }
    
    if( glCheckFramebufferStatusOES( GL_FRAMEBUFFER_OES ) != GL_FRAMEBUFFER_COMPLETE_OES )
	{
        NSLog( @"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES( GL_FRAMEBUFFER_OES ) );
        return NO;
    }
    
    return YES;
}


- (void)dealloc
{    
    [self stopAnimation];
    
	if( [EAGLContext currentContext] == context ) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];  
    [super dealloc];
}


- (void)destroyFramebuffer
{   
	glDeleteFramebuffersOES( 1, &view_framebuffer );
    view_framebuffer = 0;
    
	glDeleteRenderbuffersOES( 1, &view_renderbuffer );
    view_renderbuffer = 0;
    
    if( depth_renderbuffer )
	{
        glDeleteRenderbuffersOES( 1, &depth_renderbuffer );
        depth_renderbuffer = 0;
    }
}


- (void)drawView
{
	[EAGLContext setCurrentContext:context];
	glBindFramebufferOES( GL_FRAMEBUFFER_OES, view_framebuffer );
	
	rwLoop();
	
	glBindRenderbufferOES( GL_RENDERBUFFER_OES, view_renderbuffer );
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}


// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}


- (id)initWithFrame:(CGRect)frame
{
	if( ( self = [super initWithFrame:frame] ) )
	{
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if( !context || ![EAGLContext setCurrentContext:context] )
		{
            [self release];
            return nil;
        }
        
        animation_interval = 1.0f / 60.0f;
		
		rwInit();
		rwResize();
    }
	
    return self;
}


- (void)layoutSubviews
{
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}


- (void)setAnimationInterval:(NSTimeInterval)interval
{
    animation_interval = interval;
    
	if( animation_timer )
	{
        [self stopAnimation];
        [self startAnimation];
    }
}


- (void)setAnimationTimer:(NSTimer *)new_timer
{
    [animation_timer invalidate];
    animation_timer = new_timer;
}


- (void)startAnimation
{
    self.animation_timer = [NSTimer scheduledTimerWithTimeInterval:animation_interval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation
{
    self.animation_timer = nil;
}


-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	int i;
	
	UITouch *touch;
	CGPoint point;
	NSSet *all_touches = [event allTouches];
	
	raamwerk_t *rw = rwStat();
	rw->touch.num_touches = 0;
	
	for( i = 0; i < [all_touches count]; i++ )
	{
		touch = [[all_touches allObjects] objectAtIndex:i];
		
		if( [touch phase] == UITouchPhaseEnded || [touch phase] == UITouchPhaseCancelled ) {
			continue;
		}
		
		point = [touch locationInView:self];
		rw->touch.point[rw->touch.num_touches].x = point.x;
		rw->touch.point[rw->touch.num_touches].y = point.y;
		rw->touch.num_touches++;
		
		if( rw->touch.num_touches == 1 )
		{
			rw->mouse.x = point.x;
			rw->mouse.y = point.y;
			rw->mouse.down[0] = RW_TRUE;
		}
	}
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	int i;
	
	UITouch *touch;
	CGPoint point;
	NSSet *all_touches = [event allTouches];
		
	raamwerk_t *rw = rwStat();
	rw->touch.num_touches = 0;
	
	for( i = 0; i < [all_touches count]; i++ )
	{
		touch = [[all_touches allObjects] objectAtIndex:i];
		
		if( [touch phase] == UITouchPhaseEnded || [touch phase] == UITouchPhaseCancelled ) {
			continue;
		}
		
		point = [touch locationInView:self];
		rw->touch.point[rw->touch.num_touches].x = point.x;
		rw->touch.point[rw->touch.num_touches].y = point.y;
		rw->touch.num_touches++;
		
		if( rw->touch.num_touches == 1 )
		{
			rw->mouse.x = point.x;
			rw->mouse.y = point.y;
		}
	}
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	int i;
	
	UITouch *touch;
	CGPoint point;
	NSSet *all_touches = [event allTouches];
	
	raamwerk_t *rw = rwStat();
	rw->touch.num_touches = 0;
	
	for( i = 0; i < [all_touches count]; i++ )
	{
		touch = [[all_touches allObjects] objectAtIndex:i];
		
		if( [touch phase] == UITouchPhaseEnded || [touch phase] == UITouchPhaseCancelled ) {
			continue;
		}
		
		point = [touch locationInView:self];
		rw->touch.point[rw->touch.num_touches].x = point.x;
		rw->touch.point[rw->touch.num_touches].y = point.y;
		rw->touch.num_touches++;
		
		if( rw->touch.num_touches == 1 )
		{
			rw->mouse.x = point.x;
			rw->mouse.y = point.y;
			rw->mouse.down[0] = RW_FALSE;
		}
	}
}


- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	raamwerk_t *rw = rwStat();
	rw->touch.num_touches = 0;
	rw->mouse.down[0] = RW_FALSE;
}


@end
