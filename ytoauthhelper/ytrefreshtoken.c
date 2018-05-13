#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <osg_aux.h>

static int LoadFileLineIntoBuffer( const char * folder_prefix, const char * file, char * buffer, int buffersize )
{
	char filename[1024];
	snprintf( filename, 1024, "%s/%s", folder_prefix, file );
	FILE * f = fopen( filename, "r" );
	if( !f )
	{
		fprintf( stderr, "Error: can't get client_id.txt\n" );
		return -1;
	}
	int c;
	int i = 0;
	while( ( c = fgetc( f ) ) != EOF && i < buffersize-1 )
	{
		if( c == '\n' ) break;
		buffer[i++] = c;
	}
	buffer[i] = 0;
	fclose( f );
	return i;
}

int YTRefreshOAuthToken( const char * folder_prefix)
{
	char client_secret[2048];
	char client_id[2048];
	char token[2048];
	char renew[2048];
	if( LoadFileLineIntoBuffer( folder_prefix, ".client_id.txt", client_id, sizeof( client_id ) ) < 5 ||
		LoadFileLineIntoBuffer( folder_prefix, ".client_secret.txt", client_secret, sizeof( client_secret ) ) < 5 ||
		LoadFileLineIntoBuffer( folder_prefix, ".oauthrenew.txt", renew, sizeof( renew ) ) < 5 ||
		LoadFileLineIntoBuffer( folder_prefix, ".oauthtoken.txt", token, sizeof( token ) ) < 5 )
	{
		return -1;
	}
	
	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = "https://accounts.google.com/o/oauth2/token";
	char auxhead[8192];
	req.AddedHeaders = auxhead;
	sprintf( auxhead, "Content-Type: application/x-www-form-urlencoded" );

	char content[8192];
	snprintf( content, 8191, "client_id=%s&client_secret=%s&refresh_token=%s&grant_type=refresh_token", client_id, client_secret, renew );

	req.AuxData = content;
	req.AuxDataLength = strlen( content );

	//Get a bunch of messages
	struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

	if( !r->payload )
	{
		fprintf( stderr, "Error: empty response received.\n" );
		return( -5 );
	}

	char * foundkey = strstr( r->payload, "access_token" );
	char * endkey;
	if( foundkey ) foundkey = strchr( foundkey+14, '\"' );
	if( foundkey ) endkey = strchr( ++foundkey, '\"' );
	if( !foundkey || !endkey )
	{
		fprintf( stderr, "Error: can't find token.\n" );
		return( -18 );
	}
	*endkey = 0;


	FILE * f = fopen( "../.oauthtoken.txt", "w" );
	fprintf( f, "%s\n", foundkey );
	fclose( f );
	return 0;
}

#ifdef MAKE_EXE
int main()
{
	int ret = YTRefreshOAuthToken( ".." );
	if( ret == 0 )
	{
		printf( "Refresh successful.\n" );
	}
	else
	{
		printf( "Refresh failed.\n" );
	}
	return ret;
}
#endif

