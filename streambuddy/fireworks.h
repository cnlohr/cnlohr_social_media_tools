
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
			int sparks = weight*100;
			for( j = 0; j < sparks; j++ )
			{
				float bright = sin( (rand() % 100 )/4.0 * diff ) + 1.2 - diff * .4;
				//if( bright < .1 ) bright = .1;

				//CNFGColor( CCtoHEX( f->colors[c], 1, bright ) );
				uint32_t color = CCtoHEX( f->colors[c], 1, bright );
				glColor4f( (color&0xff)/255.0, (color&0xff00)/65280.0, (color&0xff0000)/16711680, bright );

				float ang = j * 3.14159 * 2.0 / sparks + (rand()%100);
				float amp = 50 * ((rand()%100)/50.0+.2) * sqrt( diff*2.0 )*1.0;
				float fx = sin( ang ) * amp;
				float fy = cos( ang ) * amp;
				float ramp = bright*6.0;
				if( bright > 0.1 && amp > 0.1)
				{
					CNFGTackRectangle( lx-ramp+fx, ly-ramp+fy, lx+ramp+fx, ly+ramp+fy );
					CNFGDrawToTransparencyMode( 1 );
					CNFGColor( 0xffffff );
					CNFGTackRectangle( lx-ramp+fx, ly-ramp+fy, lx+ramp+fx, ly+ramp+fy );
					CNFGDrawToTransparencyMode( 0 );
				}


			}

		}
	}
}

struct ampset
{
	float color;
	float amp;
};

int AmpSetCompare(const void * a, const void * b)
{
	float v = ((struct ampset *)a)->amp - ((struct ampset *)b)->amp;
	return (0.f < v) - (v < 0.f);
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
	if( ndd )
	{
		//Get only the strongest notes.
		struct ampset notes[number_ndd];
		int onotes = 0;
		for( i = 0; i < number_ndd; i++ )
		{
			if( ndd[i].taken && ndd[i].amp > 0 )
			{
				notes[onotes].color = ndd[i].mean/freqbins;
				notes[onotes++].amp   = ndd[i].amp;
			}
		}
		qsort(notes,number_ndd, sizeof(struct ampset), AmpSetCompare);

		for( i = 0; i < FCOLORS; i++ )
		{
			if( i < onotes )
			{
				f->colors[i] = notes[i].color;
				f->weights[i] = notes[i].amp;
			}
			else
			{
				f->weights[i] = 0;
			}
		}
	}
	else
	{
		for( i = 0	; i < FCOLORS; i++ )
		{
			f->colors[i] = (rand()%100)/100.0;
			f->weights[i] = 1;
		}
	}
}

