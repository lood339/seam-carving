#include <string>
#include <iostream>
#include "foreMap.h"
#include "integral_hist.h"
#include "image_template\cvx_sf.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using cvx_space::BwImage;
using cvx_space::RgbImage;

ForeMap::ForeMap()
{
	m_orgImage = NULL;
	m_labImage = NULL;
	m_hist = NULL;
	m_foregroundProbMap = NULL;
	m_binNum = 8;
	m_ratio = 0.2;
	for (int i = 0; i<4; ++i) {
		m_cornerBin[i] = NULL;
	}	
}
ForeMap::~ForeMap()
{
	if (m_orgImage) {
		cvReleaseImage(&m_orgImage);
	}
	if (m_foregroundProbMap) {
		cvReleaseImage(&m_foregroundProbMap);
	}
}

void ForeMap::set(IplImage *image, float ratio, int binNum)
{
	//ratio:边界长度比率,binNum:直方图的桶数，
	//ratio = 0.2, isLab = true, saclar : 0.1, 0.2, 0.3
	assert(image && image->nChannels == 3);
	assert(binNum == 4 || binNum == 8 || binNum == 16);
	assert(ratio > 0 && ratio < 0.5);
	m_orgImage = (IplImage *)cvClone(image);
	m_foregroundProbMap = cvCreateImage(cvGetSize(m_orgImage), IPL_DEPTH_8U, 1);
	cvZero(m_foregroundProbMap);
	m_ratio = ratio;	
	m_binNum = binNum;
	m_labImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
	cvCvtColor(m_orgImage, m_labImage, CV_BGR2Lab);
	
}
void ForeMap::computerCornerBin(void)
{
	//只计算一次，计算四个角落位置的直方图
	if (m_labImage) {
		int w = m_labImage->width;
		int h = m_labImage->height;
		int cw = w * m_ratio;
		int ch = h * m_ratio;

		int startP[4][2] = {0, 0, 
							w - cw, 0,
							0, h - ch,
							w - cw, h - ch};
		//得到角落上的四张图
		IplImage *t_cImage[4] = {NULL};
		for (int i = 0; i<4; ++i) {
			t_cImage[i] = cvCreateImage(cvSize(cw, ch), IPL_DEPTH_8U, 3);
			cvSetImageROI(m_labImage, cvRect(startP[i][0], startP[i][1], cw, ch));
			cvCopy(m_labImage, t_cImage[i]);
			cvResetImageROI(m_labImage);
		}		
		for (int i = 0; i<4; ++i) {
			m_cornerBin[i] = new int[3 * m_binNum];
			memset(m_cornerBin[i], 0, sizeof(m_cornerBin[i][0]) * 3 * m_binNum);
		}
		for (int i = 0; i<4; ++i) {
			IntegralHist::getBin(t_cImage[i], m_binNum, m_cornerBin[i]);
		}
		for (int i = 0; i<4; ++i) {
			cvReleaseImage(&t_cImage[i]);
		}
	}

}
void ForeMap::computerMap(const std::vector<float> &scale, bool isShow)
{
	if (scale.size() != 0) {
		m_hist = new IntegralHist;
		m_hist->propagation(m_labImage, m_binNum);
		// scale的比率, 周围像素的saliency值默认为0
		int w = m_labImage->width;
		int h = m_labImage->height;
		m_scale = scale;		
		IplImage *t_probMap = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
		cvZero(t_probMap);	
		for (int i = 0; i<scale.size(); ++i) {
			computerOneScale(scale[i], t_probMap);
			cvAddWeighted(t_probMap, 1.0/m_scale.size(), m_foregroundProbMap, 1.0, 0, m_foregroundProbMap);
		}
		if (isShow) {
			cvShowImage("foreground probability map", m_foregroundProbMap);
		}
	}
	
}
void ForeMap::expComputerMap(const std::vector<float> &scale, bool isShow)
{
	if (m_labImage && scale.size() != 0) {
		int w = m_labImage->width;
		int h = m_labImage->height;
		m_scale = scale;

		//将原始图像扩展到周边区域
		std::vector<float>::iterator i = std::max_element(m_scale.begin(), m_scale.end());
		int ew = w * (*i);
		int eh = h * (*i);
		if (ew%2) {
			ew++;
		}
		if (eh%2) {
			eh++;
		}
		CvRect rect = cvRect(ew/2, eh/2, w, h);
		//保存指针
		IplImage *prevLabImage = m_labImage;
		m_labImage = cvCreateImage(cvSize(w + ew, h + eh), IPL_DEPTH_8U, 3);
		cvZero(m_labImage);
		ForeMap::expandImage(prevLabImage, ew, eh, m_labImage);
		//保存指针
		IplImage *prevForeProbMap = m_foregroundProbMap;
		m_foregroundProbMap = cvCreateImage(cvSize(w + ew, h + eh), IPL_DEPTH_8U, 1);
		cvZero(m_foregroundProbMap);
		
		//在扩大的图像上做直方图匹配
		computerMap(scale, false);

		//恢复数据
		cvSetImageROI(m_labImage, rect);
		cvCopy(m_labImage, prevLabImage);
		cvResetImageROI(m_labImage);

		cvSetImageROI(m_foregroundProbMap, rect);
		cvCopy(m_foregroundProbMap, prevForeProbMap);
		cvResetImageROI(m_foregroundProbMap);

		cvReleaseImage(&m_labImage);
		m_labImage = prevLabImage;
		cvReleaseImage(&m_foregroundProbMap);
		m_foregroundProbMap = prevForeProbMap;

		if (isShow) {
			cvShowImage("foreground probability map", m_foregroundProbMap);
		}
	}
	else
	{
		assert(0);
	}
}

