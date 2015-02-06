#include "offsetMap.h"
#include <iostream>

using cvx_space::BwImage;
using cvx_space::RgbImage;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

OffsetMap::OffsetMap(IplImage *image)
{
	assert(image);
	m_image = (IplImage*)cvClone(image);
	m_rWidth = m_image->width;
	switch(m_image->depth)
	{
	case IPL_DEPTH_8U:
		m_depthByte = 1;
		break;
	case IPL_DEPTH_32S:
		m_depthByte = 4;
		break;
	default:
		printf("OffsetMap depth byte not defined\n");
		exit(-1);
	}
}
OffsetMap::~OffsetMap()
{
	if (m_image) {
		cvReleaseImage(&m_image);
	}
}
bool OffsetMap::offsetLeft(const vector<CvPoint> &seamPos)
{
	if (seamPos.size() != m_image->height) {
		printf("error: offsetLeft error 1\n");
		return false;
	}	

	for (int i = 0; i<seamPos.size(); ++i) {
		int x = seamPos[i].x;
		int y = seamPos[i].y;
		int num_copy = m_rWidth - x - 1;
		if (x < 0) {
			cerr<<"x = "<<x<<endl;
			cerr<<"offsetLeft error 1"<<endl;
			continue;
		}
		if (x >= m_rWidth) {
			cerr<<"x = "<<x<<endl;
			cerr<<"rWidth = "<<m_rWidth<<endl;
			cerr<<"offsetLeft error 2"<<endl;
			return false;
		}
		unsigned char *pSeam = (unsigned char *)(m_image->imageData + 
			m_image->widthStep * y + 
			m_image->nChannels * x * m_depthByte);
		unsigned char *pRight = pSeam + m_image->nChannels * m_depthByte;
		memmove(pSeam, pRight, num_copy * m_image->nChannels * m_depthByte);			
	}
	m_rWidth--;
	//test
	/*if (m_image->nChannels == 3 && m_image->depth == IPL_DEPTH_8U) {
	RgbImage img(m_image);
	for (int y = 0; y<m_image->height; ++y) {
	img[y][m_rWidth].r = 255;
	img[y][m_rWidth].g = 0;
	img[y][m_rWidth].b = 0;
	}
	}
	cvShowImage("offsetLeft", m_image);*/
}
void OffsetMap::getARandSeam(vector<CvPoint> &seamPos) const
{
	int width = m_rWidth;
	int height = m_image->height;
	int startX = rand()%width;
	for (int y = 0; y<height; ++y) {
		seamPos.push_back(cvPoint(startX, y));
		int x_offset = rand()%3 - 1;
		if (startX + x_offset < 0) {
			x_offset = 0;
		}
		if (startX + x_offset >= m_rWidth) {
			x_offset = 0;
		}
		startX += x_offset;
	}		
}
void OffsetMap::markSeam(const vector<CvPoint> &seamPos)
{
	if (m_image->nChannels == 3 && m_image->depth == IPL_DEPTH_8U) {
		RgbImage img(m_image);
		for (int i = 0; i<seamPos.size(); ++i) {
			int x = seamPos[i].x;
			int y = seamPos[i].y;
			img[y][x].r = 255;
			img[y][x].g = 0;
			img[y][x].b = 0;
		}
		cvShowImage("markSeam", m_image);
	}

}