#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>
#include <unistd.h>


#ifndef LOADFILEDEFINED
#define LOADFILEDEFINED
static int LoadFileLineIntoBuffer( const char * folder_prefix, const char * file, char * buffer, int buffersize )
{
	char filename[1024];
	snprintf( filename, 1024, "%s/%s", folder_prefix, file );
	FILE * f = fopen( filename, "r" );
	if( !f )
	{
		fprintf( stderr, "Error: can't get client_id.txt\n" );
		return -1;
	}
	int c;
	int i = 0;
	while( ( c = fgetc( f ) ) != EOF && i < buffersize-1 )
	{
		if( c == '\n' ) break;
		buffer[i++] = c;
	}
	buffer[i] = 0;
	fclose( f );
	return i;
}
#endif

int main( int argc, char ** argv )
{
	char vididbuf[128];
	char curlurlbase[8192];
	char auxhead[8192];
	jsmntok_t tokens[131072];
	jsmn_parser jsmnp;

	const char * vidid = 0;
	if( argc == 1 )
	{
		int len = LoadFileLineIntoBuffer( "..", "stream_id.txt", vididbuf, sizeof( vididbuf ) );
		if( len < 5 )
		{
			fprintf( stderr, "Error: no parameters set and no valid stream_id.txt found.\n" );
			return -5;
		}
		vidid = vididbuf;
	}

	if( argc == 2 )
	{
		vidid = strdup(argv[1]);
	}

	if( !vidid )
	{
		fprintf( stderr, "Error! Usage: ./chatmon [stream (optional, if not present will re-use last stream ID)]\n" );
		return -5;
	}	

	const char * reqtype = "liveStreamingDetails";

	char apikey[8192];
	int uses_api_key = 0;

	{
		FILE * f = fopen( "../.ytapikey.txt", "r" );
		if(f)
		{
			fscanf( f, "%8100s", apikey );
			fclose( f );	
			uses_api_key = 1;
		}
	}
	sprintf( curlurlbase, "https://www.googleapis.com/youtube/v3/videos?id=%s&part=%s", vidid,reqtype);

	if( uses_api_key )
	{
		//fprintf( stderr, "Warning: Using API key instead of OAUTH\n" );
		sprintf( curlurlbase + strlen(curlurlbase), "&key=%s", apikey );
	}
//	fprintf( stderr, "Request: %s\n", curlurlbase);

	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = curlurlbase;

	if( !uses_api_key )
	{
		char oauthbear[8192];
		FILE * f = fopen( "../.oauthtoken.txt", "r" );
		if( !f )
		{
			fprintf( stderr, "Error: no oauth token found.  Run yt_oauth_helper\n" );
			return -9;
		}
		fscanf( f, "%s", oauthbear );
		fclose( f );	
		sprintf( auxhead, "Authorization: Bearer %s", oauthbear );
		req.AddedHeaders = auxhead;
	}
	else
	{
		req.AddedHeaders = 0;
	}

	//char AuxData[8192];    //For Websockets, this is the "Origin" URL.  Otherwise, it's Post data.
	req.AuxData = 0;
	req.AuxDataLength = 0;


	//Get a bunch of messages
	struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

	if( !r->payload )
	{
		fprintf( stderr, "No payload.\n" );
		return -9;
	}

	r->payload[r->payloadlen] = 0;
	//Uncomment this if you aren't getting anything.
	printf( "%s\n", r->payload );

	char * foundactiveChatId = strstr( r->payload, "activeLiveChatId" );
	char * endfoundactiveChatId;
	if( foundactiveChatId ) foundactiveChatId = strchr( foundactiveChatId+18, '\"' );
	if( foundactiveChatId ) endfoundactiveChatId = strchr( ++foundactiveChatId, '\"' );
	if( !foundactiveChatId || !endfoundactiveChatId )
	{
		fprintf( stderr, "Error: can't find activeLiveChatId.\n" );
	}
	else
	{
		*endfoundactiveChatId = 0;
		FILE * f = fopen( "../live_chat_id.txt", "wb" );
		fprintf( f, "%s\n", foundactiveChatId );
		fclose( f );
	}

	if( argc == 2 )
	{
		FILE * f = fopen( "../stream_id.txt", "wb" );
		fprintf( f, "%s\n", vidid );
		fclose(f );
	}

//    "actualStartTime": "2018-05-08T05:41:05.000Z",
//    "concurrentViewers": "1",
//    "activeLiveChatId": "EiEKGFVDUXM4Z19Ddy1IMFN0ZUxtNUVuSkJfdxIFL2xpdmU"

}

