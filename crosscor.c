//*******************************************************************/
/* crosscor:       Implements Normalized correlation               */
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
                {"if2=", 0, " input file  stereo image "},
                {"pfo=",  0, " text file with locations of original image "},
                {"pft=",  0, " text file with locations of transformed image"},
                {"ps=",  0, " patch size "},
//                {    "ofo=",    0,   " output file "},
                {    "oft=",    0,   " output file "},
                {0,      0, 0}  /* list termination */
        };
#define  IVAL1   par[0].val
#define  IVAL2   par[1].val
#define  POVAL   par[2].val
#define  PTVAL   par[3].val
#define  SVAL   par[4].val
//#define  OVALO   par[5].val
#define  OVALT   par[5].val

void DrawCross(int x, int y);
Vfstruct (im1);                      /* i/o original image structure          */
Vfstruct (imo);                      /* i/o original image structure          */
Vfstruct (tm1);                      /* original image structure            */
Vfstruct (im2);                        /* i/o stereo image structure          */
Vfstruct (imt);                      /* i/o original image structure          */
Vfstruct (tm2);
int        y,x;                     /* index counters               */
int range=30;

int main(int argc, char **argv) {
    VXparse(&argc, &argv, par);
    FILE *fpo,*fpt;
    int i,j;
    int n_points;  /*number of points in the file*/
    int px[100], py[100];
    int pxt[100], pyt[100];
//    int labelo_mat[1000][1000][256],labelt_mat[1000][1000][256];
    int yy, xx;
    int k=0,l=0; //more coutners
//    int label;
//    int n_regions,n_pixels_o[255]={0},n_pixels_t[255]={0}; /*number of regions in each image, and pixels in each region*/
    int flag=1; /*flag 1 enters the labeling loop; if it exists being 1 that means labels<250
 *              and finish loop (change flag to 2. if flag=0 at the end of the loop (label>255)
 *              and continue loop */
    int xcount = 0, ycount = 0;
    int size = atoi(SVAL);
    unsigned char template[100][100]; /* baseline patch size is 9x9 */
    int mse[100];        /* mean squared error */
    int minmse[100] = {2147483647};
    int ox[100] = {0};        /* point in stereo image that gives max mse */
    int oy[100] = {0};

/* Read text file of original image*/
    fprintf(stderr, "%s\n", POVAL);
    fpo = fopen(POVAL, "r");
    if (fpo == NULL) {
        fprintf(stderr, "The file is empty\n");
    } else {
        fscanf(fpo, "%d", &n_points);
        fprintf(stderr, "%d\n", n_points);
        i = 0;
        while ((fscanf(fpo, "%d %d", &px[i], &py[i])) != EOF) {
//            printf("[%d,%d]\n", px[i], py[i]);
            i++;
        }
        fclose(fpo);
    }
    if (size > 100) {
        fprintf(stderr, "patch size can't be bigger than 100");
        exit(-1);
    }

    /* Read text file of transformed image*/
    fprintf(stderr, "%s\n", PTVAL);
    fpt = fopen(PTVAL, "r");
    if (fpt == NULL) {
        fprintf(stderr, "The file is empty\n");
    } else {
        fscanf(fpt, "%d", &n_points);
        fprintf(stderr, "%d\n", n_points);
        i = 0;
        while ((fscanf(fpt, "%d %d", &pxt[i], &pyt[i])) != EOF) {
//            printf("[%d,%d]\n", pxt[i], pyt[i]);
            i++;
        }
        fclose(fpt);
    }
    if (size > 100) {
        fprintf(stderr, "patch size can't be bigger than 100");
        exit(-1);
    }

    size=25;
/*Read images*/
    Vfread(&im1, IVAL1);                /* read image file              */
    Vfembed(&tm1, &im1, 1, 1, 1, 1);       /* image structure with border  */
    Vfread(&imo, IVAL1);                /* read image file              */

    Vfread(&im2, IVAL2);                /* read image file              */
    Vfembed(&tm2, &im2, 1, 1, 1, 1);       /* image structure with border  */
    Vfread(&imt, IVAL2);                /* read image file              */


    if (im1.type != VX_PBYTE) {      /* check image format           */
        fprintf(stderr, "vtemp: no byte image data in input file\n");
        exit(-1);
    }
    if (im2.type != VX_PBYTE) {      /* check image format           */
        fprintf(stderr, "vtemp: no byte image aata in input file\n");
        exit(-1);
    }
    /*Check interest points is within picture boundaries*/
    for (i = 0; i < n_points; i++) {
        if (py[i] - size / 2 < 0 || py[i] + size / 2 > im1.yhi || px[i] - size / 2 < 0 || px[i] + size / 2 > im1.xhi) {
            fprintf(stderr, "interest point number %d too close to border\n", i);
            exit(-1);
        }
    }

/*Start loop*/
/*store pixel values around interest point into template array*/

    for (i = 0; i < n_points; i++) {
            j=30;
			size= j;
			minmse[i] = 2147483647;
			xcount = 0;
			ycount = 0;
			for (y = py[i] - size / 2; y <= py[i] + size / 2; y++) {
				for (x = px[i] - size / 2; x <= px[i] + size / 2; x++) {

					template[ycount][xcount] = im1.u[y][x];
					xcount = xcount + 1;
				}
				xcount = 0;
				ycount = ycount + 1;
			}

	/*compare template values to patches in stereo image */
    /* iterate through entire stereo image */

			for (y = im2.ylo; y <= im2.yhi - size; y++) {
				for (x = im2.xlo; x <= im2.xhi - size; x++) {
					/* iterate over 9x9 template */

    				for (yy = 0; yy < size; yy++) {
                        for (xx = 0; xx < size; xx++) {

                            mse[i] = mse[i] + abs(im2.u[y + yy][x + xx] - template[yy][xx]) *
                                              abs(im2.u[y + yy][x + xx] - template[yy][xx]);
                        }

                    }

                    if (mse[i] < minmse[i]) {
                        minmse[i] = mse[i];
                        ox[i] = x + size / 2;
                        oy[i] = y + size / 2;
                    }
                    mse[i] = 0;

                }
            }
            DrawCross(ox[i],oy[i]);
            if(abs(ox[i]-pxt[i])+abs(oy[i]-pyt[i])<12) {
                printf("%d ,%d\n,[%d %d]\n", j, minmse[i], ox[i], pxt[i]);
                printf(" [%d %d]\n", oy[i], pyt[i]);
            }
    }

    Vfwrite(&imt, OVALT);             /* write image file                */
    exit(0);
}
void DrawCross(int x, int y) {
    int i, j;
    for (i = -10; i <= 10; i++) {
        for (j = -2; j <= 2; j++) {
            imt.u[y + i][x + j] = 255;
            imt.u[y + j][x + i] = 255;
        }
    }
}
void circle(int x)
{
    int i,j;
    for(i=-x;i<x;i++)
    {
        for(j=-x;j<x;j++)
        {
            if(i*i+j*j<x*x)
                imt.u[j][i] = 255;
        }
    }
}
