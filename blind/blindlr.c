#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PN 9
#define PN2 4

float psf[PN][PN];
float psf0[PN][PN];

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

void copy_a_to_output()
{
    int     x,y;
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            output[x][y] = a[x][y];
        }
    }
}

void calc_error_into_b()
{
    int     x,y;
    
    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            b[x][y] = input[x][y] / b[x][y];
        }
    }
}


void sub_c_from_a()
{
    int     x,y;
    
    for (x = 3; x < N - 3; x++) {
        for (y = 3; y < N - 3; y++) {
            a[x][y] *= c[x][y];
        }
    }
 }

void conv_b_into_c()
{
    int     x,y;
    int     dx,dy;
    

    for (x = PN2; x < N - PN2; x++) {
        for (y = PN2; y < N - PN2; y++) {
            float   sum;
            sum = 0;
            for (dx = 0; dx < PN; dx++) {
                for (dy = 0; dy < PN; dy++) {
                    sum += b[x + dx - PN2][y + dy - PN2] * psf[(PN-1)-dx][(PN-1)-dy];
                }
            }
            

            c[x][y] = sum;
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

void dumpx()
{
    return;
    for (int y = (N2-7); y < (N2+7); y++) {
        for (int x = (N2-7); x < (N2+7); x++) {
            printf(" %2.2f-%2.2f", input[x][y] * 20.0, b[x][y]);
        }
        printf("\n");
    }
    printf("\n\n");
}


void deconv()
{
    copy_image_to_a();
    for (int i = 0; i < 20; i++) {
        conv_a_into_b();
        calc_error_into_b();
        dumpx();
        conv_b_into_c();
        sub_c_from_a();
    }
    copy_a_to_output();
    
}

float sharpness()
{
    int     x,y;
    float   sharp = 0;
    
    for (x = PN; x < N-PN; x++) {
        for (y = PN; y < N-PN; y++) {
                sharp += output[x][y] * output[x][y];
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
            input[x][y] = 0.01;
        }
    }
    input[N2][N2] += 0.6;
    input[N2+1][N2+1] += 0.6;
    input[N2+2][N2+2] += 0.4;
    input[N2+2][N2] += 0.4;
    
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
            psf[x][y] = 0.0;
        }
    }
    psf[PN2][PN2] = 1.0;
    psf[PN2+1][PN2] = 0.55;}

void dump()
{
    for (int y = (N2-7); y < (N2+7); y++) {
        for (int x = (N2-7); x < (N2+7); x++) {
            printf("\t%2.2f", output[x][y] * 20.0);
        }
        printf("\n");
    }
}

void dumppsf()
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
    float   max_sharp = 0;
    init();
    norm_psf();
    deconv();
    dump();
    
    
    max_sharp = sharpness();
    int cnt = 0;
    
    while(1) {
        save_psf();
        modify_psf();
     
        deconv();
        float new_sharp = sharpness();
        if (new_sharp <= max_sharp) {
            restore_psf();
        }
        else {
            max_sharp = new_sharp;
        }
        cnt++;
        if (cnt % 55599 == 0) {
            for (int i = 0; i < 20; i++) {
                modify_psf();
            }
            deconv();
            max_sharp = sharpness();
        }
        if (cnt % 500 == 0) {
            system("clear");
            printf("%f %d\n", max_sharp, cnt);
            dump();
            printf("\n\n");
            dumppsf();
        }

    }
}
