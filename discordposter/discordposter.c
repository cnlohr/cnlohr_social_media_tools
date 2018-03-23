#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <unistd.h>

int main( int argc, char ** argv )
{
	unsigned int sendmode;
	if( argc != 3 || ( sendmode = atoi( argv[2] ) ) > 1 )
	{
		fprintf( stderr, "Error! Usage: ./discordposter [discord id\"/\"token] [sendmode]\n" );
		fprintf( stderr, "  Where [sendmode] is: (1) for an embed, with title\\tmessage, or (0) a message\n" );
		return -5;
	}
	char * chatline;
	char discorddata[8192];
	char addedh[8192];
	char discordurl[256];
	sprintf( discordurl, "https://discordapp.com/api/webhooks/%s", argv[1] );
	memset( argv[1], '-', strlen( argv[1] ) );	//Prevent ps and /proc from seeing the variable.
    size_t bufsize = 7168;
    int    characters;
	struct cnhttpclientrequest reqdiscord;

	chatline = malloc( bufsize );

	memset( &reqdiscord, 0, sizeof( reqdiscord ) );
	reqdiscord.host = 0;
	reqdiscord.port = 0;
	reqdiscord.URL = discordurl;
	reqdiscord.AddedHeaders = addedh;
	reqdiscord.AuxData = discorddata;
	reqdiscord.AuxDataLength = 0;

    while( (characters = getline(&chatline,&bufsize,stdin)) >= 0 )
	{
		chatline[characters] = 0;
		int len = 0;

		if( sendmode == 0 )
		{
			len = snprintf( discorddata, sizeof(discorddata), "{\"content\":\"%s\" }", chatline );
		}
		else if( sendmode == 1 )
		{
			char * text = strchr( chatline, '\t' );
			char * author = chatline;
			if( text == 0 )
			{
				text = author;
				author = "(nil)";
			}
			else
			{
				text[0] = 0;
				text++;
			}
			len = snprintf( discorddata, sizeof(discorddata), "{ \"embeds\": [{\"title\":\"%s\", \"description\": \"%s\", \"type\": \"rich\", \"color\":\"65535\"}] }", author, text );
		}


		sprintf( addedh, "Content-Type: application/json\r\nContent-length: %d", len );
		reqdiscord.AuxDataLength = len;
printf( "%s\n", discorddata );
		struct cnhttpclientresponse * r = CNHTTPClientTransact( &reqdiscord );
		r->payload[r->payloadlen-1] = 0;
		if( r->payloadlen )
			fprintf( stderr, "%s\n", r->payload );
		CNHTTPClientCleanup( r );

		usleep( 100000 );
		bufsize = sizeof(chatline);
	}
	fprintf( stderr, "discordposter received EOF.\n" );
#if 0
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }


	char curlurl[8192];
	jsmn_parser jsmnp;


	const char * discordtoken = 0;
	char discordurl[8192];

	discordtoken = argv[1];

					//time_t curtime;
					//time(&curtime);
					//printf( "...\n" );
					char ctimebuffer[128];
					int ctn = snprintf( ctimebuffer, 127, "%s", ctime( &curtime ) );
					if( ctn ) ctimebuffer[ctn-1] = ':';
					//char fullmsg[8182];
					//snprintf( fullmsg, 7000, "%s%s",ctimebuffer, chatsnip );
					int len = snprintf( discorddata, sizeof(discorddata), "{ \"embeds\": [{\"title\":\"%s:%s\", \"description\": \"%s\", \"type\": \"rich\", \"color\":\"65535\"}] }", authorsnip, ctimebuffer, chatsnip );
					char addedh[1024];
					sprintf( addedh, "Content-Type: application/json\r\nContent-length: %d", len );
					reqdiscord.AddedHeaders = addedh;
					reqdiscord.AuxDataLength = len;

					struct cnhttpclientresponse * r = CNHTTPClientTransact( &reqdiscord );
					r->payload[r->payloadlen-1] = 0;
					//printf( "%s\n", r->payload );
					CNHTTPClientCleanup( r );

	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = curlurl;
	req.AddedHeaders = addedh;
	req.AuxData = 0;
	req.AuxDataLength = 0;

	
	{
	}
#endif

}
