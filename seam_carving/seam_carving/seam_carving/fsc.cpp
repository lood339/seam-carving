#include <iostream>
#include "fsc.h"
#include "image_template\cvx_emap.h"


using cvx_space::BwImage;
using cvx_space::RgbImage;
using cvx_space::BwImageInt;
using std::cout;
using std::cerr;
using std::endl;

FSC::FSC()
{
	m_image = NULL;	
	m_accuMap = NULL;
	m_gray = NULL;
	m_dirMap = NULL;
	m_indexMap = NULL;
}
FSC::~FSC()
{

}

void FSC::loadImage(IplImage *image)
{
	assert(image);
	m_image = (IplImage *)cvClone(image);
	m_gray = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCvtColor(m_image, m_gray, CV_BGR2GRAY);
	m_accuMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_32S, 1);	
	cvZero(m_accuMap);
	m_indexMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_32S, 1);	

	BwImageInt id_map(m_indexMap);
	for (int y = 0; y<image->height; ++y) {
		for (int x = 0; x<image->width; ++x) {
			id_map[y][x] = x;
		}
	}
	m_dirMap = cvCreateImage(cvGetSize(image), IPL_DEPTH_32S, 1);
	cvZero(m_dirMap);
}


void FSC::shrink(int num, bool speedup, bool isShow)
{
	assert(m_image);
	int w = m_image->width;
	int h = m_image->height;
	if (num < 0 || num >= w) {
		return;
	}

	//计算累计能量图
	calcAccumulateEnergyDirMap(false);

	OffsetMap grayMap(m_gray);
	OffsetMap aMap(m_accuMap);
	OffsetMap oMap(m_image);
	OffsetMap dirMap(m_dirMap);
	OffsetMap idMap(m_indexMap);

	for (int i = 0; i<num; ++i) {
		vector<CvPoint> seamPos;
		//选择一条seam
		selectSeam(aMap, dirMap, seamPos, false);		
		//改变梯度图
		grayMap.offsetLeft(seamPos);
		//改变累计能量图
		aMap.offsetLeft(seamPos);		
		//改变方向
		dirMap.offsetLeft(seamPos);
		//重新计算dirMap
		if (speedup) {
			reCalcAccumulateEnergySP(aMap, grayMap, dirMap, seamPos, false);
		}
		else {
			reCalcAccumulateEnergy(aMap, grayMap, dirMap, false);
		}		

		if (isShow) {
			//改变原图
			oMap.offsetLeft(seamPos);
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
		cvSaveImage("fsc.png", resImage);
	}
}




void FSC::selectSeam(OffsetMap &accuMap, OffsetMap &dirMap, vector<CvPoint> &seamPos, bool isShow) const
{
	//从累计能量中得到最优化seam位置
	int w = accuMap.width();
	int h = accuMap.height();

	const IplImage *pMap = accuMap.image();
	const IplImage *pDir = dirMap.image();
	BwImageInt a_map(pMap);
	BwImageInt dir_map(pDir);
	int startx = 0;
	int min_e =  INT_MAX;
	for (int x = 0; x<w; ++x) {
		if (a_map[h-1][x] < min_e) {
			min_e = a_map[h-1][x];
			startx = x;
		}
	}
	//最后一行
	seamPos.push_back(cvPoint(startx, h-1));
	//从倒数第二行，向第一行回溯
	for (int y = h - 1; y > 0; y--) {		
		int c_y = y - 1;
		int c_x = startx + dir_map[y][startx];		
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
		cvShowImage("fsc one seam", showImage);
		cvReleaseImage(&showImage);
	}
}

void FSC::calcAccumulateEnergyDirMap(bool isShow)
{
	//计算累计能量，只计算一次
	assert(m_gray);
	assert(m_dirMap);
	assert(m_accuMap);

	int w = m_gray->width;
	int h = m_gray->height;

	BwImage gray_map(m_gray);
	BwImageInt dir_map(m_dirMap);
	BwImageInt a_map(m_accuMap);

	//第一行
	a_map[0][0] = abs(gray_map[0][0] - gray_map[0][1]);
	dir_map[0][0] = 0;
	a_map[0][w-1] = abs(gray_map[0][w-2] - gray_map[0][w-1]);
	dir_map[0][w-1] = 0;
	for (int x = 1; x<w-1; ++x) {
		a_map[0][x] = abs(gray_map[0][x+1] - gray_map[0][x-1]);
		dir_map[0][x] = 0;
	}
	for (int y = 1; y<h; ++y) {
		//x = 0
		a_map[y][0] = a_map[y-1][0] + 3 * abs(gray_map[y][0] - gray_map[y][1]);
		dir_map[y][0] = 0;		
		//x = w -1;
		a_map[y][w-1] = a_map[y-1][w-1] + 3 * abs(gray_map[y][w-1] - gray_map[y][w-2]);
		dir_map[y][w-1] = 0;	

		for (int x = 1; x<w-1; ++x) {
			int A = gray_map[y-1][x-1];
			int B = gray_map[y-1][x];
			int C = gray_map[y-1][x+1];
			int D = gray_map[y][x-1];
			int E = gray_map[y][x];
			int F = gray_map[y][x+1];
			//中间
			a_map[y][x] = a_map[y-1][x] + abs(D - F);
			dir_map[y][x] = 0;

			//左边
			if (a_map[y-1][x-1] + abs(D - F) + abs(B - D) < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x-1] + abs(D - F) + abs(B - D);
				dir_map[y][x] = -1;
			}
			//右边
			if (a_map[y-1][x+1] + abs(D - F) + abs(B - F) < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x+1] + abs(D - F) + abs(B - F);
				dir_map[y][x] = 1;
			}
		}
					
	}

	if (isShow) {
		CvxEMap::showMap(m_accuMap, "accumulate energy");
	}

}

