#include "tccexports.h"
#include <stdarg.h>
#include "../cntools/os_generic.h"
#include <unistd.h>
#include <fcntl.h>

#define FULL_1080P
//#define RES_1366x768

#define STREAMID   "sMQcz8fDTpg"
#define LIVECHATID "Cg0KC3NNUWN6OGZEVHBnKicKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxILc01RY3o4ZkRUcGc"

#ifdef FULL_1080P

#define BRD_X 1920
#define BRD_Y 1056
#define WIN_X 1400
#define WIN_Y 956
#define CHAT_Y 500
#define DEFAULT_SIZE 4
#define BIG_SIZE 8
#define HUGE_SIZE 10
#define STATS_X 300

#elif defined( RES_1366x768 )

#define BRD_X 1366
#define BRD_Y 742
#define WIN_X 1024
#define WIN_Y 666
#define CHAT_Y 300
#define DEFAULT_SIZE 3
#define BIG_SIZE 4
#define HUGE_SIZE 8
#define STATS_X 180

#else

#define BRD_X 1280
#define BRD_Y 720
#define WIN_X 960
#define WIN_Y 630
#define CHAT_Y 300
#define DEFAULT_SIZE 3
#define BIG_SIZE 4
#define HUGE_SIZE 8
#define STATS_X 180

#endif


void DrawFatTextAt( int x, int y, int size, int width, int height, char * format, ... );
int spawn_process_with_pipes( const char * execparam, char * const argv[], int pipefd[3] );
int waitpid(pid_t pid, int *status, int options);
int kill(pid_t pid, int sig);
char * strchr( const char *, char );

//Opengl stuff
void glColor4f( float r, float g, float b, float a );
void glEnable (int cap);
void glBlendFunc (int sfactor, int dfactor);
void glClear( int mask );
#define GL_BLEND				0x0BE2
#define GL_ONE_MINUS_DST_ALPHA			0x0305
#define GL_DST_ALPHA				0x0304
#define GL_SRC_ALPHA				0x0302
#define GL_ONE_MINUS_SRC_ALPHA			0x0303
#define GL_DEPTH_BUFFER_BIT   0x00000100 // == 0b000000100000000
#define GL_ACCUM_BUFFER_BIT   0x00000200 // == 0b000001000000000
#define GL_STENCIL_BUFFER_BIT 0x00000400 // == 0b000010000000000
#define GL_COLOR_BUFFER_BIT   0x00004000 // == 0b100000000000000

//At bottom of code, send message to chat.
void SendChatMessage( const char * message );

struct ScriptStructure
{
	int lastx, lasty;
	int compiles;
};

int chat_process;
int chatpipes[3];

og_thread_t songthread;
og_thread_t chatthread;
og_thread_t statusthread;

struct ScriptStructure * id;
int doquit;

#include "streamstatus.h"
#include "nowplaying.h"
#include "colorchord.h"
#include "fireworks.h"
#include "livechatmon.h"

int init( struct ScriptStructure * cid )
{
	CNFGClearFrame();
	printf( "Init\n" );
}

int start( struct ScriptStructure * cid )
{
	printf( "Start\n" );
	cid->compiles++;
	id = cid;
	StartColorChord();
	songthread = OGCreateThread( RunNowPlaying, 0 );
	chatthread = OGCreateThread( RunChatMon, 0 );
	statusthread = OGCreateThread( RunStreamStatus, 0 );
}

int stop( struct ScriptStructure * cid )
{
	doquit = 1;
	StopColorChord();
	OGJoinThread( songthread );
	OGJoinThread( chatthread );
	OGJoinThread( statusthread );

	printf( "Stop\n" );

	if( chat_process )
	{
		int ret;
		waitpid(chat_process, &ret, 0);
	}
}

void DrawCursor()
{
	CNFGColor( 0x8c3000 );
	CNFGTackRectangle( id->lastx-5, id->lasty-5, id->lastx+5, id->lasty+5 );
}


void DrawTextOverlay()
{
	CNFGColor( 0xffffff );
	DrawFatTextAt( BIG_SIZE*23, WIN_Y+5, DEFAULT_SIZE, -1, -1, "%s", NowPlaying );

	DrawFatTextAt( WIN_X + 4, CHAT_Y, DEFAULT_SIZE, BRD_X-WIN_X-30, BRD_Y - CHAT_Y, "%s", ChatWindowText );

//	DrawFatTextAt( 780, WIN_Y+5, 4, -1, -1, "(1) Recap\n(2) Figure out how to open lighthouse\n(3) Use FX3 to record data" );

	DrawFatTextAt( WIN_X - STATS_X, WIN_Y+5+45, BIG_SIZE, -1, -1, 
(CurrentViewers>0)?"%d WATCHING":"STREAM STATUS ERROR", CurrentViewers );

	//printf( "%f\n", OGGetAbsoluteTime() );
	
}

int update( struct ScriptStructure * cid )
{
	short sw, sh;
	CNFGGetDimensions( &sw, &sh );
	static int i;
	//CNFGClearTransparencyLevel();
	//CNFGClearFrame();
	CNFGHandleInput();

#define RIGHTTOP 480

	CNFGDrawToTransparencyMode( 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	CNFGColor( 0x000000 );
	CNFGTackRectangle( 0, 0, sw, sh );
	CNFGColor( 0xffffff );
	CNFGTackRectangle( 0, RIGHTTOP, BRD_X, BRD_Y );
	CNFGColor( 0x0000000 );
	CNFGTackRectangle( 0, 0, WIN_X, WIN_Y );
	CNFGDrawToTransparencyMode( 0 );

	CNFGColor( 0x0202020 );
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f( .1, .1, .1,  1.0 );
	CNFGTackRectangle( 0, WIN_Y, WIN_X, BRD_Y );
	glColor4f( .1, .1, .1,  .3 );
	CNFGTackRectangle( WIN_X, 0, BRD_X, BRD_Y );

	glColor4f( .1, .1, .1,  1.0 );

	DrawColorChord();
	DrawTextOverlay();
	DrawCursor();
	DrawFireworks();

	CNFGSwapBuffers();
	OGUSleep( 16000 );
	//printf( "Update: %d %d\n", cid->compiles, 1 );
}


void handleKeyCB( struct ScriptStructure * cid, int keycode, int bDown )
{
	//Can't receive keypresses if an overlay.
}

void handleButtonCB( struct ScriptStructure * cid, int x, int y, int button, int bDown )
{
	if( bDown )
		MakeFirework( x, y, 1.0 );
//	printf( "Button: %d %d\n", button, bDown );
}

void handleMotionCB( struct ScriptStructure * cid, int x, int y, int mask )
{
	cid->lastx = x;
	cid->lasty = y;
}



void SendChatMessage( const char * message )
{
	int chat_pipes[3];
	//Close existing process if running.
	if( chat_process )
	{
		int ret;
		waitpid(chat_process, &ret, 0);
	}
	char * argv[3] = { "../ytposter/ytposter", LIVECHATID, 0 };
	chat_process = spawn_process_with_pipes( "../ytposter/ytposter", argv, chat_pipes );
	if( chat_process > 0 )
	{
		write( chat_pipes[0], message, strlen( message ) );
		OGUSleep(10000);
	}
	close( chat_pipes[0] );
	close( chat_pipes[1] );
	close( chat_pipes[2] );
}





