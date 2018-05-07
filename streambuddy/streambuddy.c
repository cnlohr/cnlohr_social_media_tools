#include <stdio.h>
#include <tccengine.h>
#include <tccexports.h>
#include <CNFGFunctions.h>
#include <os_generic.h>


void (*handleKeyCB)( int keycode, int bDown );
void (*handleButtonCB)( int x, int y, int button, int bDown );
void (*handleMotionCB)( int x, int y, int mask );


void PopFn( TCCEngine * tce )
{
	handleKeyCB = 0;
	handleButtonCB = 0;
	handleMotionCB = 0;
	PopulateTCCE( tce );
}

//Callbacks from CNFG
void HandleKey( int keycode, int bDown )                  { if( handleKeyCB ) handleKeyCB( keycode, bDown ); }
void HandleButton( int x, int y, int button, int bDown )  { if( handleButtonCB ) handleButtonCB( x, y, button, bDown ); }
void HandleMotion( int x, int y, int mask )               { if( handleMotionCB ) handleMotionCB( x, y, mask ); }
void HandleDestroy(){}

int main()
{
	unsigned char tcceb[8192];

	CNFGPrepareForTransparency();
	CNFGSetupFullscreen( "test", 0 );

	TCCEngine * e = TCCECreate( "script.c", 0, 0, PopFn, tcceb );

	while(1)
	{
		TCCECheck( e, 0 );

		CNFGClearTransparencyLevel();
		CNFGClearFrame();

		if( e->update ) e->update( e->cid );
		CNFGSwapBuffers();

		if( !handleKeyCB ) handleKeyCB = TCCEGetSym( e, "handleKeyCB" );
		if( handleButtonCB ) handleButtonCB = TCCEGetSym( e, "handleButtonCB" );
		if( handleMotionCB ) handleMotionCB = TCCEGetSym( e, "handleMotionCB" );

		OGUSleep( 30000 );
	}
}