void FSC::reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &grayMap, 
								 OffsetMap &dirMap,	bool isShow)const
{
	//重新计算能量和方向，不加速版本	
	int w = accuMap.width();
	int h = accuMap.height();

	const IplImage *pGray = grayMap.image();
	const IplImage *pDir = dirMap.image();
	const IplImage *pAccu = accuMap.image();

	BwImage gray_map(pGray);
	BwImageInt dir_map(pDir);
	BwImageInt a_map(pAccu);

	//第一行
	a_map[0][0] = abs(gray_map[0][0] - gray_map[0][1]);
	dir_map[0][0] = 0;
	a_map[0][w-1] = abs(gray_map[0][w-2] - gray_map[0][w-1]);
	dir_map[0][w-1] = 0;
	for (int x = 1; x<w-1; ++x) {
		a_map[0][x] = abs(gray_map[0][x+1] - gray_map[0][x-1]);
		dir_map[0][x] = 0;
	}
	for (int y = 1; y<h; ++y) {
		//x = 0
		a_map[y][0] = a_map[y-1][0] + 3 * abs(gray_map[y][0] - gray_map[y][1]);
		dir_map[y][0] = 0;		
		//x = w -1;
		a_map[y][w-1] = a_map[y-1][w-1] + 3 * abs(gray_map[y][w-1] - gray_map[y][w-2]);
		dir_map[y][w-1] = 0;	

		for (int x = 1; x<w-1; ++x) {
			int A = gray_map[y-1][x-1];
			int B = gray_map[y-1][x];
			int C = gray_map[y-1][x+1];
			int D = gray_map[y][x-1];
			int E = gray_map[y][x];
			int F = gray_map[y][x+1];
			//中间
			a_map[y][x] = a_map[y-1][x] + abs(D - F);
			dir_map[y][x] = 0;

			//左边
			if (a_map[y-1][x-1] + abs(D - F) + abs(B - D) < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x-1] + abs(D - F) + abs(B - D);
				dir_map[y][x] = -1;
			}
			//右边
			if (a_map[y-1][x+1] + abs(D - F) + abs(B - F) < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x+1] + abs(D - F) + abs(B - F);
				dir_map[y][x] = 1;
			}
		}
	}
	if (isShow) {
		CvxEMap::showMap(pAccu, "re accumulate energy");
	}
}


