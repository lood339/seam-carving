#include <time.h>
#include "balanced_vgsc.h"
#include "image_template\cvx_emap.h"

#if 1
int main()
{
	//70
	//6
	//58
	char *path = "org_img\\fs_img\\58.png";

	IplImage *orgImage = cvLoadImage(path);
	assert(orgImage);

	double tt = clock();
	int w = orgImage->width;
	BalancedVGSC myBVGSC;
	myBVGSC.loadImage(orgImage);
	myBVGSC.shrink(w * 0.25, true, true);
	myBVGSC.showSeamPos();
	printf("%f\n", clock() - tt);

	cvWaitKey(0);
	return 1;
}
#endif