#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <unistd.h>

#define IBUFSIZE (8192-1024)

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
    size_t bufsize = IBUFSIZE;
    int    characters;
	struct cnhttpclientrequest reqdiscord;

	chatline = malloc( IBUFSIZE );

	memset( &reqdiscord, 0, sizeof( reqdiscord ) );
	reqdiscord.host = 0;
	reqdiscord.port = 0;
	reqdiscord.URL = discordurl;
	reqdiscord.AddedHeaders = addedh;
	reqdiscord.AuxData = discorddata;
	reqdiscord.AuxDataLength = 0;

    while( (characters = getline(&chatline,&bufsize,stdin)) >= 0 )
	{
		chatline[characters-1] = 0;
		printf( "%s\n", chatline );
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
		struct cnhttpclientresponse * r = CNHTTPClientTransact( &reqdiscord );
		if( r->payloadlen > 1 )
		{
			r->payload[r->payloadlen-1] = 0;
			fprintf( stderr, "Discord[%d]: %s\n", r->payloadlen, r->payload );
		}
		CNHTTPClientCleanup( r );

		usleep( 100000 );
		bufsize = IBUFSIZE;
	}
	fprintf( stderr, "discordposter received EOF.\n" );
	return 0;
}

