#include <windows.h>
#include <raamwerk.h>
#include <myapp.h>


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    rwDestroyFunc( &destroy );
    rwInitFunc( &init );
	rwLoopFunc( &loop );
	rwResizeFunc( &resize );
    return rwRun();
}
