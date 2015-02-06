#ifndef FORE_MAP_H
#define FORE_MAP_H 1

//ForeMap ��ͼ����������ֱ��ͼ������ȷ����saliencyMap (0--255)
#include "cvx_image\cvx_image.h"
#include "integral_hist.h"
#include <vector>

class ForeMap
{
public:
	ForeMap();
	~ForeMap();

	
	void set(IplImage *image, float ratio, int binNum);
	void computerCornerBin(void);
	void computerMap(const std::vector<float> &scale, bool isShow);	
	void expComputerMap(const std::vector<float> &scale, bool isShow);
	void saveMap(char *fileName, bool isShow);
	void copyForeMap(IplImage *image);

private:
	IplImage *m_orgImage;
	IplImage *m_labImage;               
	IntegralHist *m_hist;               //��ǰm_labImage�Ļ���ֱ��ͼ
	IplImage *m_foregroundProbMap;     //��� 
	float m_ratio;         //�߿�ռ�ı���	
	int m_binNum;
	std::vector<float> m_scale;    //ֱ��ͼ���ű���
	int *m_cornerBin[4];           //Ԥ�ȼ�����ĸ������ֱ��ͼ


private:

	
	static void expandImage(IplImage *image, int ew, int eh, IplImage *expImage);
	
	void setBdyZero(IplImage *image, int b_w, int b_h);
	void computerOneScale(float scale, IplImage *fProbMap);
};

#endif