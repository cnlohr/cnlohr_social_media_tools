#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>
#include <unistd.h>


char * NextPageToken = 0;
	int pollinfo = 0;


const char * GetTokenByName( char * origtext, jsmntok_t * tok )
{
	origtext[tok->end] = 0;
	return origtext + tok->start;
}

void ProcessNextElement( jsmntok_t ** tok )
{
	int nr_children = (*tok)->size;
	int j;
	//printf( "Process Next: %d\n", nr_children );
	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		ProcessNextElement( tok );
	}
}

const char * ReadInsideChatSnippet( char * origtext, jsmntok_t ** tok )
{
	const char * msgo = 0;
	int j;
	int nr_children = (*tok)->size;

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );

		if( strcmp( st, "displayMessage" ) == 0 )
		{
			msgo = GetTokenByName( origtext, (*tok) + 1 );
			ProcessNextElement( tok );
		}
		else
		{
			ProcessNextElement( tok );
		}
	}
	return msgo;
}


const char * ReadInsideChatAuthorDetails( char * origtext, jsmntok_t ** tok )
{
	const char * auth = 0;
	int j;
	int nr_children = (*tok)->size;

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );

		if( strcmp( st, "displayName" ) == 0 )
		{
			auth = GetTokenByName( origtext, (*tok) + 1 );
			ProcessNextElement( tok );
		}

		ProcessNextElement( tok );
	}
	return auth;
}


void ReadChatEntry( char * origtext, jsmntok_t ** tok, const char ** chatsnip, const char ** authorsnip )
{
	//Found items... Advnce to next token.
	int j;
	int nr_children = (*tok)->size;

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );

		if( strcmp( st, "snippet" ) == 0 )
		{
			(*tok)++;
			*chatsnip = ReadInsideChatSnippet( origtext, tok );
		}
		else if( strcmp( st, "authorDetails" ) == 0 )
		{
			(*tok)++;
			*authorsnip = ReadInsideChatAuthorDetails( origtext, tok );
		}
		else
		{
			ProcessNextElement( tok );
		}
	}
}


void ProcessChatMessageResponse(char * origtext, jsmntok_t ** tok, jsmntok_t * tokend )
{

	int nr_children = (*tok)->size;
	int j;
	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );

		if( strcmp( st, "pollingIntervalMillis" ) == 0 )
		{			
			const char * st = GetTokenByName( origtext, *tok + 1 );
			pollinfo = atoi( st );
		}
		else if( strcmp( st, "nextPageToken" ) == 0 )
		{
			if( NextPageToken ) free( NextPageToken );
			NextPageToken = strdup( GetTokenByName( origtext, *tok + 1 ) );
		}
		else if( strcmp( st, "items" ) == 0 )
		{
			(*tok)++;
			int nr_children = (*tok)->size;
			int i;
			for( i = 0; i < nr_children; i++ )
			{
				(*tok)++;
				const char * chatsnip, *authorsnip;
				chatsnip = authorsnip = 0;
				ReadChatEntry( origtext, tok, &chatsnip, &authorsnip );
				printf( "%s: %s\n", authorsnip, chatsnip );
				fflush( stdout );
			}
		}
		ProcessNextElement( tok );
	}
}



int main()
{
	char curlurl[8192];
	jsmntok_t tokens[131072];
	jsmn_parser jsmnp;

	const char * livechatid = "EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU";
	const char * apikey = "AIzaSyA1XpoUMNDFOx0W4-HjiUI1uiahdfe20lE";
	const char * reqtype = "authorDetails,snippet";

	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = curlurl;
	req.AddedHeaders = "";
	req.AuxData = 0;
	req.AuxDataLength = 0;

	while( 1 )
	{
		sprintf( curlurl, "https://www.googleapis.com/youtube/v3/liveChat/messages?liveChatId=%s&part=%s&key=%s%s%s",
			livechatid, 
			reqtype,
			apikey,
			NextPageToken?"&pageToken=":"",
			NextPageToken?NextPageToken:"" );

		//printf("%s\n", curlurl );

		//Get a bunch of messages
		struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

		r->payload[r->payloadlen-1] = 0;

		//printf( "PAYLOAD: %s\n", r->payload );
		jsmn_init( &jsmnp );
		int tottoks = jsmn_parse(&jsmnp, r->payload, r->payloadlen,
			tokens, sizeof(tokens)/sizeof(tokens[0]) );
		if( tottoks > 0 )
		{
			jsmntok_t * tok = &tokens[0];
			ProcessChatMessageResponse( r->payload, &tok, &tokens[tottoks] );
			CNHTTPClientCleanup( r );
		}
		else
		{
			fprintf( stderr, "Error %d parsing the JSON : %s\n", tottoks, r->payload );
			CNHTTPClientCleanup( r );
		}

		if( pollinfo )
			usleep( pollinfo * 1000 );
		else
			sleep(2);
	}

}

