/*********************************************************************/
/* vsdif:     compute the thresholded difference between consecutive */
/* image frames        */
/*********************************************************************/

#include "VisXV4.h"          /* VisionX structure include file       */
#include "Vutil.h"           /* VisionX utility header files         */

VXparam_t par[] =            /* command line structure               */
{
{    "if=",    0,   " input file vssum: compute temporal mean"},
{    "of=",    0,   " output file "},
{    "th=",     0,   " number of frames "},
{     0,       0,    0}
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  TVAL   par[2].val

int
main(argc, argv)
int argc;
char *argv[];
{
V3fstruct (im);
V3fstruct (tm);
int        x,y,z;           /* index counters                 */
int        th;               /* Threshold value 		      */

    VXparse(&argc, &argv, par); /* parse the command line    */

    th = (TVAL ? atoi(TVAL) : 127); /* read n, default is n=1   */

    while (Vbfread( &im, IVAL, 2)) {
	if ( im.type != VX_PBYTE || im.chan != 1) { /* check format  */
           fprintf (stderr, "image not byte type\n");
           exit (1);
        }
        for (y = im.ylo; y <= im.yhi; y++) {
           for (x = im.xlo; x <= im.xhi; x++) {
				  if(abs(im.u[0][y][x]-im.u[1][y][x])>= th){
					  im.u[0][y][x]=255;
				  }else{
					  im.u[0][y][x]=128;
				  }
            }
        }
        V3fwrite (&im, OVAL); /* write the oldest frame */
    }
    exit(0);
}
