#ifndef OFFSET_MAP_H
#define OFFSET_MAP_H 1
#include <vector>
#include "cvx_image\cvx_image.h"


class OffsetMap 
{
public:
	OffsetMap(IplImage *image);		
	~OffsetMap();

	int width(void){return m_rWidth;}
	int height(void){return m_image->height;}
	const IplImage *image(void){return m_image;}

	bool offsetLeft(const std::vector<CvPoint> &seamPos);

	//test
	void getARandSeam(std::vector<CvPoint> &seamPos) const;
	void markSeam(const std::vector<CvPoint> &seamPos);

private:
	IplImage *m_image;
	int m_rWidth;       //图像中实际有效宽度
	int m_depthByte;      //每一channel的字节数
};

#endif