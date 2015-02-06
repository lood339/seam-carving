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
	IplImage *m_image;         //ԭͼ
	IplImage *m_grayImage;     //�Ҷ�ͼ
	IplImage *m_balanceMap;    //ƽ������ͼ	
	IplImage *m_accuMap;     //�ۼ�����ͼ
	IplImage *m_dirMap;      //��¼seam����
	IplImage *m_indexMap;    //��¼seam λ��
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