#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>
#include <unistd.h>
#include <string.h>
#include <http_bsd.h>
#include <cnhttp.h>

int port = 8089;
	char client_secret[8192];
	char client_id[8192];
char oauthresp[1024];
volatile int oauthrespgot;

static void oauth2cb()
{
	URLDecode( oauthresp, 512, curhttp->pathbuffer+18 );

	if( strncmp( oauthresp, "code=", 5 ) != 0 )
	{
		fprintf( stderr, "Error received on key.\n%s\n", oauthresp );
		DataStartPacket();
		PushString( "Auth failed.\r\n" );
		PushString( oauthresp );
		EndTCPWrite( curhttp->socket );

		curhttp->state = HTTP_WAIT_CLOSE;
		return;
	}
	oauthrespgot = 1;


	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = "https://accounts.google.com/o/oauth2/token";
	char auxhead[8192];
	req.AddedHeaders = auxhead;
	sprintf( auxhead, "Content-Type: application/x-www-form-urlencoded" );

	char content[8192];
	snprintf( content, 8191, "%s&client_id=%s&client_secret=%s&redirect_uri=http%%3A%%2F%%2Flocalhost:%d%%2Fd%%2foauth2callback&grant_type=authorization_code", oauthresp, client_id, client_secret, port );

	req.AuxData = content;
	req.AuxDataLength = strlen( content );

	//Get a bunch of messages
	struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

	if( !r->payload )
	{
		fprintf( stderr, "Error: empty response received.\n" );
		exit( -5 );
	}

	char * foundkey = strstr( r->payload, "access_token" );
	char * endkey;
	if( foundkey ) foundkey = strchr( foundkey+14, '\"' );
	if( foundkey ) endkey = strchr( ++foundkey, '\"' );
	if( !foundkey || !endkey )
	{
		fprintf( stderr, "Error: can't find token.\n" );
		exit( -18 );
	}

	*endkey = 0;

	FILE * f = fopen( "../.oauthtoken.txt", "w" );
	fprintf( f, "%s\n", foundkey );
	
	DataStartPacket();
	PushString( "<HTML><BODY ONLOAD=\"window.open('','_parent','');window.close();\">Auth ok.  You can close this page.</BODY></HTML>" );
	EndTCPWrite( curhttp->socket );
	DataStartPacket();

	printf( "Keys written to ../.oauthtoken.txt\n" );

	curhttp->state = HTTP_WAIT_CLOSE;

	HTTPClose();

	TermHTTPServer();
}

void HTTPCustomCallback( )
{
	if( curhttp->rcb )
		((void(*)())curhttp->rcb)();
	else
		curhttp->isdone = 1;
}


//Close of curhttp happened.
void CloseEvent()
{
}


void NewWebSocket()
{
}



void WebSocketTick()
{
}

void WebSocketData( int len )
{
}


void HTTPCustomStart( )
{
	if( strncmp( (const char*)curhttp->pathbuffer, "/d/oauth2callback", 17 ) == 0 )
	{
		curhttp->rcb = (void(*)())&oauth2cb;
		curhttp->bytesleft = 0xffffffff;
	}
	else
	{
		curhttp->rcb = 0;
		curhttp->bytesleft = 0;
	}
	curhttp->isfirst = 1;
	HTTPHandleInternalCallback();
}

int main( int argc, char ** argv )
{
	RunHTTP( port );

	FILE * f = fopen( "../.client_id.txt", "r" );
	if( !f )
	{
		fprintf( stderr, "Error: can't get client_id.txt\n" );
		goto missing_files;
	}
	if( fscanf( f, "%s\n", client_id ) != 1 )
	{
		fprintf( stderr, "Error: can't understand client_id.txt\n" );
		goto missing_files;
	}


	f = fopen( "../.client_secret.txt", "r" );
	if( !f )
	{
		fprintf( stderr, "Error: can't get client_secret.txt\n" );
		goto missing_files;
	}

	if( fscanf( f, "%s\n", client_secret ) != 1 )
	{
		fprintf( stderr, "Error: can't understand client_secret.txt\n" );
		goto missing_files;
	}
	
	printf( "Please visit:\n\nhttps://accounts.google.com/o/oauth2/auth?client_id=%s&scope=https://www.googleapis.com/auth/youtube&response_type=code&access_type=offline&redirect_uri=http%%3A%%2F%%2Flocalhost:%d%%2Fd%%2foauth2callback\n", client_id, port );

	while(!oauthrespgot)
	{
		TickHTTP();
		usleep( 30000 );
		printf( "%d\n", oauthrespgot );
	}

	exit( 0 );
	missing_files:
	printf( "Missing files ../.client_secret.txt and/or ../.client_id.txt.\n\
		Log into your API Dashboard.\n\
		https://console.developers.google.com/\n\
		Select your project and go to the youtube section.\n\
		Create new oauth credentials.\n\
\n\
		Select type for new oauth app to be \"other\"\n\
\n\
		Get the CLIENT_ID and note the SECRET_KEY.\n\
\n\
		Write them into ../client_id.txt and../client_secret.txt.\n" );
	return -9;
}

