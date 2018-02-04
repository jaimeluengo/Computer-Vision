/*******************************************************************/
/* cclabel      accepts a segmented image as input and generates a */
/* labeled output image in which all pixels of a given connected   */
/* component are assigned a unique number.                         */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
{ /* prefix, value,   description                         */   
{    "if=",    0,   " input file  vtemp: local max filter "},
{    "of=",    0,   " output file "},
{    "r=",     0,   " range"},
{	 "-p",     0,   " flag to select labeling scheme "},
{     0,       0,   0}  /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  RVAL   par[2].val 
#define  PFLAG  par[3].val 
void setlabel(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
int        y,x;                     /* index counters               */
int label;
int first;
int range = RVAL;

main(argc, argv)
int argc;
char *argv[];
{
int        y,x;                     /* index counters               */
  VXparse(&argc, &argv, par);       /* parse the command line       */

  Vfread(&im, IVAL);                /* read image file              */
  Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
  if ( im.type != VX_PBYTE ) {      /* check image format           */
     fprintf(stderr, "vtemp: no byte image data in input file\n");
     exit(-1);
  }

  for (y = im.ylo ; y <= im.yhi ; y++) {
     for (x = im.xlo; x <= im.xhi; x++)  {
		im.u[y][x]=0;           
     }
   }
 label=1;
for (y = im.ylo ; y <= im.yhi ; y++) {
     for (x = im.xlo; x <= im.xhi; x++)  {
		if (im.u[y][x]==0){ /*checks if the pixel is part of an object and if it hasn't been labeled */
			first=tm.u[y][x];
			if(PFLAG){
				setlabel(x,y,first);
			}else{
				setlabel(x,y,label)
				label++;
			}
			if(label>255){
				label=1;}
		}           
     }
   } 
   Vfwrite(&im, OVAL);             /* write image file                */
   exit(0);
}

/* function to set each object to a label*/
void setlabel(int x, int y, int n) 
{						
	im.u[y][x] = n;
	if (tm.u[y+1][x]!=0 && im.u[y+1][x]==0 && y < im.yhi && abs(tm.u[y+1][x]-n)<range){
		setlabel(x,y+1,n);}
	if (tm.u[y-1][x]!=0 && im.u[y-1][x]==0 && y > im.ylo && abs(tm.u[y-1][x]-n)<range)){
		setlabel(x,y-1,n);}
	if (tm.u[y][x+1]!=0 && im.u[y][x+1]==0 && x < im.xhi && abs(tm.u[y][x+1]-n)<range)){
		setlabel(x+1,y,n);}
	if (tm.u[y][x-1]!=0 && im.u[y][x-1]==0 && x > im.xlo && abs(tm.u[y][x-1]-n)<range)){
		setlabel(x-1,y,n);}
}
