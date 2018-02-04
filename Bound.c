/*******************************************************************/
/* bound:accepts a segmented image as input and generates an image */
/* in which the segment boundary pixels are set to 255, interior   */
/* pixels are set to 128 and the background remains set to zero.   */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
{ /* prefix, value,   description                         */   
{    "if=",    0,   " input file  vtemp: local max filter "},
{    "of=",    0,   " output file "},
{     0,       0,   0}  /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val

main(argc, argv)
int argc;
char *argv[];
{
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
int        y,x;                     /* index counters               */
  VXparse(&argc, &argv, par);       /* parse the command line       */

  Vfread(&im, IVAL);                /* read image file              */
  Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
  if ( im.type != VX_PBYTE ) {      /* check image format           */
     fprintf(stderr, "vtemp: no byte image data in input file\n");
     exit(-1);
  }
  for (y = im.ylo ; y <= im.yhi ; y++) {  /* compute the function */
     for (x = im.xlo; x <= im.xhi; x++)  {     /************************/
          if(tm.u[y][x-1]==0 & tm.u[y][x]!= tm.u[y][x-1]){
			im.u[y][x] = 128;}
		  else if (tm.u[y][x]==1 & tm.u[y][x]!= tm.u[y][x+1]){
			im.u[y][x] = 128;}
		  else if (tm.u[y][x]==1 & tm.u[y][x]== tm.u[y][x+1]){
			im.u[y][x] = 255;
		  }
     }
   }

   Vfwrite(&im, OVAL);             /* write image file                */
   exit(0);
}
