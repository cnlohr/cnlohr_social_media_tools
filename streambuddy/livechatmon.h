char ChatWindowText[2048];

void * RunChatMon( void * v )
{
	int clf = open( "../chatlog.txt", O_RDONLY );
	if( clf <= 0 )
	{
		sprintf( ChatWindowText, "Error: can't open chat log.\n" );
	}
	int i;
	for( i = 0; i < sizeof( ChatWindowText )-1; i++ )
	{
		ChatWindowText[i] = ' ';
	}
	ChatWindowText[i] = 0;

	int already_init = 0;

	double dLast = 0;

	fcntl( clf, F_SETFL, O_NONBLOCK );
	while(!doquit)
	{
		char bufferout[2000];
		char buffer[500];
		int r = read( clf, buffer, sizeof( buffer ) - 1 );
		if( r < 0 )
		{
			sprintf( ChatWindowText, "Error: Fault reading from chat window file.\n" );
			break;
		}

		if( r == 0 )
		{
			already_init = 1;
			OGUSleep( 100000 );
			continue;
		}

		int ro = 0;
		for( i = 0; i < r; i++ )
		{
			if( buffer[i] == '\t' )
			{
				bufferout[ro++] = '-';
				bufferout[ro++] = '-';
				bufferout[ro++] = '-';
				bufferout[ro++] = '\n';
				bufferout[ro++] = '\t';
			}
			if( buffer[i] == '\n' )
			{
				bufferout[ro++] = '\n';
			}
			else if( buffer[i] == '\\')
			{
				if( i < r-1 )
				{
					i++;
					bufferout[ro++] = buffer[i];
				}
			}
			else
				bufferout[ro++] = buffer[i];
		}
		bufferout[ro] = 0;

		int end = sizeof( ChatWindowText );
		for( i = 0; i < end - ro-1; i++ )
		{
			ChatWindowText[i] = ChatWindowText[i+ro];
		}

		int j = 0;
		memcpy( ChatWindowText+i, bufferout, ro );
		if( !already_init ) continue;




		if( already_init && strstr( bufferout, "boom" ) )
		{
			srand( OGGetAbsoluteTime()*1000 );
			MakeFirework( rand()%(BRD_X),  rand()%(BRD_Y - 200) + 100, 1.0 );
		}
		if( already_init && strstr( bufferout, "moob" ) )
		{
			srand( OGGetAbsoluteTime()*1000 );
			MakeFirework( rand()%(BRD_X),  rand()%(BRD_Y - 200) + 100, -1.0 );
		}


		//printf( "%d %d %d,%s\n", ChatWindowText[i], i, r, ChatWindowText );
		OGUSleep( 100000 );

		double now = OGGetAbsoluteTime();
		if( now - dLast > 1000 )
		{
			dLast = now;
			printf( "Refreshing stream token\n" );
			int pipes[3];
			char tbuff[1024];
			int ret;
			char * argv[3] = { "bash", "./refreshtoken.sh", 0 };
			int procv = spawn_process_with_pipes( "bash", argv, pipes );
			OGUSleep(200000);
			if( procv < 1 ) goto closev;
			fcntl( pipes[1], F_SETFL, O_NONBLOCK );
			fcntl( pipes[2], F_SETFL, O_NONBLOCK );
			int r = read( pipes[2], tbuff, 1024 );
			if( r > 0 ) 
			{
				tbuff[r] = 0;
				printf( "refresh response (err): %s\n", tbuff );
			}
			r = read( pipes[1], tbuff, 1024 );
			if( r > 0 ) 
			{
				tbuff[r] = 0;
				printf( "refresh response: %s\n", tbuff );
			}
			if( procv > 0 )	waitpid(procv, &ret, 0);
			printf( "Stream token stat: %d %d\n", ret, procv );
			closev:
			close( pipes[0] );
			close( pipes[1] );
			close( pipes[2] );
		}
	}
}


