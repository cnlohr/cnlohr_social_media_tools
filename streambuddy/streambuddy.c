#include <stdio.h>
#include <tccengine.h>
#include <tccexports.h>
#include <CNFGFunctions.h>
#include <os_generic.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>   

unsigned char * gtcceb;

void (*handleKeyCB)( void * v, int keycode, int bDown );
void (*handleButtonCB)( void * v, int x, int y, int button, int bDown );
void (*handleMotionCB)( void * v, int x, int y, int mask );

int spawn_process_with_pipes( const char * execparam, char * const argv[], int pipefd[3] );

void DrawFatTextAt( int x, int y, int size, int width, int height, char * format, ... );


void PopFn( TCCEngine * tce )
{
	handleKeyCB = 0;
	handleButtonCB = 0;
	handleMotionCB = 0;
	PopulateTCCE( tce );

	TCCESetSym( tce, "CNFGPenX", &CNFGPenX );
	TCCESetSym( tce, "CNFGPenY", &CNFGPenY );
	TCCESetSym( tce, "CNFGBGColor", &CNFGBGColor );
	TCCESetSym( tce, "CNFGLastColor", &CNFGLastColor );
	TCCESetSym( tce, "CNFGDialogColor", &CNFGDialogColor );
	
	TCCESetSym( tce, "CNFGDrawText", CNFGDrawText );
	TCCESetSym( tce, "CNFGDrawBox", CNFGDrawBox );
	TCCESetSym( tce, "CNFGGetTextExtents", CNFGGetTextExtents );
	TCCESetSym( tce, "CNFGDrawTextbox", CNFGDrawTextbox );
	TCCESetSym( tce, "CNFGColor", CNFGColor );
	TCCESetSym( tce, "CNFGUpdateScreenWithBitmap", CNFGUpdateScreenWithBitmap );
	TCCESetSym( tce, "CNFGTackPixel", CNFGTackPixel );
	TCCESetSym( tce, "CNFGTackSegment", CNFGTackSegment );

	TCCESetSym( tce, "CNFGTackRectangle", CNFGTackRectangle );
	TCCESetSym( tce, "CNFGTackPoly", CNFGTackPoly );
	TCCESetSym( tce, "CNFGSwapBuffers", CNFGSwapBuffers );
	TCCESetSym( tce, "CNFGClearFrame", CNFGClearFrame );
	TCCESetSym( tce, "CNFGGetDimensions", CNFGGetDimensions );
	TCCESetSym( tce, "CNFGSetup", CNFGSetup );
	TCCESetSym( tce, "CNFGSetupFullscreen", CNFGSetupFullscreen );
	TCCESetSym( tce, "CNFGHandleInput", CNFGHandleInput );

	TCCESetSym( tce, "CNFGPrepareForTransparency", CNFGPrepareForTransparency );
	TCCESetSym( tce, "CNFGDrawToTransparencyMode", CNFGDrawToTransparencyMode );
	TCCESetSym( tce, "CNFGClearTransparencyLevel", CNFGClearTransparencyLevel );
	TCCESetSym( tce, "CNFGSetLineWidth", CNFGSetLineWidth );

	TCCESetSym( tce, "DrawFatTextAt", DrawFatTextAt );
	TCCESetSym( tce, "spawn_process_with_pipes", spawn_process_with_pipes );


	TCCESetSym( tce, "mmap", mmap );
	TCCESetSym( tce, "munmap", munmap );
	TCCESetSym( tce, "shm_unlink", shm_unlink );
	TCCESetSym( tce, "shm_open", shm_open );

}

//Callbacks from CNFG
void HandleKey( int keycode, int bDown )                  { if( handleKeyCB ) handleKeyCB( gtcceb, keycode, bDown ); }
void HandleButton( int x, int y, int button, int bDown )  { if( handleButtonCB ) handleButtonCB( gtcceb, x, y, button, bDown ); }
void HandleMotion( int x, int y, int mask )               { if( handleMotionCB ) handleMotionCB( gtcceb, x, y, mask ); }
void HandleDestroy(){}

