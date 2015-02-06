#ifndef SHRESHOLD_SEG_H
#define SHRESHOLD_SEG_H 1

#include <vector>
#include "cvx_image\cvx_image.h"

using std::vector;

class ThresholdSeg
{
public:
	ThresholdSeg();
	~ThresholdSeg();

	void loadImage(IplImage *image);
	
	void getFgProbMap(float ratio, int binNum, const vector<float> &scale, bool isExp, bool showMap);
	//min_size��ͼ��ߴ��Լ�ƽ���̶Ⱦ���
	void overSegment(float sigma = 0.5, float k = 500, int min_size = 400, bool showSeg = true);
	//threshold:��Ҫ���������ֵ 2.0,
	void foregroundSeg(float threshold, bool isShow);
	void saveFgArea(char *fileName);
private:
	IplImage *m_orgImage;    //ԭͼ
	IplImage *m_fgProbMap;   //ǰ������ͼ
	IplImage *m_segMap;      //���ָ�����ͼ
	IplImage *m_aveCMap;     //��Ҫ��ͼ����������ƽ��
	int m_labelN;            //���ָ��������Ŀ
	IplImage *m_fgImage;     //ǰ������ͼ
};

#endif