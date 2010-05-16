//
//  AppDelegate.h
//  Raamwerk
//
//  Created by abitvin on 5/13/10.
//  Copyright Vincent van Ingen 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GLView.h"
#import "raamwerk.h"

@class GLView;

@interface AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    GLView *glview;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) GLView *glview;

@end

