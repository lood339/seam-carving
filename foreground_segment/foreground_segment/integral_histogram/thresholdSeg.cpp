#include "thresholdSeg.h"
#include "foreMap.h"
#include <iostream>
#include <string>
#include "..\cv_segment\cv_segment.h"
#include "image_template\image_draw.h"
#include "image_template\cvx_emap.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using cvx_space::BwImage;
using cvx_space::RgbImage;

ThresholdSeg::ThresholdSeg()
{
	m_orgImage = NULL;
	m_fgProbMap = NULL;
	m_segMap = NULL;
	m_aveCMap = NULL;
	m_labelN = 0;
	m_fgImage = NULL;
}
ThresholdSeg::~ThresholdSeg()
{
	
}

void ThresholdSeg::loadImage(IplImage *image)
{
	m_orgImage = (IplImage*)cvClone(image);
	m_fgProbMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvZero(m_fgProbMap);
	m_aveCMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvZero(m_aveCMap);
	m_fgImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvZero(m_fgImage);

}

void ThresholdSeg::getFgProbMap(float ratio, int binNum, const vector<float> &scale, 
								bool isExp, bool showMap)
{
	assert(m_orgImage);
	
	ForeMap myFM;
	myFM.set(m_orgImage, ratio, binNum);
	myFM.computerCornerBin();
	if (isExp) {
		myFM.expComputerMap(scale, showMap);
	}
	else {
		myFM.computerMap(scale, true);
	}

	
	myFM.copyForeMap(m_fgProbMap);

	if (showMap) {
		cvShowImage("foreground Map", m_fgProbMap);
	}
}
void ThresholdSeg::overSegment(float sigma, float k, int min_size, bool showSeg)
{
	assert(m_orgImage);
	CvSegment::segment(m_orgImage, m_segMap, &m_labelN, sigma, k, min_size);
	cout<<"over segment label number = "<<m_labelN<<endl;

	if (showSeg) {
		IplImage *hsvImage = NULL;
		ImageDraw::randColorPaint_malloc(m_segMap, m_labelN, hsvImage);
		cvShowImage("seg_hsv", hsvImage);
		cvReleaseImage(&hsvImage);
	}
}

void ThresholdSeg::foregroundSeg(float threshold, bool isShow)
{
	assert(m_fgProbMap);
	assert(m_segMap);

	int width = m_orgImage->width;
	int height = m_orgImage->height;

	//统计每块segment的平均saliency

	int *t_segStatics = new int[m_labelN * 2];
	memset(t_segStatics, 0, sizeof(t_segStatics[0])*m_labelN*2); //0 数值，1 num
	BwImage f_map(m_fgProbMap);
	BwImage lab_img(m_segMap);

	//统计每个区域占领的像素个数，和总的概率值
	for (int y = 0; y<height; ++y) {
		for (int x = 0; x<width; ++x) {
			int lab = lab_img[y][x];
			t_segStatics[2 * lab] += f_map[y][x];
			t_segStatics[2 * lab + 1]++;
		}
	}
	//计算平均值
	CvScalar v_avg = cvAvg(m_fgProbMap);
	
	//将label分成两类
	for (int i = 0; i<m_labelN; ++i) {
		t_segStatics[2 * i] = t_segStatics[2 * i]/t_segStatics[2*i+1];
		if (t_segStatics[2 * i] > threshold * v_avg.val[0]) {
			t_segStatics[2*i+1] = 1;
		}
		else	{
			t_segStatics[2*i+1] = 0;
		}
	}	

	//标定前景区域
	BwImage fg_img(m_fgImage);
	BwImage ave_img(m_aveCMap);
	for (int y = 0; y<height; ++y) {
		for (int x = 0; x<width; ++x) {
			int lab = lab_img[y][x];
			ave_img[y][x] = t_segStatics[2 * lab];
			if (t_segStatics[2 * lab+1] == 0) {
				fg_img[y][x] = 0;				
			}
			else
			{
				fg_img[y][x] = 255;
			}
		}
	}
	if (isShow) {
		
		cvShowImage("average foreground map", m_aveCMap);
		IplImage *showImage = (IplImage*)cvClone(m_orgImage);
		RgbImage show_img(showImage);
		for (int y = 0; y<height; ++y) {
			for (int x = 0; x<width; ++x) {
				if (fg_img[y][x] == 0) {
					show_img[y][x].r = 0;
					show_img[y][x].g = 0;
					show_img[y][x].b = 0;
				}
			}
		}
		cvShowImage("foreground", showImage);
		cvSaveImage("auto_segment.png", showImage);
		cvReleaseImage(&showImage);		
	}		
	delete []t_segStatics, t_segStatics = NULL;
}
void ThresholdSeg::saveFgArea(char *fileName)
{
	cvSaveImage(fileName, m_fgImage);
}
