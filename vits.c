/*********************************************************************/
/* vits:     Iterative Threshold Selection Algorithm                 */
/*********************************************************************/

#include "VisXV4.h"          /* VisionX structure include file       */
#include "Vutil.h"           /* VisionX utility header files         */

VXparam_t par[] =            /* command line structure               */
{
{  "if=",   0,   " input file, vtpeak: threshold between hgram peaks"},
{  "of=",   0,   " output file "},
{  "-v",    0,   "(verbose) print threshold information"},
{  "t=",    0,   "threshold (default average of pixels)"},
{   0,      0,   0} /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  VFLAG  par[2].val
#define  TVAL   par[3].val

main(argc, argv)
int argc;
char *argv[];
{

    Vfstruct (im);                 /* input image structure          */
    int y,x;                       /* index counters                 */
    int i,j;


    int thresh;                    /* threshold                      */
    int sum[2];
    int avg1[256];
    int avg2[256];
    int n[2];						   /*number of pixels in the image  */

			     
    VXparse(&argc, &argv, par);    /* parse the command line         */


    while ( Vfread( &im, IVAL) ) {
        if ( im.type != VX_PBYTE ) {
              fprintf (stderr, "error: image not byte type\n");
              exit (1);
        }


  
         /* compute the threshold */
		sum[0]=0;
		n[0]=0;
        for (y = im.ylo; y <= im.yhi; y++){
            for (x = im.xlo; x <= im.xhi; x++){
				sum[0]+=im.u[y][x];
				n[0]++;
		}
	}
	thresh =sum[0]/n[0];
		
	if (TVAL) thresh = atoi(TVAL);  /* if d= was specified, get value */
	if (thresh < 0 || thresh > 255) {
		fprintf(stderr, "t= must be between 0 and 255\nUsing greyscale average\n");
			thresh = sum[0]/n[0];
	}
		/*Iterations to finds threshold*/
		i=0;
		avg1[0]=thresh;
		avg2[0]=thresh;
		do{
			n[0]=0;
			n[1]=0;
			sum[0]=0;
			sum[1]=0;
			for (y = im.ylo; y <= im.yhi; y++)
				for (x = im.xlo; x <= im.xhi; x++)
					if(im.u[y][x]>=thresh){
						sum[0]+=im.u[y][x];
						n[0]++;
					}else{
						sum[1]+=im.u[y][x];
						n[1]++;
					}	
				
			i++;
			if(n[0]!=0)
				avg1[i]=sum[0]/n[0];
			if(n[1]!=0)
				avg2[i]=sum[1]/n[1];
			thresh=(avg1[i]+avg2[i])/2;
		}while((avg1[i]!=avg1[i-1] || avg2[i]!=avg2[i-1])&&i<256);	

        if(VFLAG){
             fprintf(stderr, "Final threshold = %d\n",
                                 thresh);
	          for (j=0; j<=i; j++){
         		 fprintf(stderr,"iteration %d: avg1= %d, avg2= %d thres= %d\n",j+1,*(avg1+j),*(avg2+j),(*(avg2+j)+*(avg1+j))/2);
		  }
	}
  
        /* apply the threshold */
        for (y = im.ylo; y <= im.yhi; y++) {
            for (x = im.xlo; x <= im.xhi; x++) {
                 if (im.u[y][x] >= thresh) im.u[y][x] = 255;
                 else                      im.u[y][x] = 0;
            }
        }
  
        Vfwrite( &im, OVAL);
    } /* end of every frame section */
    exit(0);
}
