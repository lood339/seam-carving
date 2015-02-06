#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include "integral_hist.h"
#include "image_template\image_draw.h"
#include "image_template\cvx_sf.h"
#include "image_template\cvx_emap.h"

using std::cerr;
using std::endl;
using std::cout;
using std::string;
using std::vector;
using std::pair;
using std::min;
using cvx_space::RgbImage;
using cvx_space::BwImageInt;
using cvx_space::BwImage;

IntegralHist::IntegralHist()
{
	//快速计算图像两任意两点确定的rect的直方图
	m_image = NULL;
	m_iBin = NULL;
	m_binNum = 0;

}
IntegralHist::~IntegralHist()
{
	delete []m_iBin, m_iBin = NULL;
}

void IntegralHist::propagation(IplImage *image, int binNum)
{
	//binNum为直方图的bin数目
	//在每个像素点位置，计算改点位置与（0，0）左上点位置所确定
	//的长方形的累计直方图
	//只被使用一次
	if (image && image->nChannels == 3) {
		int width = image->width;
		int height = image->height;
		if (width >= 400 || height >= 400) {
			cerr<<"Warning! integral histogram,input image too large, might slow!"<<endl;
		}
		if (binNum != 4 && binNum != 8 && binNum != 16) {
			cerr<<"Error! integral histogram binNum must be 4, 8, or 16!"<<endl;
			return;
		}
		if (binNum > 8) {
			cerr<<"Warning! integral histogram,input binNum too large, might slow!"<<endl;
		}
		m_image = (IplImage*)cvClone(image);
		m_binNum = binNum;
		m_iBin = new int[width * height * 3 * m_binNum];
		memset(m_iBin, 0, sizeof(m_iBin[0]) * width * height * 3 * binNum);

		int step = 256/m_binNum;
		RgbImage org_img(image);

		//左上角第一个像素
		{
			int *pBin = m_iBin;
			int r_i = org_img[0][0].r/step;
			int g_i = org_img[0][0].g/step;
			int b_i = org_img[0][0].b/step;
			pBin[r_i] = 1;
			pBin[g_i + m_binNum] = 1;
			pBin[b_i + m_binNum + m_binNum] = 1;
		}
		//第一行
		for (int x = 1; x<width; ++x) {
			int *pBin = &m_iBin[3*x*m_binNum];
			memcpy(pBin, pBin - 3*m_binNum, sizeof(pBin[0])*3*m_binNum);

			int r_i = org_img[0][x].r/step;
			int g_i = org_img[0][x].g/step;
			int b_i = org_img[0][x].b/step;		
			pBin[r_i] += 1;
			pBin[g_i + m_binNum] += 1;
			pBin[b_i + m_binNum + m_binNum] += 1;
		}
		//第一列
		for (int y = 1; y<height; ++y) {
			int index = y * width;
			int *pBin = &m_iBin[3*index*m_binNum];
			memcpy(pBin, pBin - 3*m_binNum* width, sizeof(pBin[0])*3*m_binNum);

			int r_i = org_img[y][0].r/step;
			int g_i = org_img[y][0].g/step;
			int b_i = org_img[y][0].b/step;		
			pBin[r_i] += 1;
			pBin[g_i + m_binNum] += 1;
			pBin[b_i + m_binNum + m_binNum] += 1;
		}

		for (int y = 1; y<height; ++y) {
			for (int x = 1; x<width; ++x) {
				int index = y * width + x;
				int *pBin = &m_iBin[3*index*m_binNum];
				int *pleft = pBin - 3 * m_binNum;
				int *pUp = pBin - 3 * m_binNum * width;
				int *pLeftUp = pUp - 3 * m_binNum;
				for (int i = 0; i<3 * m_binNum; ++i) {
					pBin[i] = pleft[i] + pUp[i] - pLeftUp[i];
				}
				int r_i = org_img[y][x].r/step;
				int g_i = org_img[y][x].g/step;
				int b_i = org_img[y][x].b/step;
				pBin[r_i] += 1;
				pBin[g_i + m_binNum] += 1;
				pBin[b_i + m_binNum + m_binNum] += 1;
			}
		}
	}
}

void IntegralHist::getBin(IplImage *image, int binNum, int *bin)
{
	if (image && image->nChannels == 3 && bin) {
		if (binNum != 4 && binNum != 8 && binNum != 16) {
			cerr<<"Error! integral histogram getBin, binNum must be 4, 8, or 16!"<<endl;
			return;
		}
		int w = image->width;
		int h = image->height;
		int step = 256/binNum;
		memset(bin, 0, sizeof(bin[0])*3*binNum);
		RgbImage img(image);
		for (int y = 0; y<h; ++y) {
			for (int x = 0; x<w; ++x) {
				int r_i = img[y][x].r/step;
				int g_i = img[y][x].g/step;
				int b_i = img[y][x].b/step;

				bin[r_i] += 1;
				bin[g_i + binNum] += 1;
				bin[b_i + binNum + binNum] += 1;			
			}
		}
	}
	else
	{
		assert(0);
	}

}
void IntegralHist::intersection(int t_w, int t_h, const int *bin, IplImage *rstImage)
{
	//以图像中每个像素为中心的t_w, t_h的滑动窗口，和bin的直方图进行相交比较
	//得到相似图
	//w,h为目标图像的尺寸，bin为目标图像的直方图，rstImage返回saliency map,数值已经翻转

	if (m_image && bin && rstImage &&
		rstImage->nChannels == 1 && rstImage->depth == IPL_DEPTH_8U &&
		rstImage->width == m_image->width &&
		rstImage->height == m_image->height)
	{

		if (t_w%2 == 1) {
			t_w += 1;
			cout<<"w add 1"<<endl;
		}
		if (t_h%2 == 1) {
			t_h += 1;
			cout<<"h add 1"<<endl;
		}
		cout<<"rect width = "<<t_w<<endl;
		cout<<"rect height = "<<t_h<<endl;

		int width = m_image->width;
		int heigh = m_image->height;

		int h_w = t_w/2;
		int h_h = t_h/2;
		int *t_curBin = new int[3 * m_binNum];
		memset(t_curBin, 0, sizeof(t_curBin[0]) * 3 * m_binNum);

		IplImage *t_disImage = cvCreateImage(cvSize(width, heigh), IPL_DEPTH_32S, 1);
		cvZero(t_disImage);
		//显示
		BwImageInt dis_img(t_disImage);
		for (int y = h_h; y<heigh - h_h; ++y) {
			for (int x = h_w; x<width - h_w; ++x) {
				int index = (y + h_h) * width + (x + h_w);
				int *pBin  = &m_iBin[3*index*m_binNum];
				int *pLeft = pBin - 3 * m_binNum * t_w;
				int *pUP = pBin - 3 * m_binNum * t_h * width;
				int *pLeftUp = pUP - 3 * m_binNum * t_w;
				for (int i = 0; i<3*m_binNum; ++i) {
					t_curBin[i] = pBin[i] - pLeft[i] - pUP[i] + pLeftUp[i];
				}
				//直方图相交
				int dis = 0;
				for (int i = 0; i<3*m_binNum; ++i) {
					dis += min(bin[i], t_curBin[i]);
				}				
				dis_img[y][x] = dis;
			}
		}
		double max_v = 0;
		cvMinMaxLoc(t_disImage, NULL, &max_v);
		cvScale(t_disImage, rstImage, 255.0/max_v);

		//	cvShowImage("rstImage", rstImage);
		cvReleaseImage(&t_disImage);
		delete []t_curBin, t_curBin = NULL;
	}
	else
	{
		assert(0);
	}
}





