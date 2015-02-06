#ifndef BACKWARD_SEAM_H
#define BACKWARD_SEAM_H 1
//seam carving for image resizing siggraph2007 论文实现，可以看seam 位置

#include "cvx_image\cvx_image.h"
#include "offsetMap.h"
#include <vector>

using std::vector;


class BackwardSeam
{
public:
	BackwardSeam();
	~BackwardSeam();

	void loadImage(IplImage *image);
	void shrink(int num, bool nSpeedUp, bool isShow = false);
	void showSeamPos(void);
	

private:
	IplImage *m_image;         //原图
	IplImage *m_energyMap;     //原始能量图
	IplImage *m_accuMap;     //累计能量图
	IplImage *m_indexMap;    //记录seam 位置
	vector<vector<CvPoint>> m_seamPos;

private:
	void calcAccumulateEnergy(bool isShow = false);
	void selectSeam(OffsetMap &accuMap, vector<CvPoint> &seamPos, bool isShow) const;
	void reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &energyMap, 
		 const vector<CvPoint> &seamPos, bool isShow);
	void reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &energyMap, 
		                        bool isShow);
	void calcSeamPos(const IplImage *idImage, const vector<CvPoint> &seamPos);
};
#endif
