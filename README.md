# Computer-Vision
Image Processing, Image Filtering, Edge Detection and Object Recognition

# bound.c
Accepts a segmented image as input and generates an image in which the segment boundary pixels are set to 255, interior pixels are set to 128 and the background remains set to zero.
# cclabel.c
1- First embed the image im into the image tm with a one pixel border. (already done in vtemp). We can now use the image im for the label (output) image since we have a copy of it in the image tm.
2- Clear the output (label) image, im (i.e. set all pixels (labels) to 0). Set the current label to 1.
3- Search the input image, tm, until an object pixel is found.
4- If the object pixel is not labeled (i.e. the output image, im, is zero for this pixel), then call the recursive function setlabel(i, j, n) where (i, j) is the object pixel location and n is the next label value.
5- Increment the label value.
6- Repeat the steps 3, 4, and 5 for all unlabeled object pixels.

# vits.c
1- Get an initial threshold thresh from the command line if specified 
or 
2- Compute the average gray value of image and use it as an initial threshold thresh
3- Consider the two image regions R1 and R2 that are determined by thresh. (i.e. pixels above thresh and pixels below thresh) Compute the 4- average gray values ( avg1, avg2) for R1 and R2. Note that the number of pixels in R1 or R2 may be zero.
5- Compute a new threshold using: thresh = (avg1 + avg2)/2
6- Repeat 2-4 until avg1 and avg2 do not change between successive iterations
7- Apply the threshold to the image (values above thresh set to 255 otherwise 0)

# vgrow.c
1- Add a new input parameter, "r=", to vgrow. This is an integer value that will set the region pixel range. Assign this input value, (or default), to a global variable range.
2- Add a new flag, "-p", to vgrow to select the labeling scheme. When this flag is set, the label value will be the value of the first pixel in the region. Otherwise the regions are numbered sequentially.
3- Initialize im and tm as in cclabel. Note that tm and im should be global variables.
4- Scan the input image, tm, if the image pixel is not labeled (i.e. the output image, im, is zero for this pixel), then
    * set the global variable first to the pixel value.
    * if the "-p" flag is set, then call setlabel(j, i, first) else call setlabel with the next sequential label number. Note that you must go back to label number 1 after using label number 255.
5- Repeat step 4 for all unlabeled pixels.

# vsmed.c
Compute the median of each set of THREE consecutive image frames

# vsdif.c
Compute the thresholded difference between consecutive image frames