int main()
{
	unsigned char tcceb[8192];
	gtcceb = &tcceb[0];

	CNFGPrepareForTransparency();
	CNFGSetupFullscreen( "test", 0 );
	CNFGClearTransparencyLevel();
	const char * additionalfiles[] = { "fireworks.h", "colorchord.h" };

	TCCEngine * e = TCCECreate( "script.c", additionalfiles, 2, PopFn, tcceb );

	while(1)
	{
		TCCECheck( e, 0 );

		if( e->update ) e->update( e->cid );

		if( !handleKeyCB ) handleKeyCB = TCCEGetSym( e, "handleKeyCB" );
		if( !handleButtonCB ) handleButtonCB = TCCEGetSym( e, "handleButtonCB" );
		if( !handleMotionCB ) { handleMotionCB = TCCEGetSym( e, "handleMotionCB" ); printf( "-- %p\n", handleMotionCB ); }
	}
}



//Commands
int spawn_process_with_pipes( const char * execparam, char * const argv[], int pipefd[3] )
{
	int pipeset[6];
  
	pipe(&pipeset[0]);
	pipe(&pipeset[2]);
	pipe(&pipeset[4]);
	
	int rforkv = fork();
	if (rforkv == 0)
	{
		// child
		close( pipeset[1] );
		close( pipeset[2] );
		close( pipeset[4] );
		dup2(pipeset[0], 0);
		dup2(pipeset[3], 1);
		dup2(pipeset[5], 2);
		close( pipeset[0] );
		close( pipeset[3] );
		close( pipeset[5] );
		execvp( execparam, argv );
	}
	else
	{
		// parent
		pipefd[0] = pipeset[1];
		pipefd[1] = pipeset[2];
		pipefd[2] = pipeset[4];
		close( pipeset[0] );
		close( pipeset[3] );
		close( pipeset[5] );
		return rforkv;
	}
}

void DrawFatTextAt( int x, int y, int size, int width, int height, char * format, ... )
{
	char buffer[16384];
    va_list args1;
    va_start(args1, format);
	vsnprintf( buffer, sizeof( buffer ), format, args1 );

	int len = strlen( buffer );

	if( width > 0 )
	{
		int i;
		int wid = 0;
		int lastbreakchar = -1;
		for( i = 0; i < len; i++ )
		{
			if( buffer[i] == '\n' )
			{
				wid = 0;
				lastbreakchar = -1;
			}
			else if( ! ( ( buffer[i] >= '0' && buffer[i] <= '9' ) ||
				 (buffer[i] >= 'A' && buffer[i] <= 'Z' ) ||
				(buffer[i] >= 'a' && buffer[i] <= 'z' ) ) )
			{
				lastbreakchar = i+1;
			}

			wid += size*3;
			if( wid >= width-size*6 && buffer[i+1] != ' ' )
			{
				//Insert breark at lastbreakchar.
				int push = 1;
				if( lastbreakchar < 0 ) 
				{
					lastbreakchar = i-1;
					push = 2;
				}
				int j;
				for( j = len; j >= lastbreakchar; j-- )
				{
					buffer[j+push] = buffer[j];
				}
				len+=push;
				if( push == 2 ) 
				{
					buffer[lastbreakchar+1] = '\n';
					buffer[lastbreakchar] = '-';
				}
				else
				{
					buffer[lastbreakchar] = '\n';
				}
				wid = 0;
				i = lastbreakchar+1;
				lastbreakchar = -1;
			}
			if( len > sizeof(buffer) - 2)  break;
		}
		buffer[sizeof(buffer)-1] = 0;
	}

	if( height > 0 )
	{
		int i;
		int cheight = 0;
		for( i = len-1; i >= 0; i-- )
		{
			if( buffer[i] == '\n' )	cheight += size*6;
			if( cheight >= height ) break;
		}
		if( i > 0 )
		{
			int j, k;
			for( j = i+1, k = 0; j < len; j++, k++ )
			{
				buffer[k] = buffer[j];
			}
			buffer[k] = 0;
			len -= i;
		}
	}

	int thickx = (size+1) / 4;
	int thicky = (size) / 4;
	int ix, iy;
	for( ix = 0; ix <= thickx; ix++ )
	for( iy = 0; iy <= thicky; iy++ )
	{
		CNFGPenX = x+ix;
		CNFGPenY = y+iy;
		CNFGDrawText( buffer, size );
	}
}


