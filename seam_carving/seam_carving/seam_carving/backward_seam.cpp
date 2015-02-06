#include "backward_seam.h"
#include "image_template\cvx_emap.h"
#include <iostream>

using cvx_space::BwImage;
using cvx_space::RgbImage;
using cvx_space::BwImageInt;
using std::cout;
using std::cerr;
using std::endl;

BackwardSeam::BackwardSeam()
{
	m_image = NULL;
	m_energyMap = NULL;
	m_accuMap = NULL;

}
BackwardSeam::~BackwardSeam()
{

}

void BackwardSeam::loadImage(IplImage *image)
{
	assert(image);
	m_image = (IplImage *)cvClone(image);
	m_energyMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvZero(m_energyMap);
	m_accuMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_32S, 1);	
	cvZero(m_accuMap);
	m_indexMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_32S, 1);

	BwImageInt id_map(m_indexMap);
	for (int y = 0; y<image->height; ++y) {
		for (int x = 0; x<image->width; ++x) {
			id_map[y][x] = x;
		}
	}

}
void BackwardSeam::shrink(int num, bool nSpeedUp, bool isShow)
{
	assert(m_image);
	int w = m_image->width;
	int h = m_image->height;
	if (num < 0 || num >= w) {
		return;
	}

	//能量图	
	CvxEMap::sobel(m_image, m_energyMap);
	//计算累计能量图
	calcAccumulateEnergy(false);

	OffsetMap eMap(m_energyMap);
	OffsetMap aMap(m_accuMap);
	OffsetMap oMap(m_image);
	OffsetMap idMap(m_indexMap);
	for (int i = 0; i<num; ++i) {
		vector<CvPoint> seamPos;
		//选择一条seam
		selectSeam(aMap, seamPos, false);		
		//改变能量图
		eMap.offsetLeft(seamPos);
		//改变累计能量图
		aMap.offsetLeft(seamPos);
		//改变原图
		oMap.offsetLeft(seamPos);
		if (nSpeedUp) {
			reCalcAccumulateEnergy(aMap, eMap, seamPos, false);
		}
		else	{
			reCalcAccumulateEnergy(aMap, eMap, false);
		}
		
		
		if (isShow) {
			calcSeamPos(idMap.image(), seamPos);
			idMap.offsetLeft(seamPos);
		}
	}

	if (isShow) {
		IplImage *resImage = cvCreateImage(cvSize(oMap.width(), oMap.height()), IPL_DEPTH_8U, 3);
		cvZero(resImage);

		cvSetImageROI((IplImage*)oMap.image(), cvRect(0, 0, oMap.width(), oMap.height()));
		cvCopy(oMap.image(), resImage);
		cvResetImageROI((IplImage*)oMap.image());
		cvShowImage("result", resImage);
		cvSaveImage("bksc.png", resImage);
	}
}
void BackwardSeam::showSeamPos(void)
{
	IplImage *seamImage = (IplImage*)cvClone(m_image);

	RgbImage seam_map(seamImage);
	for (int i = 0; i<m_seamPos.size(); ++i) {
		for (int y = 0; y<m_seamPos[i].size(); ++y) {
			int x = m_seamPos[i][y].x;
			seam_map[y][x].r = 255;
			seam_map[y][x].g = 0;
			seam_map[y][x].b = 0;
		}
	}
	cvShowImage("seam position", seamImage);
	cvSaveImage("backward_seam.png", seamImage);
	cvReleaseImage(&seamImage);
}
void BackwardSeam::calcSeamPos(const IplImage *idImage, const vector<CvPoint> &seamPos)
{
	//取得seam 在原来图像中的位置
	BwImageInt id_map(idImage);
	vector<CvPoint> nPos;
	for (int y = 0; y<seamPos.size(); ++y) {
		int x = seamPos[y].x;
		nPos.push_back(cvPoint(id_map[y][x], y));
	}
	m_seamPos.push_back(nPos);
}

