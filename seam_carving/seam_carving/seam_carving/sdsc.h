#ifndef SDSC_H
#define SDSC_H 1
//实现saliency detection for content-aware image resizing ICIP2009

#include "cvx_image\cvx_image.h"
#include "offsetMap.h"
#include <vector>

using std::vector;
using cvx_space::LabImage;

class SDSC
{
public:
	SDSC();
	~SDSC();

	void loadImage(IplImage *image);
	void shrink(int num, bool speedup, bool isShow = false);
	void showSeamPos(void)const;


private:
	IplImage *m_image;         //原图
	IplImage *m_labImage;      //原图转换到lab空间
	IplImage *m_salencyMap;    //视觉显著性图
	IplImage *m_accuMap;     //累计能量图
	IplImage *m_dirMap;      //记录seam方向
	IplImage *m_indexMap;    //记录seam 位置
	vector<vector<CvPoint>> m_seamPos;

private:
	void calcAccumulateEnergyDirMap(bool isShow = false);

	void selectSeam(OffsetMap &accuMap, OffsetMap &dirMap, vector<CvPoint> &seamPos, bool isShow) const;
	
	void reCalcAccumulateEnergy(OffsetMap &accuMap, OffsetMap &labMap,
								OffsetMap &salMap,	OffsetMap &dirMap,	bool isShow) const;
	
	void reCalcAccumulateEnergySP(OffsetMap &accuMap, OffsetMap &labMap,
		OffsetMap &salMap,	OffsetMap &dirMap,	vector<CvPoint> &seamPos, bool isShow)const;

	void calcSeamPos(const IplImage *idImage, const vector<CvPoint> &seamPos);
	inline int distance(LabImage &image, int y1, int x1, int y2, int x2) const
	{
		unsigned char *pd1 = (unsigned char*)&image[y1][x1];
		unsigned char *pd2 = (unsigned char*)&image[y2][x2];
		int dif = (pd2[0] - pd1[0]) * (pd2[0] - pd1[0]) +
				  (pd2[1] - pd1[1]) * (pd2[1] - pd1[1]) +
				  (pd2[2] - pd1[2]) * (pd2[2] - pd1[2]);
	//	return sqrt(double(dif));	
		return abs(pd2[0] - pd1[0]) + abs(pd2[1] - pd1[1]) + abs(pd2[2] - pd1[2]); //speed up
	}
};

#endif