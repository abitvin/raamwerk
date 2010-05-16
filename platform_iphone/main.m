//
//  main.m
//  Raamwerk
//
//  Created by vin777 on 5/13/10.
//  Copyright Vincent van Ingen 2010. All rights reserved.
//

//#import <UIKit/UIKit.h>
#include <raamwerk.h>
#include <myapp.h>

int main( int argc, char **argv )
{    
    /*
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain( argc, argv, nil, @"AppDelegate" );
    [pool release];
    return retVal;
	 */
	
	rwDestroyFunc( &destroy );
    rwInitFunc( &init );
	rwLoopFunc( &loop );
	rwResizeFunc( &resize );
    return rwRun();
}
