//
//  AppDelegate.m
//  Raamwerk
//
//  Created by abitvin on 5/13/10.
//  Copyright Vincent van Ingen 2010. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window;
@synthesize glview;


- (void)applicationDidBecomeActive:(UIApplication *)application
{
	raamwerk_t *rw = rwStat();
	rw->is_active = RW_TRUE;
	
	glview.animation_interval = 1.0f / 60.0f;
}


- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	glview = [[GLView alloc] initWithFrame:window.bounds];
	[window addSubview:glview];
	[window makeKeyAndVisible];
	
	glview.animation_interval = 1.0f / 60.0f;
	[glview startAnimation];
}


- (void)applicationWillResignActive:(UIApplication *)application
{
	raamwerk_t *rw = rwStat();
	rw->is_active = RW_FALSE;
	
	glview.animation_interval = 1.0f / 5.0f;
}


-(void)applicationWillTerminate:(UIApplication *)application
{
	rwDestroy();
} 


- (void)dealloc
{
	[window release];
	[glview release];
	[super dealloc];
}

@end
