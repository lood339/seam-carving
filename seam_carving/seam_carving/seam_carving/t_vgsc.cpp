#include <time.h>
#include "vgsc.h"

#if 0
int main()
{
	char *path = "org_img\\fs_img\\10.png";
//	char *path = "org_img\\stitch.jpg";

	IplImage *orgImage = cvLoadImage(path);
	assert(orgImage);

	double tt = clock();
	int w = orgImage->width;
	VGSC myVGSC;
	myVGSC.loadImage(orgImage);
	myVGSC.shrink(w * 0.3, true, true);
	myVGSC.showSeamPos();
	printf("%f\n", clock() - tt);


	cvWaitKey(0);
	return 1;
}
#endif