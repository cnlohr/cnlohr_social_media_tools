#include <stdio.h>
#include <string.h>

#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>

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
	for( i = 1; i < tottoks; i++ )
	{
		jsmntok_t * tok = &tokens[i];

		if( memcmp( tok->start + r->payload, "items", 5 ) != 0 )
		{
			i += tok->size;
			continue;
		}

		i+=2;
		break;
	}
	for( ; i < tottoks; i++ )
	{
		//Found items... Advnce to next token.
		jsmntok_t * tok = &tokens[i];
		int siz = tok->end - tok->start;
		int j;
		int nrt = tok->size;
		printf( "==================== %d %d\n", i, nrt );
		for( j = i; j < i+nrt; j++ )
		{
			tok = &tokens[j];
			printf( "SIZ: %d %d %d\n", siz, tok->type, tok->size );
			char memcpybuf[siz+1];
			memcpy( memcpybuf, tok->start + r->payload, siz );
			memcpybuf[siz] = 0;
			printf( "%d/%d -> %s\n", tok->type, siz, memcpybuf );
		}

		printf( "--------------------------\n" );
	}


	CNHTTPClientCleanup( r );

}

