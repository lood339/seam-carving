#include "sdsc.h"
#include <time.h>

#if 0
int main()
{
	char *path = "org_img\\fs_img\\23.png";

	IplImage *orgImage = cvLoadImage(path);
	assert(orgImage);

	double tt = clock();
	int w = orgImage->width;
	SDSC mySDSC;
	mySDSC.loadImage(orgImage);
	mySDSC.shrink(w*0.34, true, true);
	mySDSC.showSeamPos();
	printf("%f\n", clock() - tt);


	cvWaitKey(0);
	return 1;
}
#endif