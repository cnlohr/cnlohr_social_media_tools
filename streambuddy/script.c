#include "tccexports.h"
#include <stdarg.h>
#include "../cntools/os_generic.h"
#include <unistd.h>
#include <fcntl.h>

//#define FULL_1080P


#ifdef FULL_1080P

#define BRD_X 1920
#define BRD_Y 1056
#define WIN_X 1400
#define WIN_Y 956
#define CHAT_Y 500
#define DEFAULT_SIZE 4
#define BIG_SIZE 8
#define HUGE_SIZE 15


#else

#define BRD_X 1280
#define BRD_Y 710
#define WIN_X 960
#define WIN_Y 630
#define CHAT_Y 300
#define DEFAULT_SIZE 3
#define BIG_SIZE 4
#define HUGE_SIZE 8

#endif


void DrawFatTextAt( int x, int y, int size, int width, int height, char * format, ... );
int spawn_process_with_pipes( const char * execparam, char * const argv[], int pipefd[3] );
int waitpid(pid_t pid, int *status, int options);
int kill(pid_t pid, int sig);
extern const char * loremipsum;

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
		int r = read( pipes[2], tbuff, 1024 );
		if( r > 0 ) 
		{
			tbuff[r] = 0;
			printf( "Error: %s\n", tbuff );
		}
		r = read( pipes[1], tbuff, 1024 );
		if( r >= 0 ) tbuff[r] = 0;
		else tbuff[0] = 0;

//		if( procv > 0 ) kill( procv, -9 );
		if( procv > 0 )	waitpid(procv, &ret, 0);
		//snprintf( NowPlaying, sizeof(NowPlaying)-1, "%d %d %d - %s\n", r, k, ret, tbuff );
		memcpy( NowPlaying, tbuff, r+1 );
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
	DrawFatTextAt( BIG_SIZE*23, WIN_Y+5, DEFAULT_SIZE, -1, -1, "%s", NowPlaying );

	DrawFatTextAt( WIN_X + 4, CHAT_Y, DEFAULT_SIZE, BRD_X-WIN_X, BRD_Y - CHAT_Y-40, loremipsum );
	DrawFatTextAt( WIN_X - 180, WIN_Y+5, HUGE_SIZE, -1, -1, "8:88:88" );
	DrawFatTextAt( WIN_X - 180, WIN_Y+5+45, BIG_SIZE, -1, -1, "88 WATCHING" );
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




const char * loremipsum = "Lorem ipsum dolor sit amet, ad sea modo vidisse interesset, vis elit mentitum at. Sea utroque accusam no, debet audire mei ad. Agam eius constituto duo ad, vix euismod accusata inciderint ad, iusto libris audire at nam. Facilis invidunt argumentum te eum, pro an illud reque.\nEt sit bonorum ceteros, ut enim vitae nonumy sed, mei ei sonet eloquentiam. Ius errem consequuntur id, munere feugait accusam vix ea. Dolore bonorum vel eu. Per magna decore dolorem te.\nMea cu illum oratio nullam, et causae mentitum mei, per at facer fierent adolescens. No pro pericula complectitur, usu no integre aliquid scripserit, nam eu purto euismod maluisset. Tamquam oportere ex pro, graeci dolores verterem vim eu, nostro disputando at sed. Modo dolorum nusquam an eos. Ad dicam iracundia mediocritatem vim, ponderum disputando in nec. No brute fierent eam, eam ei dolor singulis voluptatum. Ad mundi ornatus delectus eos, melius lucilius senserit qui eu, verear verterem id mei.\nMei in latine luptatum, sit mollis timeam accusata ea. Tota sensibus eam ne, has discere eleifend expetenda eu. Vocent suscipit ullamcorper te eum. Cum ad ceteros suscipiantur, error sapientem sit an, graece menandri vix te. Id his probo voluptua dissentiunt, usu possit sententiae voluptatibus ex. Debitis detracto iudicabit qui te, eu cum odio vivendum urbanitas, ne quo adipiscing appellantur. Aliquid perfecto percipitur nec at, dicunt prompta et vis.\nSale adhuc et eam, te minim nobis mentitum has. Nibh essent an duo, dolores principes in eam. No scripta dignissim hendrerit nam, perfecto conceptam ad mei, vis ex prodesset definiebas honestatis. Pro novum neglegentur te.";
