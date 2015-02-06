#include "backward_seam.h"
#include <time.h>

#if 0
int main()
{
	//25
	char *path = "org_img\\fs_img\\25.png";

	IplImage *orgImage = cvLoadImage(path);
	assert(orgImage);

	double tt = clock();
	int w = orgImage->width;
	/*BackwardSeam myBS;
	myBS.loadImage(orgImage);
	myBS.shrink(w*0.2, false, true);
	myBS.showSeamPos();
	printf("%f\n", clock() - tt);*/

	IplImage *resizeImage = cvCreateImage(cvSize(orgImage->width*0.8, orgImage->height), 
		IPL_DEPTH_8U, 3);
	cvZero(resizeImage);

	cvResize(orgImage, resizeImage);
	cvSaveImage("resized.png", resizeImage);


	cvWaitKey(0);
	return 1;
}
#endif