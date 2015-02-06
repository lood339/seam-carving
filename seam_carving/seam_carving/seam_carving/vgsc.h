#ifndef VGSC_H
#define VGSC_H 1
//Variation in the Gradient of intensity
//ʵ��discontinuous seam-carving for video retargeting�е�ͼ�񲿷�
//Ϊ���٣�����discontinuous����
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
	IplImage *m_image;         //ԭͼ
	IplImage *m_gray;         //�Ҷ�ͼ
	IplImage *m_accuMap;     //�ۼ�����ͼ
	IplImage *m_dirMap;      //��¼seam����
	IplImage *m_indexMap;    //��¼seam λ��
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