#include <time.h>
#include <iostream>
#include <vector>
#include "cvx_image\cvx_image.h"
#include "image_template\cvx_sf.h"
#include "image_template\image_draw.h"
#include "integral_hist.h"
#include "foreMap.h"

using namespace::std;
using cvx_space::BwImage;
using cvx_space::RgbImage;

#if 0
int main()
{
	char fileName[] = "org_img\\fs_img\\36.png";
	IplImage *image = cvLoadImage(fileName, 1);
	assert(image);

	int width = image->width;
	int height = image->height;

	IplImage *copyImage = (IplImage *)cvClone(image);
	cvLine(copyImage, cvPoint(0, height/5), cvPoint(width, height/5), cvScalar(255, 255, 255));
	cvLine(copyImage, cvPoint(0, height*4/5), cvPoint(width, height*4/5), cvScalar(255, 255, 255));
	cvLine(copyImage, cvPoint(width/5, 0), cvPoint(width/5, height), cvScalar(255, 255, 255));
	cvLine(copyImage, cvPoint(width*4/5, 0), cvPoint(width*4/5, height), cvScalar(255, 255, 255));

	cvShowImage("line", copyImage);

	vector<float> scale;
	for (int i = 1; i<=3; ++i) {
		scale.push_back(0.1*i);
	}

	float ratio = 0.2;
	bool isLab = true;
	int binNum = 8;
	bool isMax = true;
	IplImage *tMap = NULL;
	ForeMap myFM;
	myFM.set(image, ratio, binNum);
	myFM.computerCornerBin();
	myFM.expComputerMap(scale, true);

	cvWaitKey(0);
	return 1;
}
#endif