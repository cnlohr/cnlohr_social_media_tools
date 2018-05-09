
#define FCOLORS 3
struct Firework
{
	//int colors[FCOLORS];
	float colors[FCOLORS];
	float weights[FCOLORS];
	double time;
	double moob;
	int x, y;
};

#define FIREWORKS 20
int headfirework = 0;
struct Firework fireworks[FIREWORKS];

void DrawFireworks()
{
	int i, j;
	double Now = OGGetAbsoluteTime();
	for( i = 0; i < FIREWORKS; i++ )
	{
		struct Firework * f = &fireworks[i];
		double diff = Now - f->time;
		if( diff > 5 ) continue;
		if( f->moob < 0 ) diff = 3.0-diff;

		srand( f->x+f->y*1000);
		int lx = f->x;
		int ly = f->y + diff*diff*40;

		int c = FCOLORS-1;
		for( ; c >= 0; c-- )
		{
			float weight = f->weights[c];
			int sparks = weight*200;
			for( j = 0; j < sparks; j++ )
			{
				float bright = sin( (rand() % 100 )/4.0 * diff ) + 1.2 - diff * .4;
				if( bright < .1 ) bright = .1;
				CNFGColor( CCtoHEX( f->colors[c], 1, bright ) );

				float ang = j * 3.14159 * 2.0 / sparks + (rand()%100);
				float amp = 40 * ((rand()%100)/50.0+.2) * pow( diff, .6 )*2.0;
				float fx = sin( ang ) * amp;
				float fy = cos( ang ) * amp;
				float ramp = bright*5.0;
				CNFGTackRectangle( lx-ramp+fx, ly-ramp+fy, lx+ramp+fx, ly+ramp+fy );
			}

		}
	}
}

void MakeFirework( int x, int y, float moob )
{
	struct Firework * f = &fireworks[headfirework];
	if( ++headfirework >= FIREWORKS ) headfirework = 0;

	f->x = x;
	f->y = y;
	f->time = OGGetAbsoluteTime(); 
	f->moob = moob;
	int i;
	for( i = 0; i < FCOLORS; i++ )
	{
		if( ndd[i].taken && ndd[i].amp > 0 )
		{
			f->colors[i] = ndd[i].mean/freqbins;
			f->weights[i] = ndd[i].amp;
		}
		else
		{
			f->weights[i] = 0;
		}
	}
}

