#ifndef VGSC_H
#define VGSC_H 1
//Variation in the Gradient of intensity
//实现discontinuous seam-carving for video retargeting中的图像部分
//为加速，不做discontinuous部分
#include "cvx_image\cvx_image.h"
#include "offsetMap.h"
#include <vector>

using std::vector;

class VGSC
{
public:
	VGSC();
	~VGSC();

	void loadImage(IplImage *image);
	void shrink(int num, bool speedup, bool isShow = false);
	void showSeamPos(void)const;


private:
	IplImage *m_image;         //原图
	IplImage *m_gray;         //灰度图
	IplImage *m_accuMap;     //累计能量图
	IplImage *m_dirMap;      //记录seam方向
	IplImage *m_indexMap;    //记录seam 位置
	vector<vector<CvPoint>> m_seamPos;

private:
	void calcAccumulateEnergyDirMap(bool isShow = false);

	void selectSeam(OffsetMap &accuMap, OffsetMap &dirMap, vector<CvPoint> &seamPos, bool isShow) const;
	void reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &grayMap, 
		OffsetMap &dirMap,	bool isShow) const;
	void reCalcAccumulateEnergySP(OffsetMap &accuMap, OffsetMap &grayMap, 
		OffsetMap &dirMap,	vector<CvPoint> &seamPos, bool isShow)const;

	void calcSeamPos(const IplImage *idImage, const vector<CvPoint> &seamPos);
};


#endif