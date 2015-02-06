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
	//min_size有图像尺寸以及平滑程度决定
	void overSegment(float sigma = 0.5, float k = 500, int min_size = 400, bool showSeg = true);
	//threshold:重要性区域的阈值 2.0,
	void foregroundSeg(float threshold, bool isShow);
	void saveFgArea(char *fileName);
private:
	IplImage *m_orgImage;    //原图
	IplImage *m_fgProbMap;   //前景概率图
	IplImage *m_segMap;      //过分割区域图
	IplImage *m_aveCMap;     //重要性图，经过区域平均
	int m_labelN;            //过分割区域的数目
	IplImage *m_fgImage;     //前景区域图
};

#endif