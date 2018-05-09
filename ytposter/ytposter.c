#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>
#include <unistd.h>


int main( int argc, char ** argv )
{
	char curlurlbase[8192];
	char auxhead[8192];
	jsmntok_t tokens[131072];
	jsmn_parser jsmnp;

	if( argc < 2 )
	{
		fprintf( stderr, "Error! Usage: ./chatmon [livechatid] [message]\n" );
		return -5;
	}	

	const char * livechatid = strdup(argv[1]);

	const char * reqtype = "snippet";

	sprintf( curlurlbase, "https://www.googleapis.com/youtube/v3/liveChat/messages?part=%s",reqtype);

	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = curlurlbase;

	char oauthbear[8192];
	FILE * f = fopen( "../.oauthtoken.txt", "r" );
	if( !f )
	{
		fprintf( stderr, "Error: no oauth token found.  Run yt_oauth_helper\n" );
		return -9;
	}
	fscanf( f, "%s", oauthbear );
	fclose( f );	

	sprintf( auxhead, "Authorization: Bearer %s\r\nContent-Type: application/json; charset=UTF-8", oauthbear );

	req.AddedHeaders = auxhead;
	char AuxData[8192];    //For Websockets, this is the "Origin" URL.  Otherwise, it's Post data.

	char * message;
	if( argc < 3 )
	{
		int mml = 16384;
		message = malloc( mml+4 );
		int messagelen = 0;
		char line[8192];
		line[0] = 0;
		while ((fgets(line, sizeof line, stdin) != NULL) && ( line[0]  && line[0] != '\n' ) )
		{
			int linelen = strlen( line );
			printf( "GOT line len: %d\n", linelen );
			int i;
			for( i = 0; i < linelen; i++ )
			{
				char c = line[i];
				if( c == 9 ) c = ' ';
				else if( c < 32 ) continue;
				else if( c == '\"' || c == '\\' )
				{
					message[messagelen++] = '\\';
					message[messagelen++] = c;
				}
				else
					message[messagelen++] = c;

				if( messagelen > mml ) break;				
			}
			if( messagelen > mml ) break;
			message[messagelen++] = ' ';
			message[messagelen] = 0;
			line[0] = 0;
			printf( "Emitting message length: %d\n", messagelen );
			puts( message );
		}
	}
	else
	{
		message = argv[2];
	}

	snprintf( AuxData, sizeof( AuxData ) - 1, "{\
  \"snippet\": { \
    \"liveChatId\": \"%s\", \
    \"type\": \"textMessageEvent\", \
    \"textMessageDetails\": { \
      \"messageText\": \"%s\" \
    } \
  } \
}", livechatid, message );


//	memset( argv[1], '-', strlen( argv[1] ) );
//	memset( argv[2], '-', strlen( argv[2] ) );

	req.AuxData = AuxData;
	req.AuxDataLength = strlen( AuxData );

	//Get a bunch of messages
	struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

	if( r->payload )
	{
		r->payload[r->payloadlen] = 0;
		//Uncomment this if you aren't getting anything.
		printf( "%s\n", r->payload );
	}
	else
	{
		fprintf( stderr, "No payload.\n" );
	}

}

