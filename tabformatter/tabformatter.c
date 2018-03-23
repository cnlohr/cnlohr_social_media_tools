#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IBUFSIZE 7168

int main( int argc, char ** argv )
{
	char * chatline;
    size_t bufsize = IBUFSIZE;
    int    characters;
	chatline = malloc( bufsize );

    while( (characters = getline(&chatline,&bufsize,stdin)) >= 0 )
	{
		chatline[characters-1] = 0;
		char * cts = strchr( chatline, '\t' );
		if( cts )
		{
			*cts = 0;
			cts++;
			printf( "**%s**:%s\n", chatline, cts );
		}
		else
		{
			printf( "%s\n", chatline );
		}
		fflush ( stdout );

		bufsize = IBUFSIZE;
	}

	//time_t curtime;
	//time(&curtime);
					//printf( "...\n" );

}
