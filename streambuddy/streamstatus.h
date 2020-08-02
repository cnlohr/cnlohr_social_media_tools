int CurrentViewers = -1;

void * RunStreamStatus( void * v )
{
	char tbuff[1024];
	char * argv[3] = { "../ytstreamstats/ytstreamstats", STREAMID, 0 };
	int pipes[3];
	int firstsong = 1;
	int procv;
	int r;
	int ret;
	printf( "RUNNING STREAM STATUS\n" );
	while(!doquit)
	{
		//sprintf( NowPlaying, "Hello, world %d\n", k++ );

		procv = spawn_process_with_pipes( argv[0], argv, pipes );
		OGUSleep(1000000);

		if( procv < 1 ) goto closev;
		fcntl( pipes[1], F_SETFL, O_NONBLOCK );
		fcntl( pipes[2], F_SETFL, O_NONBLOCK );
		r = read( pipes[2], tbuff, sizeof(tbuff)-1 );
		if( r < 0 )
		{
			printf( "Problem with pipes\n" );
			goto closev;
		}
		if( r > 0 ) 
		{
			tbuff[r] = 0;
			printf( "Error: %s\n", tbuff );
		}

		r = read( pipes[1], tbuff, sizeof(tbuff)-1 );
		if( r >= 0 ) tbuff[r] = 0;
		else tbuff[0] = 0;
		if( procv > 0 ) kill( procv, -9 );
		if( procv > 0 )	waitpid( procv, &ret, 0);
		//printf( "STREAM STATUS: %s\n", tbuff );

		//printf( "%s\n", tbuff );
		char * ssv = strstr( tbuff, "\"concurrentViewers\"" );
		if( ssv && (ssv = strchr( ssv + 19, '\"' ) ) )
		{
			CurrentViewers = atoi( ssv + 1 );
		}

		//TODO: Show total time on stream running.

		closev:
		close( pipes[0] );
		close( pipes[1] );
		close( pipes[2] );
	}
}


