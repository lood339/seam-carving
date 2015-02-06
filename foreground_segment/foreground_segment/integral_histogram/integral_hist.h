#ifndef INTEGRAL_HIST_H
#define INTEGRAL_HIST_H 1
//implement of "Integral histogram:A fast way to 
// extract histograms in cartesian spaces"

#include "cvx_image\cvx_image.h"
#include <vector>


class IntegralHist
{
public:
	IntegralHist();
	~IntegralHist();
	
	void propagation(IplImage *image, int binNum);	
	void intersection(int w, int h, const int *inbin, IplImage *rstImage);	
	static void getBin(IplImage *image, int binNum, int *bin);
private:
	IplImage *m_image;
	int *m_iBin;     //ingegral bin
	int  m_binNum;	
};

#endif