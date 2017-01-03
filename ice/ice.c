#include <stdio.h>
#include <math.h>
#define pi 3.1415926535

#include "bmp1.c"
#define	XS 2048 
#define uchar unsigned char

uchar    buffer[XS*XS*3];
float	 val[XS][XS];


#include <stdint.h>
 
/* These state variables must be initialized so that they are not all zero. */
 
uint32_t vx = 12, vy = 44, vz = 0x73278, vw = 0x282721;
 
uint32_t r(void) {
    uint32_t t;
 
    t = vx ^ (vx << 11);
    vx = vy; vy = vz; vz = vw;
    return vw = vw ^ (vw >> 19) ^ t ^ (t >> 8);
}


void plot()
{ 	
	int	x,y;

	for (y = 0; y < XS; y++) {
		for (x = 0; x < XS; x++) {
			float v = val[x][y];
			if (v > 255) v = 255;
			buffer[y*XS*3 + (x*3)] = v;
        		buffer[y*XS*3 + (x*3)+1] = v;
        		buffer[y*XS*3 + (x*3)+2] = v;
		}
	}
}

#define SEARCH 100

void heat_atom()
{
        int     x, y;

        x = 1 + (1.0 * (XS - 1)) * (r()%32000)/32000.0;
        y = 1 + (1.0 * (XS - 1)) * (r()%32000)/32000.0;

	val[x][y] = 0;
}

int cheat_x = XS/2;
int cheat_y = XS/2;

void add_atom()
{
	int	x, y;
	char	not_found = 1;
	int	dx, dy;
	int	tries;

	if (r() % 2 == 0) {
		x = 1 + (1.0 * (XS - 1)) * (r()%32000)/32000.0;
       		y = 1 + (1.0 * (XS - 1)) * (r()%32000)/32000.0;
	}
	else {
		x = cheat_x;
		y = cheat_y;

		x += r()%30 - 15;
		y += r()%30 - 15;
		if (x < 1) x = 1;
		if (y < 1) y = 1;
		if (x > (XS-1)) x = XS-1;
		if (y > (XS-1)) y = XS-1;
	}	
	
	tries = 0;
	
	
	do {
		dx = -20 + r()%41;
		dy = -20 + r()%41;

		x += dx;
		y += dy;
		
		if (x < 1) x = 1;
		if (y < 1) y = 1;
		if (x >= (XS-1)) x = (XS-2);
		if (y >= (XS-1)) y = (XS-2);

		    float v  = val[x+1][y] + val[x-1][y] + val[x][y-1] + val[x][y+1] + val[x+1][y+1] + val[x-1][y+1] + val[x-1][y-1] + val[x+1][y-1];

		     if (v >= 20) {
			cheat_x = x;
			cheat_y = y;	
			val[x][y] += 40;
			not_found = 0;
			val[x+1][y] *= 0.93;
                        val[x-1][y] *= 0.9;
                        val[x][y+1] *= 0.75;
                        val[x][y-1] *= 0.33;

		     }
		tries++;	
	} while(tries < 400 && not_found);
}


void main()
{
	int	x,y;

	for (y = 0; y < XS; y++)  {
		for (x = 0; x < XS; x++) {
			val[x][y] = 0;
		}
	}

	val[XS/2][XS/2] = 128;
	val[XS/3][XS/4] = 128;
	for (x = 0; x < XS; x++) {
		val[x][XS - 1] = 20;
	}
	for (y = 0; y < XS/2; y++) {
		val[0][XS - y] = 20;
	}
	
	int frame = 0;
	do {
		frame++;
		for (int iter = 0; iter < 25; iter++) { 
			for (int k = 0; k < 13000; k++) {	
				add_atom();
			}
	
			printf("%d\n", iter);
		}
		plot();
		char buf[256];	
		sprintf(buf, "out%3d.bmp", 100+frame);
		write_bmp(buf, XS, XS, (char*)buffer);
	} while(1);
}
