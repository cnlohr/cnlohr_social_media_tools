#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>
#include <unistd.h>


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


char * ProcessChatMessageResponse(char * origtext, jsmntok_t ** tok, jsmntok_t * tokend, int * pollinfo, char ** NextPageToken, int show_history )
{
	int retlen = 0;
	int retmalloc = 1024;
	char * ret = malloc( retmalloc );

	int nr_children = (*tok)->size;
	int j;

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );

		if( strcmp( st, "pollingIntervalMillis" ) == 0 )
		{			
			const char * st = GetTokenByName( origtext, *tok + 1 );
			if( pollinfo ) *pollinfo = atoi( st );
		}
		else if( strcmp( st, "nextPageToken" ) == 0 )
		{
			if( NextPageToken )
			{
				if( *NextPageToken ) free( *NextPageToken );
				*NextPageToken = strdup( GetTokenByName( origtext, *tok + 1 ) );
			}
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

				if( show_history )
				{
					if( (retlen + 1024) > retmalloc )
					{
						retmalloc += 1024;
						ret = realloc( ret, retmalloc );
					}

					retlen += sprintf( ret + retlen, "%s\t%s\n", authorsnip, chatsnip );
				}
			}
		}
		ProcessNextElement( tok );
	}
	ret[retlen] = 0;
	return ret;
}


char * GetLivechatData( const char * livechatid, char ** nextpagetoken, int include_history, int * pollinfo )
{
	char curlurlbase[8192];
	char curlurl[8192];
	const char * reqtype = "authorDetails,snippet";

	
	sprintf( curlurlbase, "https://www.googleapis.com/youtube/v3/liveChat/messages?liveChatId=%s&",livechatid);

//	memset( argv[1], '-', strlen( argv[1] ) );
//	memset( argv[2], '-', strlen( argv[2] )  );

	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = curlurl;
	req.AuxData = 0;
	req.AuxDataLength = 0;

	char oauthbear[8192];
	FILE * f = fopen( "../.oauthtoken.txt", "r" );
	if( !f )
	{
		fprintf( stderr, "Error: no oauth token found.  Run yt_oauth_helper\n" );
		return 0;
	}
	fscanf( f, "%s", oauthbear );
	fclose( f );	
	char auxhead[8192];
	sprintf( auxhead, "Authorization: Bearer %s", oauthbear );
	req.AddedHeaders = auxhead;


	sprintf( curlurl, "%spart=%s%s%s",
		curlurlbase,
		reqtype,
		(nextpagetoken&&*nextpagetoken)?"&pageToken=":"",
		(nextpagetoken&&*nextpagetoken)?*nextpagetoken:"" );

	//printf("%s\n", curlurl );

	//Get a bunch of messages
	struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

	r->payload[r->payloadlen-1] = 0;
	//Uncomment this if you aren't getting anything.
	//printf( "PAYLOAD: %s\n", r->payload );

	int tokenlen = 131072;
	jsmntok_t * tokens = malloc( sizeof( jsmntok_t ) * tokenlen );
	memset( tokens, 0, sizeof( jsmntok_t ) * tokenlen  );
	jsmn_parser jsmnp;


	jsmn_init( &jsmnp );
	int tottoks = jsmn_parse(&jsmnp, r->payload, r->payloadlen,
		tokens, tokenlen );

	char * ret = 0;

	if( tottoks > 0 )
	{
		jsmntok_t * tok = &tokens[0];
		ret = ProcessChatMessageResponse( r->payload, &tok, &tokens[tottoks], pollinfo, nextpagetoken, include_history );
	}
	else
	{
		fprintf( stderr, "Error %d parsing the JSON : %s\n", tottoks, r->payload );
		ret = 0;
	}
	free( tokens );
	CNHTTPClientCleanup( r );
	return ret;
}


#ifdef BUILD_EXE

int main( int argc, char ** argv )
{

	int show_history = 0;
	int notfirst = 0;
	int pollinfo = 0;

	if( argc != 3 )
	{
		fprintf( stderr, "Error! Usage: ./chatmon [livechatid] [show_history (0/1)]\n" );
		fprintf( stderr, "Reads youtube livestream comments and sends to STDOUT. Format: [user]\\t[text]\\n\n" );
		return -5;
	}	

	show_history = atoi( argv[2] );
	const char * livechatid = argv[1];//"EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU";
	char * nextpagetoken = 0;

	char * ret = GetLivechatData( livechatid, &nextpagetoken, show_history, &pollinfo );
	if( ret ) 
	{
		printf( "%s", ret );
		fflush( stdout );
	}

	while( 1 )
	{
		ret = GetLivechatData( livechatid, &nextpagetoken, 1, &pollinfo );
		if( ret ) 
		{
			printf( "%s", ret );
			fflush( stdout );
		}
		if( pollinfo )
			usleep( pollinfo * 1000 );
		else
			sleep(2);
	}

}
#endif

