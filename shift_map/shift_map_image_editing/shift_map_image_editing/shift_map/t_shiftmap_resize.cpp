#include "shiftmap_resize.h"

int main()
{
	char fileName[] = "org_img\\20.png";
	IplImage *orgImage = cvLoadImage(fileName, 1);
	assert(orgImage);

	int w = orgImage->width;
	ShiftMapResize mySMR;
	mySMR.loadImage(orgImage);
	mySMR.dataItem(true);
	mySMR.setNewWidth(w * 0.5);
	mySMR.setWeight(0.05);
	mySMR.proposeShiftMap(false);
	mySMR.optmizeShiftMap(2);
	mySMR.getResult(true);

	cvWaitKey(0);
	return 1;
}