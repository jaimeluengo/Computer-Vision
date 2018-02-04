/*******************************************************************/
/* lpt     Changes from cartesian to log polar space               */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

/*******************************************************************/
/* Example input text file:                                        */
/*		5                                                          */
/*		107 97                                                     */
/*		81 83                                                      */
/*		109 69                                                     */
/*		32 56                                                      */
/*		39 66                                                      */
/*  The first number is the number of points represented later in  */
/* [x y] coordinates                                               */
/*******************************************************************/


VXparam_t par[] =             /* command line structure            */
        { /* prefix, value,   description                         */
                {"if1=", 0, " input file  original image "},
                {"of=",  0, " LPT image "},
                {0,      0, 0}  /* list termination */
        };
#define  IVAL1   par[0].val
#define  OVAL   par[1].val

float LogPolar();

Vfstruct (im1);                      /* i/o original image structure          */
Vfstruct (im2);                        /* i/o stereo image structure          */
Vfstruct (tm1);


int main(int argc, char **argv) {
    VXparse(&argc, &argv, par);
    float log_base;
/*Read images*/
    Vfread(&im1, IVAL1);                /* read image file              */
    Vfread(&im2, IVAL1);                /* read image file              */
    Vfembed(&tm1, &im1, 1, 1, 1, 1);       /* image structure with border  */

    if (im1.type != VX_PBYTE) {      /* check image format           */
        fprintf(stderr, "vtemp: no byte image data in input file\n");
        exit(-1);
    }
    if (im2.type != VX_PBYTE) {      /* check image format           */
        fprintf(stderr, "vtemp: no byte image aata in input file\n");
        exit(-1);
    }

    log_base=LogPolar();

    Vfwrite(&im2, OVAL);             /* write image file                */

    exit(0);
}

float LogPolar(){
    int i,j,n=64;// n is n_theta= n_r
    int x1,x2,y1,y2;
    float r1,r2,P;
    float radii=im1.xhi+1, angles=im1.yhi+1;
    float center[1][2]={radii/2, angles/2};
    float d = sqrt(im1.xhi/2*im1.xhi/2+im1.yhi/2*im1.yhi/2); //norm(Vec2f(src.cols - center.x, src.rows - center.y));
    float log_base = pow(10.0, log10(d) / radii);
    float d_theta = M_PI / (float)angles;
    float theta = M_PI/2.0;
    float radius = 0;
    float map_x[512][512];
    float map_y[512][512];

    for (i = 0; i < angles; ++i)
    {
        for (j = 0; j < radii; ++j)
        {
            radius = pow(log_base, (float)(j)*512.0/545.0);
            //there must be a bug here since we always get 1 or 2 pixels out of the image
            float x = radius * sin(theta) + center[0][0];
            float y = radius * cos(theta) + center[0][1];
            map_x[i][j] = x;
            map_y[i][j] = y;
        }
        theta += d_theta;
    }

    /******* Bilinear interpolation ******/
    for (i = 0; i < angles; ++i) {
        for (j = 0; j < radii; ++j) {
            x1 = (int) (map_x[i][j] - 0.5);
            x2 = (int) (map_x[i][j] + 0.5);
            y1 = (int) (map_y[i][j] - 0.5);
            y2 = (int) (map_y[i][j] + 0.5);
            fprintf(stderr,"%d\n",radii);
            if(y1<im1.ylo){
                y1=im1.ylo;
            }else if(y2>im1.yhi){
                y2=im1.yhi;
            }else if(x1<im1.xlo){
                x1=im1.xlo;
            }else if(x2>im1.xhi){
                x2=im1.xhi;
            }

            r1 = (((float)(x2) - map_x[i][j]) / (float) (x2 - x1)) * im1.u[y1][x1]
                 + ((-(float)(x1) + map_x[i][j]) / (float) (x2 - x1)) * im1.u[y1][x2];
            r2 = (((float)(x2) - map_x[i][j]) / (float) (x2 - x1)) * im1.u[y2][x1]
                 + ((-(float)(x1) + map_x[i][j]) / (float) (x2 - x1)) * im1.u[y2][x2];
            P = (((float)(y2) - map_y[i][j]) / (float) (y2 - y1)) * r1
                + ((-(float)(y1) + map_y[i][j]) / (float) (y2 - y1)) * r2;
            im2.u[j][i] = P;
        }
    }
    return log_base;
}
