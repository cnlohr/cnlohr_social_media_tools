
#define FCOLORS 3
struct Firework
{
	int colors[FCOLORS];
	double time;
	int x, y;
};

#define FIREWORKS 20
int headfirework = 0;
struct Firework fireworks[FIREWORKS];

void DrawFireworks()
{
	int i;
	double Now = OGGetAbsoluteTime();
	for( i = 0; i < FIREWORKS; i++ )
	{
		struct Firework * f = &fireworks[i];
		double diff = Now - f->time;
		if( diff > 10 ) continue;
		srand( f->x+f->y*1000);
		int lx = f->x;
		int ly = f->y;

		int c = 0;
		for( ; c < FCOLORS; c++ )
		{
			CNFGColor( f->colors[c] );
			CNFGTackRectangle( lx-4, ly-4, lx+4, ly+4 );
		}
	}
}

void MakeFirework( int x, int y )
{
	struct Firework * f = &fireworks[headfirework];
	if( ++headfirework >= FIREWORKS ) headfirework = 0;

	f->x = x;
	f->y = y;
	f->time = OGGetAbsoluteTime(); 
	f->colors[0] = 0xFFFF00;
	f->colors[1] = 0xFF00FF;
	f->colors[2] = 0x00FFFF;
}

