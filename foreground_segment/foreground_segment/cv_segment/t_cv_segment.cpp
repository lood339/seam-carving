#include "cv_segment.h"
#include "cvx_image/cvx_image.h"
#include "image_template/image_draw.h"
#include <time.h>
#include <iostream>

using namespace::std;

#if 0
int main()
{
	//58
	//34
	//29
	//36.png
	char *inputName = "org_img\\fs_img\\154.png";   //
	char outputName[255] = "person7_s_seg_result.ppm";
	IplImage *orgImage = cvLoadImage(inputName, 1);
	assert(orgImage);
	int labelN = 0;
	IplImage *outImage = NULL;
	double tt = clock();
	CvSegment::segment(orgImage, outImage, &labelN, 0.5, 400, 400);
	fprintf(stderr, "cost time = %f\n", clock() - tt);

	IplImage *hsvImage = NULL;
	ImageDraw::randColorPaint_malloc(outImage, labelN, hsvImage);
	

	cvShowImage("out", outImage);
	cvShowImage("hsv", hsvImage);
	cvShowImage("org", orgImage);
	cvWaitKey(0);
	return 1;
}
#endif