void FSC::reCalcAccumulateEnergySP(OffsetMap &accuMap, OffsetMap &grayMap, 
								   OffsetMap &dirMap, vector<CvPoint> &seamPos, bool isShow)const
{
	//重新计算缝隙边缘的累计能量图
	int w = accuMap.width();
	int h = accuMap.height();

	const IplImage *pGray = grayMap.image();	
	const IplImage *pAccu = accuMap.image();	
	const IplImage *pDir = dirMap.image();

	BwImage gray_map(pGray);
	BwImageInt a_map(pAccu);
	BwImageInt dir_map(pDir);

	//第一行也有变化
	a_map[0][0] = abs(gray_map[0][0] - gray_map[0][1]);
	dir_map[0][0] = 0;
	a_map[0][w-1] = abs(gray_map[0][w-2] - gray_map[0][w-1]);
	dir_map[0][w-1] = 0;
	for (int x = 1; x<w-1; ++x) {
		a_map[0][x] = abs(gray_map[0][x+1] - gray_map[0][x-1]);
		dir_map[0][x] = 0;
	}
	
	//从第二行开始
	int startx = seamPos[0].x - 2;
	int endx = seamPos[0].x + 2;
	for (int y = 1; y<h; ++y) {
		startx = max(0, startx-2);
		endx = min(w, endx+2);

		int a_sx = a_map[y][startx];
		int a_ex = a_map[y][endx-1];
		int b_sx = a_map[y][startx+1];
		int b_ex = a_map[y][endx-2];

		if (startx == 0) {
			a_map[y][0] = a_map[y-1][0] + 3 * abs(gray_map[y][0] - gray_map[y][1]);
			dir_map[y][0] = 0;
		}
		if (endx == w) {
			a_map[y][w-1] = a_map[y-1][w-1] + 3 * abs(gray_map[y][w-1] - gray_map[y][w-2]);
			dir_map[y][w-1] = 0;
		}
		for (int x = startx+1; x<endx-1; ++x) {
			int A = gray_map[y-1][x-1];
			int B = gray_map[y-1][x];
			int C = gray_map[y-1][x+1];
			int D = gray_map[y][x-1];
			int E = gray_map[y][x];
			int F = gray_map[y][x+1];
			//中间
			a_map[y][x] = a_map[y-1][x] + abs(D - F);
			dir_map[y][x] = 0;

			//左边
			if (a_map[y-1][x-1] + abs(D - F) + abs(B - D) < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x-1] + abs(D - F) + abs(B - D);
				dir_map[y][x] = -1;
			}
			//右边
			if (a_map[y-1][x+1] + abs(D - F) + abs(B - F) < a_map[y][x]) {
				a_map[y][x] = a_map[y-1][x+1] + abs(D - F) + abs(B - F);
				dir_map[y][x] = 1;
			}
		}
		if (a_map[y][startx] == a_sx && startx != 0) {			
			if (a_map[y][startx+1] == b_sx) {
				startx++;
			}
			startx++;
		}
		if (a_map[y][endx-1] == a_ex && endx != w) {
			if (a_map[y][endx-2] == b_ex) {
				endx--;
			}
			endx--;
		}		
	}

	if (isShow) {
		CvxEMap::showMap(pAccu, "re accumulate energy");
	}
}


void FSC::showSeamPos(void)const
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
	cvReleaseImage(&seamImage);
}
void FSC::calcSeamPos(const IplImage *idImage, const vector<CvPoint> &seamPos)
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
