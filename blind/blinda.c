#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PN 7
#define PN2 3

float psf[PN][PN];
float psf0[PN][PN];

#define N 32
#define N2 16

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
float old_a[N][N];
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

void copy_b_into_c()
{
    int     x,y;
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            c[x][y] = b[x][y];
        }
    }
    
}

void copy_b_to_output()
{
    int     x,y;
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            output[x][y] = b[x][y];
        }
    }
}


void norm_input()
{
    float   sum;
    int     x,y;
    
    sum = 0;
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            sum += input[x][y];
        }
    }
    
   
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            input[x][y]/=sum;
        }
    }
}

void norm_a()
{
    float   sum;
    int     x,y;
    
    sum = 0;
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            sum += a[x][y];
        }
    }
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            a[x][y]/=sum;
        }
    }
    
}

void norm_b()
{
    float   sum;
    int     x,y;
    
    sum = 0;
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            sum += b[x][y];
        }
    }
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            b[x][y]/=sum;
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
                    sum += a[x + dx - PN2][y + dy - PN2] * psf[dx][dy];
                }
            }
            
            b[x][y] = sum;
        }
    }
}


void conv()
{
    conv_a_into_b();
    norm_b();
    
    copy_b_to_output();
    
}

float sharpness()
{
    int     x,y;
    float   sharp = 0;
    
    for (x = PN; x < N-PN; x++) {
        for (y = PN; y < N-PN; y++) {
                sharp += (output[x][y] - input[x][y]) * (output[x][y] - input[x][y]);
        }
    }
    
    return sharp;
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
            input[x][y] = 0.2;
        }
    }
    input[N2][N2] += 0.6;
    input[N2][N2+1] += 0.2;
    input[N2+1][N2] += 0.2;
    input[N2-1][N2] += 0.2;
    input[N2][N2-1] += 0.2;
    input[N2][N2+2] += 0.3; 
 
    float sum = 0;
    
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            sum += input[x][y];
        }
    }
    
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            input[x][y] /= sum;
        }
    }
   
    
    for (int x = 0; x < PN; x++) {
        for (int y = 0; y < PN; y++) {
            psf[x][y] = 0.1;
        }
    }
    psf[PN2][PN2] = 0.65;
    psf[PN2+1][PN2] = 0.25;
    psf[PN2-1][PN2] = 0.25;
    psf[PN2][PN2+1] = 0.25;
    psf[PN2][PN2-1] = 0.25;
}

void dump()
{
    for (int y = (N2-7); y < (N2+7); y++) {
        for (int x = (N2-7); x < (N2+7); x++) {
            printf("\t%2.2f-%2.2f", output[x][y] * 20.0, input[x][y] * 20.0);
        }
        printf("\n");
    }
}

void dump_psf()
{
    for (int y = 0; y < PN; y++) {
        for (int x = 0; x < PN; x++) {
            printf("\t%2.2f", psf[x][y]);
        }
        printf("\n");
    }
}

void save_psf()
{
    for (int x = 0; x < PN; x++) {
        for (int y = 0; y < PN; y++) {
            psf0[x][y] = psf[x][y];
        }
    }
}


void restore_psf()
{
    for (int x = 0; x < PN; x++) {
        for (int y = 0; y < PN; y++) {
            psf[x][y] = psf0[x][y];
        }
    }
}


void save_a()
{
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            old_a[x][y] = a[x][y];
        }
    }
}


void restore_a()
{
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            a[x][y] = old_a[x][y];
        }
    }
}


void norm_psf()
{
    float   sum = 0;
    
    for (int x = 0; x < PN; x++) {
        for (int y = 0; y < PN; y++) {
            sum += psf[x][y];
        }
    }
    
    for (int x = 0; x < PN; x++) {
        for (int y = 0; y < PN; y++) {
            psf[x][y] /= sum;
        }
    }
}

void modify_a()
{
    int x,y;
    float dv;
    
    x = rand()%N;
    y = rand()%N;
    
    dv = (rand()%1024) - 512;
    dv /= 33096.0;
    
    
    
    a[x][y] += dv;
    if (a[x][y] < 0)
        a[x][y] = 0;
    
    
    norm_a();
}


void modify_psf()
{
    int x,y;
    float dv;
    
    x = rand()%PN;
    y = rand()%PN;
    
    dv = (rand()%1024) - 512;
    dv /= 334096.0;
    

    
    psf[x][y] += dv;
    if (psf[x][y] < 0)
        psf[x][y] = 0;

    
    norm_psf();
}

void main()
{
    float   max_sharpness;
    init();
    norm_input();
    norm_psf();

    copy_image_to_a();
    

    int cnt = 0;
    conv();
    
    max_sharpness = sharpness();
    while(1) {
        cnt++;
         if ((cnt % 30000) > 15000) {
            save_psf();
            modify_psf();
            conv();
            float e1 = sharpness();
             if (e1 < max_sharpness) {
                 max_sharpness = e1;
                 system("clear");
                 dump();
                 printf("\n");
                 dump_psf();
                 printf("\n");
                 printf("\n %d %f\n", cnt, 1e6*max_sharpness);
             }
             else {
                 restore_psf();
             }
             
        }
        else {
            save_a();
            modify_a();
            conv();
            float e1 = sharpness();
            if (e1 < max_sharpness) {
                max_sharpness = e1;
                system("clear");
                dump();
                printf("\n");
                dump_psf();
                printf("\n");
                printf("\n %d %f\n", cnt, 1e6*max_sharpness);
            }
            else {
                restore_a();
            }
        }
    }
}
