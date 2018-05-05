//
//Basic tool for editing the comment field within a song.
//

#include <stdio.h>
#include <stdlib.h>
#include "id3v2lib/include/id3v2lib.h"

int main( int argc, char ** argv )
{
	if( argc != 3 )
	{
		fprintf( stderr, "Error: Use:  writeid3soruce [mp3 file] [op] < comment (current argc = %d)\n", argc );
		fprintf( stderr, " where [op] is [r] for read comment and exit, [w] for write comment, and [y/Y] for youtube tab-output\n" );
		exit(-10);
	}

	ID3v2_tag* tag = load_tag(argv[2]); // Load the full tag from the file
	if(tag == NULL)
	{
		tag = new_tag();
	}


	ID3v2_frame* comment_frame = tag_get_comment(tag); // Get the copyright message frame
	if( argv[1][0] == 'r' )
	{
		if( comment_frame )
		{
			ID3v2_frame_comment_content * comment_content = parse_comment_frame_content(comment_frame);
			if( comment_content )
			{
				fwrite( comment_content->text->data, comment_content->text->size, 1, stdout );
				printf( "\n" );
			}
		}
	}
	else if( argv[1][0] == 'w' )
	{
		char inbuff[8192];
		int inpl = 0;
		while( ( inbuff[inpl++] = getchar() ) != EOF && inpl < sizeof(inbuff)-1 );
		inbuff[inpl-1] = 0;

		fprintf( stderr, "Writing: %s\n", inbuff );

		// Set the new info
		tag_set_comment(inbuff, 0, tag);

		// Write the new tag to the file
		set_tag(argv[2], tag);
	}
	else if( argv[1][0] == 'y' || argv[1][0] == 'Y' )
	{
		if( argv[1][0] == 'Y' ) printf( "%s\t", argv[2] );
		ID3v2_frame* title = tag_get_title(tag);
		ID3v2_frame* artist = tag_get_artist(tag);
		if( title )
		{
			printf( "Music: \"");
			ID3v2_frame_text_content* tc = parse_text_frame_content(title);
			//printf( "[%d %d]\n", tc->data[0], tc->data[1] );
			int i;
			for( i = 0; i < tc->size; i++ )
			{
				if( tc->data[i] != -1 && tc->data[i] != -2 && tc->data[i] != 0 )
					putchar( tc->data[i] );
			}
			//fwrite( tc->data, tc->size, 1, stdout );
			printf( "\"" );
		}
		else
			printf( "Unknown track");
		printf( " by " );
		if( artist )
		{
			ID3v2_frame_text_content* tc = parse_text_frame_content(artist);
			int i;
			for( i = 0; i < tc->size; i++ )
			{
				if( tc->data[i] != -1 && tc->data[i] != -2 && tc->data[i] != 0 )
					putchar( tc->data[i] );
			}

			printf( "\t" );
		}
		else
			printf( "Unknown artist\t" );

		ID3v2_frame* comment_frame = tag_get_comment(tag); // Get the copyright message frame
		if( comment_frame )
		{
			ID3v2_frame_comment_content * tc = parse_comment_frame_content(comment_frame);
			if( tc )
			{
				int i;
				for( i = 0; i < tc->text->size; i++ )
				{
					if( tc->text->data[i] == '\n' )
						putchar( '\t' );
					else if( tc->text->data[i] == '\r' )
						;
					else if( tc->text->data[i] != -1 && tc->text->data[i] != -2 && tc->text->data[i] != 0 )
						putchar( tc->text->data[i] );
				}

				printf( "\n" );
			}
			else
			{
				printf( "Unknown copyright information.\n" );
			}
		}
		else
		{
			printf( "Unknown copyright information.\n" );
		}
	}

	return 0;
}

