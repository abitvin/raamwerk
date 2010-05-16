#include "myapp.h"


static float g_theta = 0.0f;
static ALuint g_snd_src, g_snd_buf;


void destroy()
{
    alSourceStop( g_snd_src );                // stop the source or else the buffer won't be freed
    alSourcei( g_snd_buf, AL_BUFFER, 0 );     // free bufferdata that still exists
    alDeleteBuffers( 1, &g_snd_buf );
    alDeleteSources( 1, &g_snd_src );
}


void init( int argc, char **argv )
{
    // init Raamwerk
    rwAudio();
	
#ifdef RW_IPHONE
	rwDisplay( "My beautiful app!", 320, 480, RW_FALSE, 24, 8, RW_NULL, RW_NULL, RW_NULL );
#else
	rwDisplay( "My beautiful app!", 640, 480, RW_FALSE, 24, 8, RW_NULL, RW_NULL, 4 );
	rwEnable( RW_VSYNC );
#endif

    // init OpenGL
    glClearColor( 0.1f, 0.1f, 0.1f, 0.0f );

    //glClearDepth( 1.0f );
    //glEnable( GL_DEPTH_TEST );
    //glDepthFunc( GL_LEQUAL );

    //glClearStencil( 0 );
    //glEnable( GL_STENCIL_TEST );

    //glShadeModel( GL_SMOOTH );
    //glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
    //glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    //glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    //glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    //glEnable( GL_TEXTURE_2D );
    //glEnable( GL_BLEND );
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


    // init OpenAL
    alGenSources( 1, &g_snd_src );
    alGenBuffers( 1, &g_snd_buf );

    // create sine wave
    float hertz = 1000.0f;
    float amplitude = 32000.0f;
    float length = 2.0f;            // length in seconds
    float sample_rate = 44100.0f;

    float freq = ( M_PI * 2 * hertz ) / sample_rate;
    int buf_length = sample_rate * length;
    short buf[buf_length];
    int time;

    for( time = 0; time < buf_length; time++ ) {
        buf[time] = amplitude * sin( time * freq );
    }

    alBufferData( g_snd_buf, AL_FORMAT_MONO16, buf, buf_length, sample_rate );
    alSourcei( g_snd_src, AL_BUFFER, g_snd_buf );
    //alSourcef( g_snd_src, AL_PITCH, 1.0f );
    //alSourcef( g_snd_src, AL_GAIN, 1.0f );
    //alSourcei( g_snd_src, AL_LOOPING, AL_TRUE );
    //alSourcei( g_snd_src, AL_SOURCE_RELATIVE, AL_TRUE );
    //alSourcei( g_snd_src, AL_ROLLOFF_FACTOR, 0 );
	
	alSourcePlay( g_snd_src );
	
	raamwerk_t *rw = rwStat();
	rw->hit.area[0].x = 0;
	rw->hit.area[0].y = 0;
	rw->hit.area[0].width = 320;
	rw->hit.area[0].height = 100;
	rw->hit.area[1].x = 0;
	rw->hit.area[1].y = 100;
	rw->hit.area[1].width = 320;
	rw->hit.area[1].height = 280;
	rw->hit.area[2].x = 0;
	rw->hit.area[2].y = 380;
	rw->hit.area[2].width = 320;
	rw->hit.area[2].height = 100;
	rw->hit.num_areas = 3;
}


void loop()
{
	raamwerk_t *rw = rwStat();
	 
    if( rw->kb.down['Z'] || rw->mouse.down[1] || rw->joy.down[1] || rw->hit.area[1].down ) {
        g_theta += 3.0f;
    }
	
	if( rw->kb.pressed['A'] || rw->mouse.pressed[0] || rw->joy.x > 64000 || rw->hit.area[0].pressed ) {
        g_theta += 15.0f;
    }

    if( rw->kb.released['S'] || rw->mouse.released[2] || rw->joy.x < 1000 || rw->hit.area[2].released ) {
        g_theta -= 15.0f;
    }

    if( rw->kb.pressed['P'] || rw->hit.area[2].pressed ) {
        alSourcePlay( g_snd_src );
    }
	
	// test for iPhone when application is interupted. for example, a phone call.
	if( rw->is_active == RW_FALSE ) {
		glClearColor( 1.0f, 0.1f, 0.1f, 0.0f );
	}

    glClear( GL_COLOR_BUFFER_BIT );

    glPushMatrix();
		glTranslatef( rw->mouse.x, rw->mouse.y, 0.0f );
		glScalef( 100.0f, 100.0f, 0.0f );
		glRotatef( g_theta, 0.0f, 0.0f, 1.0f );
	
#ifdef RW_IPHONE
	
		const GLfloat verts[] = {
			0.0f, 1.0f, 0.0f, 
			0.87f, -0.5f, 0.0f, 
			-0.87f, -0.5f, 0.0f,  
		};
	
		const GLfloat colors[] = {
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
		};
	
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );        
		glVertexPointer( 3, GL_FLOAT, 0, verts );
		glColorPointer( 4, GL_FLOAT, 0, colors );
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );

#else

		glBegin( GL_TRIANGLES );
            glColor3f( 1.0f, 0.0f, 0.0f ); glVertex2f( 0.0f, 1.0f );
            glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( 0.87f, -0.5f );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -0.87f, -0.5f );
        glEnd();

#endif

    glPopMatrix();
}


void resize()
{
	raamwerk_t *rw = rwStat();

    glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
#ifdef RW_IPHONE
	glOrthof( 0, rw->display.width, rw->display.height, 0, -99999, 99999 );
	//glOrthox( -0x10000, 0x10000, -0x18000, 0x18000, -0x10000, 0x10000 );
#else
	glOrtho( 0, rw->display.width, rw->display.height, 0, -99999, 99999 );
	//gluPerspective( 45.0f, rw->display.width / rw->display.height, 0.1f, 100.0f );
#endif
	glViewport( 0, 0, rw->display.width, rw->display.height );
	
	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

