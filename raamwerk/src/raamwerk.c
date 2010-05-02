#include "raamwerk.h"

#ifdef _WIN32
    LRESULT CALLBACK WinProc( HWND, UINT, WPARAM, LPARAM );
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    static HDC g_hdc = NULL;
    static HGLRC g_hrc = NULL;
    static HINSTANCE g_hinst = NULL;
    static HWND g_hwnd = NULL;
#else
    static Window win;
    static Display *dpy;
    static XEvent event;
#endif

static void (*g_fpClose)() = NULL;
static void (*g_fpInit)() = NULL;
static void (*g_fpLoop)() = NULL;
static void (*g_fpResize)() = NULL;
static ALCcontext *g_al_context;
static raamwerk_t g_rw;


void rwAudio()
{
    ALCdevice *dev = alcOpenDevice (NULL);

    if( dev == NULL ) {
        return;
    }

    g_al_context = alcCreateContext( dev, NULL );

    if( g_al_context == NULL )
    {
      alcCloseDevice( dev );
      return;
    }

    if( !alcMakeContextCurrent( g_al_context) )
    {
      alcDestroyContext( g_al_context );
      alcCloseDevice( dev );
      return;
    }

    alGetError();


    g_rw.sound.list_pos[0] = 0.0f;
    g_rw.sound.list_pos[1] = 0.0f;
    g_rw.sound.list_pos[2] = 0.0f;
    alListenerfv( AL_POSITION, g_rw.sound.list_pos );

    // 2D(x,y) sound orientation
	g_rw.sound.list_ori[0] = 0.0f;
	g_rw.sound.list_ori[1] = 1.0f;
	g_rw.sound.list_ori[2] = 0.0f;
	g_rw.sound.list_ori[3] = 0.0f;
	g_rw.sound.list_ori[4] = 0.0f;
	g_rw.sound.list_ori[5] = 1.0f;
	alListenerfv( AL_ORIENTATION, g_rw.sound.list_ori );

	g_rw.sound.is_active = RW_TRUE;
}


void rwCloseFunc( void (*fp)() )
{
    g_fpClose = fp;
}


void rwDisable( int var )
{
    switch( var )
    {
        case RW_VSYNC:
        {
            //TODO linux vsync
#ifdef _WIN32
            if( wglSwapIntervalEXT ) {
                wglSwapIntervalEXT( 0 );
            }
#endif
            break;
        }
    }
}


