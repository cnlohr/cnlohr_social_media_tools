#include <sys/mman.h>
#include <math.h>

int cc_fd;
int dft_fd;
int notes_fd;
uint8_t * cc_lights_data;
uint8_t * cc_dft_data;
uint8_t * cc_notes_data;


#define CC_START_X 0
#define CC_END_X WIN_X

#ifdef FULL_1080P
#define CC_X 16
#define CC_Y 3
#define CC_START_Y WIN_Y
#define CCW_START_X WIN_X
#define CCW_END_X BRD_X
#endif

uint32_t CCtoHEX( float note, float sat, float value );
uint32_t HSVtoHEX( float hue, float sat, float value );


struct NoteDists {
	float amp;   //Amplitude of normal distribution
	float mean;  //Mean of normal distribution
	float sigma; //Sigma of normal distribution
	unsigned char taken; //Is distribution associated with any notes?
};


void DrawColorChord()
{
	int freqbins = 1;

#if 0
	if( cc_lights_data )
	{
		int x, y;
		uint8_t * mark = cc_lights_data + 4;

		int sx = CCW_END_X-CCW_START_X;
		int sy = BRD_Y-CC_START_Y;

		sx /= CC_X;
		sy /= CC_Y;

		for( y = 0; y < CC_Y; y++ )
		for( x = 0; x < CC_X; x++ )
		{
			CNFGColor( mark[0] | mark[1] << 8 | mark[2] << 16 );

			int rx = x * sx + CCW_START_X;
			int ry = y * sy + CC_START_Y;
			CNFGTackRectangle( rx, ry, rx + sx, ry + sy );
			mark+=3;
		}
	}
#endif
	if( cc_dft_data )
	{
		int octaves = ((int*)cc_dft_data)[0];
		freqbins = ((int*)cc_dft_data)[1];
		float * foldedbins = &((float*)cc_dft_data)[2];
		float * unfoldedbins = &((float*)cc_dft_data)[2+freqbins];
		int i = 0;
		float sx = CC_END_X-CC_START_X;
		sx /= octaves * freqbins;
		for( ; i < octaves * freqbins; i++ )
		{
			float rsat =  unfoldedbins[i]*20;
			if( rsat > .8 ) rsat = .8;
			CNFGColor( CCtoHEX( (i% freqbins)/(float) freqbins,1, rsat ) );
			int rx = i * sx + CC_START_X;
			int ry = BRD_Y - unfoldedbins[i]*500;
			CNFGTackRectangle( rx, ry, rx+sx+1, ry+1000 );
		}
	}

	if( cc_notes_data )
	{
		int notespop = ((int*)cc_notes_data)[0];
		struct NoteDists * nd = (struct NoteDists *)(&(((int*)cc_notes_data)[1]));
	}

	if( cc_lights_data )
	{
		const char * playing = "Now--->Playing";
		int x, y;
		uint8_t * mark = cc_lights_data + 4;
		for( y = 0; y < 2; y++ )
		for( x = 0; x < 7; x++ )
		{
			int r = mark[0]; int g = mark[1]; int b = mark[2];
			if( r > 230 ) r = 230; if( g > 230 ) g = 230; if( b > 230 ) b = 230;
			r += 20; g += 20; b += 20;
			CNFGColor( r |g << 8 | b << 16 );
			mark += 3;
			DrawFatTextAt( 5+x*BIG_SIZE*3,  WIN_Y+8+y*BIG_SIZE*6, BIG_SIZE, -1, -1, "%c", playing[x+y*7] );
		}
	}
	else
	{
		DrawFatTextAt( 5,  WIN_Y+8, BIG_SIZE, -1, -1, "Now -->\nPlaying\n" );
	}
}

void StartColorChord()
{
	dft_fd = shm_open("/ccdft", O_RDONLY, 0666);
	if (cc_fd < 0 ) {
		printf("shared memory failed\n");
	}
	else
	{
		cc_dft_data = mmap(0,16384, PROT_READ, MAP_SHARED, dft_fd, 0);
		if( cc_dft_data == MAP_FAILED ) {
			printf("CC Map failed\n");
		}
	}


	notes_fd = shm_open("/ccnotes", O_RDONLY, 0666);
	if (notes_fd < 0 ) {
		printf("notes shared memory failed\n");
	}
	else
	{
		cc_notes_data = mmap(0,16384, PROT_READ, MAP_SHARED, notes_fd, 0);
		if( cc_notes_data == MAP_FAILED ) {
			printf("CC Notes Map failed\n");
		}
	}


	cc_fd = shm_open("/cclights", O_RDONLY, 0666);
	if (cc_fd < 0) {
		printf("shared memory failed\n");
	}
	else
	{
		cc_lights_data = mmap(0,16384, PROT_READ, MAP_SHARED, cc_fd, 0);
		if (cc_lights_data == MAP_FAILED) {
			printf("CC Lights Map failed\n");
		}
	}

}

void StopColorChord()
{
	if( cc_lights_data ) munmap( cc_lights_data, 16384 );
	if( cc_fd ) close( cc_fd );
}






uint32_t CCtoHEX( float note, float sat, float value )
{
	float hue = 0.0;
	note = fmodf( note, 1.0 );
	note *= 12;
	if( note < 4 )
	{
		//Needs to be YELLOW->RED
		hue = (4 - note) / 24.0;
	}
	else if( note < 8 )
	{
		//            [4]  [8]
		//Needs to be RED->BLUE
		hue = ( 4 - note ) / 12.0;
	}
	else
	{
		//             [8] [12]
		//Needs to be BLUE->YELLOW
		hue = ( 12 - note ) / 8.0 + 1./6.;
	}
	return HSVtoHEX( hue, sat, value );
}


//0/6: RED
//1/6: YELLOW
//2/6: GREEN
//3/6: CYAN
//4/6: BLUE
//5/6: PURPLE
//6/6: RED
uint32_t HSVtoHEX( float hue, float sat, float value )
{

	float pr = 0;
	float pg = 0;
	float pb = 0;

	short ora = 0;
	short og = 0;
	short ob = 0;

	float ro = fmod( hue * 6, 6. );

	float avg = 0;

	ro = fmod( ro + 6 + 1, 6 ); //Hue was 60* off...

	if( ro < 1 ) //yellow->red
	{
		pr = 1;
		pg = 1. - ro;
	} else if( ro < 2 )
	{
		pr = 1;
		pb = ro - 1.;
	} else if( ro < 3 )
	{
		pr = 3. - ro;
		pb = 1;
	} else if( ro < 4 )
	{
		pb = 1;
		pg = ro - 3;
	} else if( ro < 5 )
	{
		pb = 5 - ro;
		pg = 1;
	} else
	{
		pg = 1;
		pr = ro - 5;
	}

	//Actually, above math is backwards, oops!
	pr *= value;
	pg *= value;
	pb *= value;

	avg += pr;
	avg += pg;
	avg += pb;

	pr = pr * sat + avg * (1.-sat);
	pg = pg * sat + avg * (1.-sat);
	pb = pb * sat + avg * (1.-sat);

	ora = pr * 255;
	og = pb * 255;
	ob = pg * 255;

	if( ora < 0 ) ora = 0;
	if( ora > 255 ) ora = 255;
	if( og < 0 ) og = 0;
	if( og > 255 ) og = 255;
	if( ob < 0 ) ob = 0;
	if( ob > 255 ) ob = 255;

	return (ob<<16) | (og<<8) | ora;
}
