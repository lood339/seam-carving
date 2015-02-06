#ifndef BALANCED_VGSC_H
#define BALANCED_VGSC_H 1
//balance energy map + variation of gradient of intensity
#include "cvx_image\cvx_image.h"
#include "offsetMap.h"
#include <vector>

using std::vector;
using cvx_space::LabImage;

class BalancedVGSC
{
public:
	BalancedVGSC();
	~BalancedVGSC();

	void loadImage(IplImage *image);
	void shrink(int num, bool speedup, bool isShow = false);
	void showSeamPos(void)const;


private:
	IplImage *m_image;         //原图
	IplImage *m_grayImage;     //灰度图
	IplImage *m_balanceMap;    //平衡能量图	
	IplImage *m_accuMap;     //累计能量图
	IplImage *m_dirMap;      //记录seam方向
	IplImage *m_indexMap;    //记录seam 位置
	vector<vector<CvPoint>> m_seamPos;

private:
	void calcAccumulateEnergyDirMap(bool isShow = false);
	
	void selectSeam(OffsetMap &accuMap, OffsetMap &dirMap, vector<CvPoint> &seamPos, bool isShow) const;

	void reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &grayMap,
								OffsetMap &bMap, OffsetMap &dirMap,	bool isShow) const;

	void reCalcAccumulateEnergySP(OffsetMap &accuMap, OffsetMap &grayMap,
								  OffsetMap &bMap, OffsetMap &dirMap, 
								  vector<CvPoint> &seamPos, bool isShow)const;
	
	void calcSeamPos(const IplImage *idImage, const vector<CvPoint> &seamPos);
	
};

#endif