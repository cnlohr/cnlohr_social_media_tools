#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define IBUFSIZE 7168

int main( int argc, char ** argv )
{
	char * chatline;
    size_t bufsize = IBUFSIZE;
    int    characters;
	chatline = malloc( bufsize );
	const char * formatstr = (argc==1)?"%Y-%m-%d %H:%M:%S":argv[1];

    while( (characters = getline(&chatline,&bufsize,stdin)) >= 0 )
	{
		chatline[characters] = 0;
		char ctimebuffer[128];


		struct tm* tm_info;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		tm_info = localtime(&tv.tv_sec);
		//strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
		//printf("%s.%03d\n", buffer, millisec);
		strftime(ctimebuffer, 26, formatstr, tm_info);


		printf( "%s %s", ctimebuffer, chatline );
		bufsize = IBUFSIZE;
	}

	//time_t curtime;
	//time(&curtime);
					//printf( "...\n" );

}
