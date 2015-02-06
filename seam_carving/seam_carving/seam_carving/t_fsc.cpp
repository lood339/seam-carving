#include "fsc.h"
#include <time.h>

#if 0
int main()
{
	char *path = "org_img\\fs_img\\8.png";

	IplImage *orgImage = cvLoadImage(path);
	assert(orgImage);

	double tt = clock();
	int w = orgImage->width;
	FSC myFSC;
	myFSC.loadImage(orgImage);
	myFSC.shrink(w*0.2, true, true);
	myFSC.showSeamPos();
	printf("%f\n", clock() - tt);


	cvWaitKey(0);
	return 1;
}
#endif