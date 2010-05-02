#include <raamwerk.h>
#include <myapp.h>


int main( int argc, char **argv )
{
    rwCloseFunc( &close );
    rwInitFunc( &init );
	rwLoopFunc( &loop );
	rwResizeFunc( &resize );
    return rwRun();
}