void rwDisplay( const char *title, int width, int height, int fullscreen, int colorbits, int alphabits, int depthbits, int stencilbits, int multisample )
{
#ifdef _WIN32
    if( fullscreen )
	{
		DEVMODE dm;
		memset( &dm, 0, sizeof(dm) );
		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;
		dm.dmBitsPerPel = colorbits + alphabits;
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if( ChangeDisplaySettings( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
		    MessageBox( NULL, "Fullscreen mode not supported", title, MB_OK );
			return;
		}
	}


    DWORD ex_style;
	DWORD style;
	RECT r;
	r.left = 0;
	r.right = width;
	r.top = 0;
	r.bottom = height;

	if( fullscreen )
	{
		ex_style = WS_EX_APPWINDOW;
		style = WS_POPUP;
		ShowCursor( FALSE );
	}
	else
	{
		ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		style = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx( &r, style, FALSE, ex_style );

	int win_width =  r.right - r.left;
	int win_height = r.bottom - r.top;

	if( !fullscreen )
	{
	    win_width = width + GetSystemMetrics( SM_CYFRAME ) * 2;
	    win_height = height + GetSystemMetrics( SM_CYFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION );
	}

    g_hwnd = CreateWindowEx(
        ex_style, RW_INSTANCE_CLASS, title, style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        win_width, win_height,
        NULL, NULL, g_hinst, NULL
    );


    g_hdc = GetDC( g_hwnd );

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory( &pfd, sizeof(pfd) );
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = colorbits + alphabits;
    pfd.cDepthBits = depthbits;
    pfd.cStencilBits = stencilbits;
    pfd.iLayerType = PFD_MAIN_PLANE;

    SetPixelFormat( g_hdc, ChoosePixelFormat( g_hdc, &pfd ), &pfd );
    g_hrc = wglCreateContext( g_hdc );
    wglMakeCurrent( g_hdc, g_hrc );


    // now we can get some OpenGL extensties.
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress( "wglChoosePixelFormatARB" );


    if( multisample > 0 )
    {
        wglMakeCurrent( g_hdc, NULL );
        wglDeleteContext( g_hrc );
        ReleaseDC( g_hwnd, g_hdc );
        DestroyWindow( g_hwnd );

        int attr_i[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, colorbits,
            WGL_ALPHA_BITS_ARB, alphabits,
            WGL_DEPTH_BITS_ARB, depthbits,
            WGL_STENCIL_BITS_ARB, stencilbits,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
            WGL_SAMPLES_ARB, multisample,
            0, 0
        };
        float attr_f[] = { 0.0f, 0.0f };
        int pixelformat;
        unsigned int num_formats;

        g_hwnd = CreateWindowEx(
            ex_style, RW_INSTANCE_CLASS, title, style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            win_width, win_height,
            NULL, NULL, g_hinst, NULL
        );

        g_hdc = GetDC( g_hwnd );
        wglChoosePixelFormatARB( g_hdc, attr_i, attr_f, 1, &pixelformat, &num_formats );
        SetPixelFormat( g_hdc, pixelformat, &pfd );
        g_hrc = wglCreateContext( g_hdc );
        wglMakeCurrent( g_hdc, g_hrc );

        glEnable( GL_MULTISAMPLE_ARB );
    }


    ShowWindow( g_hwnd, SW_SHOW ); // TODO nCmdShow
    SetForegroundWindow( g_hwnd );
	SetFocus( g_hwnd );

#else

    int dbl_buf[] = { GLX_RGBA, GLX_DEPTH_SIZE, depthbits, GLX_DOUBLEBUFFER, None };

    XVisualInfo *vi;
    Colormap cmap;
    XSetWindowAttributes swa;
    GLXContext cx;
    int dummy;


    if( ( dpy = XOpenDisplay( NULL ) ) == NULL )
    {
        printf( "Could not open display\n" );
        return;
    }

    if( !glXQueryExtension( dpy, &dummy, &dummy ) )
    {
        printf( "X server has no OpenGL GLX extension\n" );
        return;
    }

    if( ( vi = glXChooseVisual( dpy, DefaultScreen( dpy ), dbl_buf ) ) == NULL )
    {
        printf( "No RGB visual with depth buffer\n" );
        return;
    }

    if( vi->class != TrueColor )
    {
        printf( "TrueColor visual required for this program\n" );
        return;
    }

    if( ( cx = glXCreateContext( dpy, vi, None, GL_TRUE ) ) == NULL )
    {
        printf( "Could not create rendering context\n" );
        return;
    }

    cmap = XCreateColormap( dpy, RootWindow( dpy, vi->screen ), vi->visual, AllocNone );
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = KeyPressMask | KeyReleaseMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

    win = XCreateWindow(
        dpy, RootWindow( dpy, vi->screen ), 0, 0,
        width, height, 0, vi->depth, InputOutput, vi->visual,
        CWBorderPixel | CWColormap | CWEventMask, &swa
    );

    XSetStandardProperties( dpy, win, title, "main", None, NULL, 0, NULL );
    glXMakeCurrent( dpy, win, cx );
    XMapWindow( dpy, win );


#endif

    g_rw.display.width = width;
    g_rw.display.height = height;
    g_rw.display.fullscreen = fullscreen;
    g_rw.display.colorbits = colorbits;
    g_rw.display.depthbits = depthbits;
    g_rw.display.stencilbits = stencilbits;
    g_rw.display.multisample = multisample;
    g_rw.display.is_active = RW_TRUE;
}


void rwEnable( int var )
{
    switch( var )
    {
        case RW_VSYNC:
        {
            //TODO linux vsync
#ifdef _WIN32
            if( wglSwapIntervalEXT ) {
                wglSwapIntervalEXT( 1 );
            }
#endif

            break;
        }
    }
}


void rwInitFunc( void (*fp)( int argc, char **argv ) )
{
    g_fpInit = fp;
}


void rwLoopFunc( void (*fp)() )
{
    g_fpLoop = fp;
}


void rwResizeFunc( void (*fp)() )
{
    g_fpResize = fp;
}


int rwRun()
{
#ifdef _WIN32
    g_hinst = GetModuleHandle( NULL );

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WinProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hinst;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = RW_INSTANCE_CLASS;
    wcex.hIconSm = LoadIcon( NULL, IDI_APPLICATION );

    if( !RegisterClassEx( &wcex ) ) {
        return 0;
    }
#endif

    // init g_rw struct
    memset( &g_rw, 0, sizeof( raamwerk_t ) );

    if( g_fpInit ) {
        g_fpInit();
    }

    if( g_fpResize ) {
        g_fpResize();
    }

    // if we got no display, then quit immideatly.
    int quit = !g_rw.display.is_active;

#ifdef _WIN32
    JOYINFOEX joy;
    memset( &joy, 0, sizeof( joy ) );
    joy.dwSize = sizeof( joy );
    joy.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY;

    MSG msg;

    while( !quit )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if( msg.message == WM_QUIT )
            {
                quit = 1;
            }
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        else
        {
            int i;

            // keyboard
            for( i = 0; i < RW_KB_TOTAL_BUTTONS; i++ )
            {
                g_rw.kb.pressed[i] = g_rw.kb.down[i] == RW_TRUE && g_rw.kb.last[i] == RW_FALSE;
                g_rw.kb.released[i] = g_rw.kb.down[i] == RW_FALSE && g_rw.kb.last[i] == RW_TRUE;
                g_rw.kb.last[i] = g_rw.kb.down[i];
            }

            // mouse
            for( i = 0; i < RW_MOUSE_TOTAL_BUTTONS; i++ )
            {
                g_rw.mouse.pressed[i] = g_rw.mouse.down[i] == RW_TRUE && g_rw.mouse.last[i] == RW_FALSE;
                g_rw.mouse.released[i] = g_rw.mouse.down[i] == RW_FALSE && g_rw.mouse.last[i] == RW_TRUE;
                g_rw.mouse.last[i] = g_rw.mouse.down[i];
            }

            // joypad/joypad
            if( joyGetPosEx( JOYSTICKID1, &joy ) == JOYERR_NOERROR )
            {
                g_rw.joy.x = joy.dwXpos;
                g_rw.joy.y = joy.dwYpos;

                for( i = 0; i < RW_JOY_TOTAL_BUTTONS; i++ )
                {
                    g_rw.joy.down[i] = ( joy.dwButtons & ( 1 << i ) ) != 0;
                    g_rw.joy.pressed[i] = g_rw.joy.down[i] == RW_TRUE && g_rw.joy.last[i] == RW_FALSE;
                    g_rw.joy.released[i] = g_rw.joy.down[i] == RW_FALSE && g_rw.joy.last[i] == RW_TRUE;
                    g_rw.joy.last[i] = g_rw.joy.down[i];
                }
            }
            else
            {
                g_rw.joy.x = 32768;
                g_rw.joy.y = 32768;
            }

            // program loop
            if( g_fpLoop ) {
                g_fpLoop();
            }

            // swap buffer
            SwapBuffers( g_hdc );
        }
    }

#else

    //KeySym key;
    int key;

    while( !quit )
    {
        while( XPending( dpy ) > 0 )
        {
            XNextEvent( dpy, &event );

            switch( event.type )
            {
                case ButtonPress:
                {
                    g_rw.kb.down[event.xbutton.button] = RW_TRUE;
                    break;
                }

                case ButtonRelease:
                {
                    g_rw.kb.down[event.xbutton.button] = RW_FALSE;
                    break;
                }

                case ClientMessage:
                {
                    if( *XGetAtomName( dpy, event.xclient.message_type) == *"WM_PROTOCOLS" )
                    {
                        printf( "Exiting sanely...\n" );
                        quit = 1;
                    }
                    break;
                }

                case ConfigureNotify:
                {
                    glViewport( 0, 0, event.xconfigure.width, event.xconfigure.height );
                    break;
                }

                case Expose:
                {
                    if( event.xexpose.count != 0 ) {
                        break;
                    }

                    //drawGLScene();
                    break;
                }

                case KeyPress:
                {
                    key = XLookupKeysym( &event.xkey, 0 );

                    if( key < RW_KB_TOTAL_BUTTONS ) {
                        g_rw.kb.down[key] = RW_TRUE;
                    }

                    break;
                }

                case KeyRelease:
                {
                    key = XLookupKeysym( &event.xkey, 0 );

                    if( key < RW_KB_TOTAL_BUTTONS ) {
                        g_rw.kb.down[key] = RW_FALSE;
                    }

                    break;
                }

                case MotionNotify:
                {
                    g_rw.mouse.x = event.xmotion.x;
                    g_rw.mouse.y = event.xmotion.y;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        // TODO joypad
        g_rw.joy.x = 32786;
        g_rw.joy.y = 32786;

         // program loop
        if( g_fpLoop ) {
            g_fpLoop();
        }

        glXSwapBuffers( dpy, win );
    }

#endif

    if( g_fpClose ) {
        g_fpClose();
    }

    if( g_rw.sound.is_active )
    {
        ALCdevice *dev = alcGetContextsDevice( g_al_context );
        alcDestroyContext( g_al_context );
        alcCloseDevice( dev );
    }

#ifdef _WIN32

    if( g_rw.display.is_active )
    {
        wglMakeCurrent( NULL, NULL );
        wglDeleteContext( g_hrc );
        ReleaseDC( g_hwnd, g_hdc );
        DestroyWindow( g_hwnd );
    }

    return msg.wParam;

#else

    return 0;

#endif
}


raamwerk_t *rwStat()
{
    return &g_rw;
}

#ifdef _WIN32
LRESULT CALLBACK WinProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg )
    {
        case WM_CLOSE: {
            PostQuitMessage( 0 ); break;
        }

        case WM_DESTROY: {
            return 0;
        }

        case WM_KEYDOWN:
        {
            if( wparam < RW_KB_TOTAL_BUTTONS ) {
                g_rw.kb.down[wparam] = RW_TRUE;
            }

            break;
        }

        case WM_KEYUP:
        {
            if( wparam < RW_KB_TOTAL_BUTTONS ) {
                g_rw.kb.down[wparam] = RW_FALSE;
            }

            break;
        }

        case WM_LBUTTONDOWN: {
            g_rw.mouse.down[0] = RW_TRUE; break;
        }

        case WM_LBUTTONUP: {
            g_rw.mouse.down[0] = RW_FALSE; break;
        }

        case WM_MBUTTONDOWN: {
            g_rw.mouse.down[1] = RW_TRUE; break;
        }

        case WM_MBUTTONUP: {
            g_rw.mouse.down[1] = RW_FALSE; break;
        }

        case WM_MOUSEMOVE:
        {
            /* TODO screen orientation

            if( g_screenOri == -90 )
            {
                n1Mouse.x = HIWORD(lParam);
                n1Mouse.y = g_screenHeight - LOWORD(lParam);
            }
            else if( g_screenOri == 90 )
            {
                n1Mouse.x = g_screenWidth - HIWORD(lParam);
                n1Mouse.y = LOWORD(lParam);
            }
            else
            {
                n1Mouse.x = LOWORD(lParam);
                n1Mouse.y = HIWORD(lParam);
            }

            g_numTouchPoints = 1;
            */
            g_rw.mouse.x = LOWORD( lparam );
            g_rw.mouse.y = HIWORD( lparam );
            break;
        }

        case WM_RBUTTONDOWN: {
            g_rw.mouse.down[2] = RW_TRUE; break;
        }

        case WM_RBUTTONUP: {
            g_rw.mouse.down[2] = RW_FALSE; break;
        }

        case WM_SIZE:
        {
            g_rw.display.width = LOWORD( lparam );
            g_rw.display.height = HIWORD( lparam );

            if( g_fpResize ) {
                g_fpResize();
            }

            break;
		}

        default: {
            return DefWindowProc( hwnd, msg, wparam, lparam );
        }
    }

    return 0;
}
#endif