void BackwardSeam::selectSeam(OffsetMap &accuMap, vector<CvPoint> &seamPos, bool isShow) const
{
	//从累计能量中得到最优化seam位置
	int w = accuMap.width();
	int h = accuMap.height();

	const IplImage *pMap = accuMap.image();
	BwImageInt a_map(pMap);
	int startx = 0;
	int min_e =  INT_MAX;
	for (int x = 0; x<w; ++x) {
		if (a_map[h-1][x] < min_e) {
			min_e = a_map[h-1][x];
			startx = x;
		}
	}

	seamPos.push_back(cvPoint(startx, h-1));
	for (int y = h - 1; y > 0; y--) {		
		int c_y = y - 1;
		int c_x = startx;
		int c_e = a_map[c_y][c_x];
		if (startx - 1 >= 0 && a_map[c_y][startx - 1] < c_e) {
			c_e = a_map[c_y][startx - 1];
			c_x = startx - 1;
		}
		if (startx + 1 <= w - 1 && a_map[c_y][startx + 1] < c_e) {
			c_e = a_map[c_y][startx + 1];
			c_x = startx + 1;
		}		
		seamPos.push_back(cvPoint(c_x, c_y));
		startx = c_x;
	}
	std::reverse(seamPos.begin(), seamPos.end());
	if (isShow) {
		IplImage *showImage = (IplImage*)cvClone(m_image);
		RgbImage s_img(showImage);
		for (int i = 0; i<seamPos.size(); ++i) {
			int x = seamPos[i].x;
			int y = seamPos[i].y;
			s_img[y][x].r = 255;
			s_img[y][x].g = 0;
			s_img[y][x].b = 0;
		}
		cvShowImage("one seam", showImage);
		cvReleaseImage(&showImage);
	}
}


void BackwardSeam::calcAccumulateEnergy(bool isShow)
{
	//计算累计能量，只计算一次
	assert(m_energyMap);
	assert(m_accuMap);

	int w = m_energyMap->width;
	int h = m_energyMap->height;

	BwImage e_map(m_energyMap);
	BwImageInt a_map(m_accuMap);
	for (int x = 0; x<w; ++x) {
		a_map[0][x] = e_map[0][x];
	}
	for (int y = 1; y<h; ++y) {
		//中间
		for (int x = 0; x<w; ++x) {
			a_map[y][x] = a_map[y-1][x] + e_map[y][x];
		}
		//左边
		for (int x = 1; x<w; ++x) {
			if (a_map[y-1][x-1] + e_map[y][x] < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x-1] + e_map[y][x];
			}
		}
		//右边
		for (int x = 0; x<w-1; ++x) {
			if (a_map[y-1][x+1] + e_map[y][x] < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x+1] + e_map[y][x];
			}
		}
	}

	if (isShow) {
		CvxEMap::showMap(m_accuMap, "accumulate energy");
	}

}
void BackwardSeam::reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &energyMap, 
							bool isShow)
{
	//不加速版本	
	int w = accuMap.width();
	int h = accuMap.height();

	const IplImage *pAccu = accuMap.image();
	const IplImage *pEnergy = energyMap.image();	

	BwImage e_map(pEnergy);
	BwImageInt a_map(pAccu);

	for (int y = 1; y<h; ++y) {
		//中间
		for (int x = 0; x<w; ++x) {
			a_map[y][x] = a_map[y-1][x] + e_map[y][x];
		}
		//左边
		for (int x = 1; x<w; ++x) {
			if (a_map[y-1][x-1] + e_map[y][x] < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x-1] + e_map[y][x];
			}
		}
		//右边
		for (int x = 0; x<w-1; ++x) {
			if (a_map[y-1][x+1] + e_map[y][x] < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x+1] + e_map[y][x];
			}
		}
	}

	if (isShow) {
		CvxEMap::showMap(pAccu, "re accumulate energy");
	}

}
void BackwardSeam::reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &energyMap, 
							const vector<CvPoint> &seamPos, bool isShow)
{
	//重新计算缝隙边缘的累计能量图
	int w = accuMap.width();
	int h = accuMap.height();

	const IplImage *pAccu = accuMap.image();
	const IplImage *pEnergy = energyMap.image();	

	BwImage e_map(pEnergy);
	BwImageInt a_map(pAccu);

	for (int x = 0; x<w; ++x) {
		a_map[0][x] = e_map[0][x];
	}
	int startx = seamPos[0].x - 1;
	int endx = seamPos[0].x + 1;
	for (int y = 1; y<h; ++y) {
		startx = max(0, startx-1);
		endx = min(w, endx+1);

		int a_sx = a_map[y][startx];
		int a_ex = a_map[y][endx-1];
		//中间
		for (int x = startx; x<endx; ++x) {
			a_map[y][x] = a_map[y-1][x] + e_map[y][x];
		}
		//左边
		for (int x = startx + 1; x<endx; ++x) {
			if (a_map[y-1][x-1] + e_map[y][x] < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x-1] + e_map[y][x];
			}
		}
		//右边
		for (int x = startx; x<endx-1; ++x) {
			if (a_map[y-1][x+1] + e_map[y][x] < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x+1] + e_map[y][x];
			}
		}
		if (a_map[y][startx] == a_sx) {
			startx++;
		}
		if (a_map[y][endx-1] == a_ex) {
			endx--;
		}
	}
	
	if (isShow) {
		CvxEMap::showMap(pAccu, "re accumulate energy");
	}
}