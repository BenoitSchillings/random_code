#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PN 9
#define PN2 4

float psf[PN][PN];


#define N 24
#define N2 12

float padding[8192];
float input[N][N];
float padding0[8192];

float output[N][N];
float padding1[8192];
float a[N][N];
float padding1[8192];
float b[N][N];
float padding2[8192];
float c[N][N];
float padding3[8192];


void copy_image_to_a()
{
    int     x,y;

    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            a[x][y] = input[x][y];
        }
    }
}


void copy_b_to_image()
{
    int     x,y;
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            input[x][y] = b[x][y];
        }
    }
}



void conv_a_into_b()
{
    int     x,y;
    int     dx,dy;
    

    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            float   sum;
            sum = 0;
            for (dx = 0; dx < PN; dx++) {
                for (dy = 0; dy < PN; dy++) {
                    sum += a[x + dx - PN2][y + dy - PN2] * psf[PN-dx][PN-dy];
                }
            }
            
            b[x][y] = sum;
        }
    }
}




void deconv()
{
    int     mx, my;
    float   max;
    
    int x,y;
    
    max = 0;
    
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            if (input[x][y] > max) {
                max = input[x][y];
                mx = x;
                my = y;
            }
        }
    }
    mx = rand() % (N - PN) + PN2;
    my = rand() % (N - PN) + PN2;
    max = input[mx][my];
    
    if (max < input[mx-1][my])
        max = 0;
    if (max < input[mx+1][my])
        max = 0;
    if (max < input[mx][my-1])
        max = 0;
    if (max < input[mx][my+1])
        max = 0;
   
    
    if (max > 0) {
        int dx, dy;
        
        float mul = max / 2000.0;
        char reject = 0;
        
        for (dx = 0; dx < PN; dx++) {
            for (dy = 0; dy < PN; dy++) {
                if ((input[mx + dx - PN2][my + dy - PN2] - psf[dx][dy] * mul) < 0)
                    reject = 1;
            }
        }
        
        if (reject == 0) {
            for (dx = 0; dx < PN; dx++) {
                for (dy = 0; dy < PN; dy++) {
                    input[mx + dx - PN2][my + dy - PN2] -= psf[dx][dy] * mul;
                }
            }
            output[mx][my] += mul;
        }

    }
    
    
}



void init()
{
	for (int i = 0; i < 8192; i++) {
		padding[i] = 0;
		padding0[i] = 0;
		padding1[i] = 0;
	}
    
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            input[x][y] = 0.0;
        }
    }
    
    float k = 1;
    
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            input[N2 + dx][N2 + dy] = k;
            k += 0.2;
        }
    }
    input[N2][N2] = 0;
    
    for (int x = 0; x < PN; x++) {
        for (int y = 0; y < PN; y++) {
            psf[x][y] = 0.0;
        }
    }
    
    
    psf[PN2][PN2] = 0.6/2;
    psf[PN2+1][PN2] = 0.25/2;
    psf[PN2-1][PN2] = 0.25/2;
    psf[PN2][PN2+1] = 0.25/2;
    psf[PN2][PN2-1] = 0.25/2;
    
    copy_image_to_a();
    conv_a_into_b();
    copy_b_to_image();

}

void dump()
{
    for (int y = (N2-7); y < (N2+7); y++) {
        for (int x = (N2-7); x < (N2+7); x++) {
            printf("\t%2.2f", output[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}




void main()
{
    float   max_sharp = 0;
    init();
    dump();
    
    int cnt = 0;
    while(1) {
        deconv();
        if (cnt % 222200 == 0) {
            dump();
        }
        cnt++;
    }
}
