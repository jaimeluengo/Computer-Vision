/*********************************************************************/
/* vsmed   Temporal Domain Filtering                                 */
/*********************************************************************/

#include "VisXV4.h"          /* VisionX structure include file       */
#include "Vutil.h"           /* VisionX utility header files         */

VXparam_t par[] =            /* command line structure               */
{
{    "if=",    0,   " input file vssum: compute temporal mean"},
{    "of=",    0,   " output file "},
{     0,       0,    0}
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  NVAL   3

int
main(argc, argv)
int argc;
char *argv[];
{
V3fstruct (im);
V3fstruct (tm);
int        x,y,z,n;           /* index counters                 */
int i; 						/*counters						  */
int        value[NVAL];

    VXparse(&argc, &argv, par); /* parse the command line    */

    n = NVAL; /* window size is 3  */

    while (Vbfread( &im, IVAL, n)) {
	if ( im.type != VX_PBYTE || im.chan != 1) { /* check format  */
           fprintf (stderr, "image not byte type\n");
           exit (1);
        }
        for (y = im.ylo; y <= im.yhi; y++) {
           for (x = im.xlo; x <= im.xhi; x++) {
			  i=0;
              for (z = im.zlo; z <= im.zhi; z++) {
                   value[i]= im.u[z][y][x];
				   i++;
              }
              if((value[0]<=value[1] && value[0] >=value[2]) ||(value[0]>=value[1] && value[0] <=value[2])){
				  im.u[0][y][x] = value[0];
			  }else if( (value[1]<=value[0] && value[1] >=value[2]) ||(value[1]>=value[0] && value[1] <=value[2])){
				  im.u[0][y][x] = value[1];
			  }else{
				  im.u[0][y][x] = value[2];
			  }
            }
        }
        V3fwrite (&im, OVAL); /* write the oldest frame */
    }
    exit(0);
}
