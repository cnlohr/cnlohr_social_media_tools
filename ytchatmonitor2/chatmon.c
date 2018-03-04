#include <stdio.h>
#include <string.h>

#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>

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

void ReadChatEntry( const char * origtext, jsmntok_t ** tok )
{
	//Found items... Advnce to next token.
	int j;
	int nr_children = (*tok)->size;
	printf( "NR_CHILDREN: %d\n", nr_children );

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		int siz = (*tok)->end - (*tok)->start;

		char buff[siz+1];
		memcpy( buff, origtext + (*tok)->start, siz );
		buff[siz] = 0;
		printf( "GOT: %s\n", buff );

		ProcessNextElement( tok );
	}
}


int main()
{
	char curlurl[8192];
	jsmntok_t tokens[32768];
	jsmn_parser jsmnp;

	const char * livechatid = "EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU";
	const char * apikey = "AIzaSyA1XpoUMNDFOx0W4-HjiUI1uiahdfe20lE";

	sprintf( curlurl, "https://www.googleapis.com/youtube/v3/liveChat/messages?liveChatId=%s&part=snippet&key=%s",
		livechatid, 
		apikey );

	//Get a bunch of messages
	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = curlurl;
	req.AddedHeaders = "";
	req.AuxData = 0;
	req.AuxDataLength = 0;
	struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

	jsmn_init( &jsmnp );
	int tottoks = jsmn_parse(&jsmnp, r->payload, r->payloadlen,
		tokens, sizeof(tokens)/sizeof(tokens[0]) );
	if( tottoks <= 0 )
	{
		fprintf( stderr, "Error parsing the JSON\n" );
		return -1;
	}

	int i;

	int messages = 0;

	jsmntok_t * tok = &tokens[i];
	for( i = 1; i < tottoks; i++ )
	{
		tok = &tokens[i];

		if( memcmp( tok->start + r->payload, "items", 5 ) == 0 )
		{
			break;
		}

		i += tok->size;
	}

	//This gets us to the array [] element so we can see how many messages we can use.
	tok++;
	messages = tok->size;
	printf("MESSAGES: %d\n", messages );

	//We want to advance to the first message.
	tok++;


	for( ; i < messages; i++ )
	{
		ReadChatEntry( r->payload, &tok );
		tok++;
	}


	CNHTTPClientCleanup( r );

}

