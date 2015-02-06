#include "integral_hist.h"
#include <time.h>
#include <iostream>
#include <vector>
#include "cvx_image\cvx_image.h"
#include "thresholdSeg.h"

using namespace::std;
using cvx_space::BwImage;
using cvx_space::RgbImage;

#if 1
int main()
{

	char fileNumber[] = "36";
	char fileDir[] = "org_img\\fs_img\\";

	string fileName = string(fileDir) + string(fileNumber) + string(".png");
	IplImage *image = cvLoadImage(fileName.c_str(), 1);
	assert(image);

	int width = image->width;
	int height = image->height;

	IplImage *copyImage = (IplImage *)cvClone(image);
	cvLine(copyImage, cvPoint(0, height/5), cvPoint(width, height/5), cvScalar(255, 255, 255));
	cvLine(copyImage, cvPoint(0, height*4/5), cvPoint(width, height*4/5), cvScalar(255, 255, 255));
	cvLine(copyImage, cvPoint(width/5, 0), cvPoint(width/5, height), cvScalar(255, 255, 255));
	cvLine(copyImage, cvPoint(width*4/5, 0), cvPoint(width*4/5, height), cvScalar(255, 255, 255));

	//	cvShowImage("line", copyImage);

	vector<float> scale;
	for (int i = 1; i<=3; ++i) {
		scale.push_back(0.1*i);
	}

	

	ThresholdSeg myTS;
	myTS.loadImage(image);
	myTS.getFgProbMap(0.2, 8, scale, false, true);
	myTS.overSegment(0.5, 500, 400, true);
	myTS.foregroundSeg(2.0, true);



	cvShowImage("org image", image);

	cvWaitKey(0);
	return 1;
}
#endif

