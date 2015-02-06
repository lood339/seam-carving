#ifndef BACKWARD_SEAM_H
#define BACKWARD_SEAM_H 1
//seam carving for image resizing siggraph2007 ����ʵ�֣����Կ�seam λ��

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
	IplImage *m_image;         //ԭͼ
	IplImage *m_energyMap;     //ԭʼ����ͼ
	IplImage *m_accuMap;     //�ۼ�����ͼ
	IplImage *m_indexMap;    //��¼seam λ��
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