void ForeMap::computerOneScale(float scale, IplImage *fProbMap)
{
	//计算一个缩放下的前景概率图
	if (m_labImage && m_hist && m_cornerBin[0]) {
		
		int w = m_labImage->width;
		int h = m_labImage->height;
		int r_w = w * m_ratio * scale;
		int r_h = h * m_ratio * scale;

		int *t_sBin = new int[m_binNum * 3];
		memset(t_sBin, 0, sizeof(t_sBin[0]) * m_binNum * 3);
	
		IplImage *t_probImage[4] = {NULL};
		for (int i = 0; i<4; ++i) {
			t_probImage[i] = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
			cvZero(t_probImage[i]);
		}
		//每个角落图像分别求值
		for (int i = 0; i<4; ++i) {
			for (int k = 0; k<m_binNum * 3; ++k) {
				t_sBin[k] = m_cornerBin[i][k] * scale;
			}
			m_hist->intersection(r_w, r_h, t_sBin, t_probImage[i]);
		}
		//取最大值
		cvMax(t_probImage[0], t_probImage[1], t_probImage[0]);
		cvMax(t_probImage[0], t_probImage[2], t_probImage[0]);
		cvMax(t_probImage[0], t_probImage[3], t_probImage[0]);
		//翻转
		cvSubRS(t_probImage[0], cvScalarAll(255.0), fProbMap, NULL);
		setBdyZero(fProbMap, r_w, r_h);

		delete []t_sBin, t_sBin = NULL;
		for (int i = 0; i<4; ++i) {
			cvReleaseImage(&t_probImage[i]);
		}
	}
	else{
		assert(0);
	}

}

void ForeMap::saveMap(char *fileName, bool isShow)
{
	cvSaveImage(fileName, m_foregroundProbMap);
	if (isShow) {
		cvShowImage("foreground probability map", m_foregroundProbMap);
	}
}
void ForeMap::copyForeMap(IplImage *image)
{
	if (image && image->width == m_foregroundProbMap->width &&
		image->height == m_foregroundProbMap->height) {
		cvCopy(m_foregroundProbMap, image);
	}
	else
	{
		assert(0);
	}
}
void ForeMap::expandImage(IplImage *image, int ew, int eh, IplImage *expImage)
{
	//图像向四个方向扩展，使用边界像素翻转算法
	if (image && expImage && image->nChannels == image->nChannels) {
		int width = image->width;
		int height = image->height;
		int h_w = ew/2;
		int h_h = eh/2;
		CvRect rect = cvRect(h_w, h_h, width, height);

		cvSetImageROI(expImage, rect);
		cvCopy(image, expImage, NULL);
		cvResetImageROI(expImage);

		RgbImage org_img(image);
		RgbImage exp_img(expImage);
		//top
		for (int y = 0; y<h_h; ++y) {
			for (int x = h_w; x<h_w + width; ++x) {
				int r_y = 2 * h_h - y;
				exp_img[y][x].r = exp_img[r_y][x].r;
				exp_img[y][x].g = exp_img[r_y][x].g;
				exp_img[y][x].b = exp_img[r_y][x].b;
			}
		}
		//down
		for (int y = 0; y<h_h; ++y) {
			for (int x = h_w; x<h_w + width; ++x) {
				int e_y = y + h_h + height;
				int o_y = h_h + height - y - 1;
				exp_img[e_y][x].r = exp_img[o_y][x].r;
				exp_img[e_y][x].g = exp_img[o_y][x].g;
				exp_img[e_y][x].b = exp_img[o_y][x].b;
			}
		}
		//left
		for (int y = 0; y<height + 2 * h_h; ++y) {
			for (int x = 0; x<h_w; ++x) {
				int o_x = 2 * h_w - x;
				exp_img[y][x].r = exp_img[y][o_x].r;
				exp_img[y][x].g = exp_img[y][o_x].g;
				exp_img[y][x].b = exp_img[y][o_x].b;
			}
		}
		//right
		for (int y = 0; y<height + 2 * h_h; ++y) {
			for (int x = 0; x<h_w; ++x) {
				int e_x = x + h_w + width;
				int o_x = h_w + width - x - 1;
				exp_img[y][e_x].r = exp_img[y][o_x].r;
				exp_img[y][e_x].g = exp_img[y][o_x].g;
				exp_img[y][e_x].b = exp_img[y][o_x].b;
			}
		}
	}
	else{
		assert(0);
	}
	



	
	//	cvShowImage("exp", expImage);
	//	cvSaveImage("exp.bmp", expImage);
}

void ForeMap::setBdyZero(IplImage *image, int b_w, int b_h)
{
	//将边界像素的前景概率设为0
	assert(image);
	assert(image->nChannels == 1);
	int width = image->width;
	int height = image->height;
	BwImage rst_img(image);
	for (int y = 0; y<height; ++y) {
		for (int x = 0; x<b_w; ++x) {
			rst_img[y][x] = 0;
		}
		for (int x = width - b_w; x<width; ++x) {
			rst_img[y][x] = 0;
		}
	}
	for (int x = b_w; x<width-b_w; ++x) {
		for (int y = 0; y<b_h; ++y) {
			rst_img[y][x] = 0;
		}
		for (int y = height - b_h; y<height; ++y) {
			rst_img[y][x] = 0;
		}
	}
}
