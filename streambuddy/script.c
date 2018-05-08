#include "tccexports.h"
#include <stdarg.h>
#include "../cntools/os_generic.h"
#include <unistd.h>
#include <fcntl.h>


#define BRD_X 1920
#define BRD_Y 1056
#define WIN_X 1400
#define WIN_Y 956
#define CHAT_Y 500

#define DEFAULT_SIZE 4
#define BIG_SIZE 8

void DrawFatTextAt( int x, int y, int size, int width, int height, char * format, ... );
int spawn_process_with_pipes( const char * execparam, char * const argv[], int pipefd[3] );
int waitpid(pid_t pid, int *status, int options);
int kill(pid_t pid, int sig);

struct ScriptStructure
{
	int lastx, lasty;
	int compiles;
};

og_thread_t songthread;
struct ScriptStructure * id;
int doquit;
char NowPlaying[1024];

#include "fireworks.h"
#include "colorchord.h"

int init( struct ScriptStructure * cid )
{
	printf( "Init\n" );
}

void * RunNowPlaying( void * v )
{
	char tbuff[1024];
	char * argv[3] = { "bash", "./currently_playing.sh", 0 };
	int pipes[3];
	int k = 0;
	int ret;
	while(!doquit)
	{
		//sprintf( NowPlaying, "Hello, world %d\n", k++ );
		int procv = spawn_process_with_pipes( "bash", argv, pipes );
		OGUSleep(100000);
		if( procv < 1 ) goto closev;
		fcntl( pipes[1], F_SETFL, O_NONBLOCK );
		fcntl( pipes[2], F_SETFL, O_NONBLOCK );
		int r = read( pipes[1], tbuff, 1024 );
		if( r >= 0 ) tbuff[r] = 0;
		else tbuff[0] = 0;
//		if( procv > 0 ) kill( procv, -9 );
		if( procv > 0 )	waitpid(procv, &ret, 0);
		//snprintf( NowPlaying, sizeof(NowPlaying)-1, "%d %d %d - %s\n", r, k, ret, tbuff );
		memcpy( NowPlaying, tbuff, r );
		k++;
		closev:
		close( pipes[0] );
		close( pipes[1] );
		close( pipes[2] );
	}
}

int start( struct ScriptStructure * cid )
{
	printf( "Start\n" );
	cid->compiles++;
	id = cid;
	StartColorChord();
	songthread = OGCreateThread( RunNowPlaying, 0 );
}

int stop( struct ScriptStructure * cid )
{
	doquit = 1;
	StopColorChord();
	OGJoinThread( songthread );
	printf( "Stop\n" );
}

void DrawCursor()
{
	CNFGColor( 0x8c3000 );
	CNFGTackRectangle( id->lastx-5, id->lasty-5, id->lastx+5, id->lasty+5 );
}


void DrawTextOverlay()
{
	CNFGColor( 0xffffff );
	DrawFatTextAt( 5,  WIN_Y+8, BIG_SIZE, -1, -1, "Now -->\nPlaying\n" );
	DrawFatTextAt( BIG_SIZE*22, WIN_Y+5, DEFAULT_SIZE, -1, -1, NowPlaying );

	DrawFatTextAt( WIN_X + 4, CHAT_Y, DEFAULT_SIZE, BRD_X-WIN_X, BRD_Y - CHAT_Y-50, //BRD_X - WIN_X - 8,
"Lorem ipsum dolor sit amet, vis nostrud signiferumque ex. "
"Et sed sadipscing interpretaris, ut sea nobis commune maiestatis. "
"Est minim zril hendrerit in. Vim probo solum id. Per vero elit "
"adipiscing ex. Suscipit gloriatur inciderint vim at. Pri homero "
"tempor disputationi ut, ex sea dicant latine percipit." );
}

int update( struct ScriptStructure * cid )
{
	static int i;
	CNFGClearTransparencyLevel();
	CNFGClearFrame();
	CNFGHandleInput();

	CNFGDrawToTransparencyMode( 1 );
	CNFGTackRectangle( 0, WIN_Y, BRD_X, BRD_Y );
	CNFGTackRectangle( WIN_X, 0, BRD_X, BRD_Y );

	CNFGDrawToTransparencyMode( 0 );

	DrawColorChord();
	DrawFireworks();
	DrawTextOverlay();
	DrawCursor();

	CNFGSwapBuffers();
	OGUSleep( 30000 );
	//printf( "Update: %d %d\n", cid->compiles, 1 );
}


void handleKeyCB( struct ScriptStructure * cid, int keycode, int bDown )
{
	//Can't receive keypresses if an overlay.
}

void handleButtonCB( struct ScriptStructure * cid, int x, int y, int button, int bDown )
{
	if( bDown )
		MakeFirework( x, y );
//	printf( "Button: %d %d\n", button, bDown );
}

void handleMotionCB( struct ScriptStructure * cid, int x, int y, int mask )
{
	cid->lastx = x;
	cid->lasty = y;
}

