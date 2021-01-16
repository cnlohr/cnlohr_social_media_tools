char NowPlaying[1024];


void * RunNowPlaying( void * v )
{
	char tbuff[1024];
	char * argv[3] = { "bash", "./currently_playing.sh", 0 };
	int pipes[3];
	int firstsong = 1;
	int k = 0;
	int ret;
	while(!doquit)
	{
		//sprintf( NowPlaying, "Hello, world %d\n", k++ );
		int procv = spawn_process_with_pipes( "bash", argv, pipes );
		OGUSleep(100000);
		if( procv < 1 ) goto closev;
		fcntl( pipes[1], F_SETFL, O_NONBLOCK );
		fcntl( pipes[2], F_SETFL, O_NONBLOCK );
		int r = read( pipes[2], tbuff, 1024 );
		if( r > 0 ) 
		{
			tbuff[r] = 0;
			printf( "Error: %s\n", tbuff );
		}
		r = read( pipes[1], tbuff, 1024 );
		if( r >= 0 ) tbuff[r] = 0;
		else tbuff[0] = 0;

//		if( procv > 0 ) kill( procv, -9 );
		if( procv > 0 )	waitpid(procv, &ret, 0);
		//snprintf( NowPlaying, sizeof(NowPlaying)-1, "%d %d %d - %s\n", r, k, ret, tbuff );
		{
			//We now can see if NowPlaying matches.
			if( strcmp( NowPlaying, tbuff ) != 0 && !firstsong )
			{
				printf( "Sending chat message: %s\n", tbuff );
				SendChatMessage( tbuff );
			}
			firstsong = 0;
			memcpy( NowPlaying, tbuff, r+1 );
		}
		k++;
		closev:
		close( pipes[0] );
		close( pipes[1] );
		close( pipes[2] );
	}
	return 0;
}
