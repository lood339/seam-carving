#ifndef SHIFT_MAP_RESIZE_H
#define SHIFT_MAP_RESIZE_H 1

//实现shift map resizing部分，data项smooth项的权重需要手动调
#include "cvx_image\cvx_image.h"

using cvx_space::RgbImage;
using cvx_space::BwImage;

class  ShiftMapResize
{
public:
	ShiftMapResize();
	~ShiftMapResize();

	void loadImage(IplImage *image);
	void dataItem(bool isShow);
	void setNewWidth(int width);
	void setWeight(float w);
	void proposeShiftMap(bool isShow);
	void optmizeShiftMap(int iterater);
	void getResult(bool isShow);
	


private:
	IplImage *m_image;
	IplImage *m_dataItem;
	IplImage *m_gradient;
	IplImage *m_shiftMap;
	IplImage *m_rstImage;
	int m_maxShift;
	int m_energy;
	float m_weight;

private:
	void alphaExp(int alpha);
	inline int distance(RgbImage &oMap, BwImage &gMap, 
						 int y1, int x1, int y2, int x2)
	{
		float belta = 2.0;
		int r_dif = oMap[y1][x1].r - oMap[y2][x2].r;
		int g_dif = oMap[y1][x1].g - oMap[y2][x2].g;
		int b_dif = oMap[y1][x1].b - oMap[y2][x2].b;
		int gra_dif = gMap[y1][x1] - gMap[y2][x2];
		int rst = r_dif * r_dif + g_dif * g_dif + b_dif * b_dif + belta * gra_dif * gra_dif;
		return m_weight * rst;
	}
	

};

#endif