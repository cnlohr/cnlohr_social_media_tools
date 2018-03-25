//
//Basic tool for editing the comment field within a song.
//

#include <stdio.h>
#include "id3v2lib/include/id3v2lib.h"

int main( int argc, char ** argv )
{
	if( argc != 2 )
	{
		fprintf( stderr, "Error: Use:  writeid3soruce [mp3 file] < comment\n" );
	}

	ID3v2_tag* tag = load_tag(argv[1]); // Load the full tag from the file
	if(tag == NULL)
	{
		tag = new_tag();
	}



	ID3v2_frame* comment_frame = tag_get_comment(tag); // Get the copyright message frame
	if( comment_frame )
	{
		ID3v2_frame_comment_content * comment_content = parse_comment_frame_content(comment_frame);
		if( comment_content )
		{
			printf("Current Comment:\n" );
			fwrite( comment_content->text->data, comment_content->text->size, 1, stdout );
			printf( "\n" );
		}
	}


	char inbuff[8192];
	int inpl = 0;
	while( ( inbuff[inpl++] = getchar() ) != EOF && inpl < sizeof(inbuff)-1 );
	inbuff[inpl-1] = 0;
	printf( "Setting...\n" );

	// Set the new info
	tag_set_comment(inbuff, 0, tag);

	printf( "Writing...\n" );

	// Write the new tag to the file
	set_tag(argv[1], tag);

	return 0;
}

