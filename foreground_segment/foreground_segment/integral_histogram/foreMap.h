#ifndef FORE_MAP_H
#define FORE_MAP_H 1

//ForeMap 有图像上下左右直方图相似性确定的saliencyMap (0--255)
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
	IntegralHist *m_hist;               //当前m_labImage的积分直方图
	IplImage *m_foregroundProbMap;     //结果 
	float m_ratio;         //边框占的比率	
	int m_binNum;
	std::vector<float> m_scale;    //直方图缩放比例
	int *m_cornerBin[4];           //预先计算的四个角落的直方图


private:

	
	static void expandImage(IplImage *image, int ew, int eh, IplImage *expImage);
	
	void setBdyZero(IplImage *image, int b_w, int b_h);
	void computerOneScale(float scale, IplImage *fProbMap);
};

#endif