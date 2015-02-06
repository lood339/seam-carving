#include "shiftmap_resize.h"
#include "image_template\cvx_emap.h"
#include "cvx_image\cvx_image.h"
#include "image_template\cvx_sf.h"
#include "graph_cut\energy_include.h"

using cvx_space::BwImage;
using cvx_space::RgbImage;
using cvx_space::BwImageInt;


ShiftMapResize::ShiftMapResize()
{
	m_image = NULL;
	m_dataItem = NULL;
	m_gradient = NULL;
	m_shiftMap = NULL;
	m_rstImage = NULL;
	m_maxShift = 0;
	m_energy = INT_MAX;
	m_weight = 0.05;
}
ShiftMapResize::~ShiftMapResize()
{

}

void ShiftMapResize::loadImage(IplImage *image)
{
	if (image && image->nChannels == 3) {
		m_image = (IplImage*)cvClone(image);
		m_dataItem = cvCreateImage(cvGetSize(m_image), IPL_DEPTH_8U, 1);
		cvZero(m_dataItem);
		m_gradient = cvCreateImage(cvGetSize(m_image), IPL_DEPTH_8U, 1);
		cvZero(m_gradient);
	}
}
void ShiftMapResize::dataItem(bool isShow)
{
	//计算data项
	CvxEMap::sobel(m_image, m_gradient);
	int w = m_image->width;
	int h = m_image->height;

	BwImage g_map(m_gradient);
	BwImage d_map(m_dataItem);
	for (int y = 0; y<h; ++y) {
		for (int x = 0 ; x<w; ++x) {
			d_map[y][x] = 255 - g_map[y][x];
		}
	}
	if (isShow) {
		cvShowImage("gradient", m_gradient);
	}
}
void ShiftMapResize::setNewWidth(int width)
{
	//得到目标尺寸
	if (m_image && width < m_image->width) {
		int h = m_image->height;
		m_shiftMap = cvCreateImage(cvSize(width, h), IPL_DEPTH_32S, 1);
		m_rstImage = cvCreateImage(cvSize(width, h), IPL_DEPTH_8U, 3);
	}
}
void ShiftMapResize::setWeight(float w)
{
	m_weight = w;
}
void ShiftMapResize::proposeShiftMap(bool isShow)
{
	//初始化shift map
	if (m_shiftMap) {
		m_maxShift = m_image->width - m_shiftMap->width;
		int w = m_shiftMap->width;
		int h = m_shiftMap->height;
		BwImageInt s_map(m_shiftMap);
		for (int y = 0; y<h; ++y){
			for (int x = 0; x<w; ++x){
				int delta_x = 1.0*x/w* m_maxShift;
				s_map[y][x] = delta_x;
				if ( x >= w - 5){
					s_map[y][x] = m_maxShift;				
				}
			}
		}
		if (isShow) {
			CvxSF::showImage(m_shiftMap, "initial shift map");
		}
	}
}

void ShiftMapResize::optmizeShiftMap(int iterater)
{
	//进行alpha expantion
	if (iterater > 0 && m_shiftMap) {
		for (int j = 0; j<iterater; ++j) {
			alphaExp(0);
			alphaExp(m_maxShift);
			for (int i = 1; i<m_maxShift; ++i) {
				alphaExp(i);
			}
		}
	}
}
void ShiftMapResize::getResult(bool isShow)
{
	RgbImage o_map(m_image);
	RgbImage r_map(m_rstImage);
	BwImageInt s_map(m_shiftMap);

	int w = m_shiftMap->width;
	int h = m_shiftMap->height;
	for (int y = 0; y<h; ++y) {
		for (int x = 0; x<w; ++x) {
			int r_x = x + s_map[y][x];
			r_map[y][x].r = o_map[y][r_x].r;
			r_map[y][x].g = o_map[y][r_x].g;
			r_map[y][x].b = o_map[y][r_x].b;
		}
	}
	if (isShow) {
		cvShowImage("result", m_rstImage);
		CvxSF::showImage(m_shiftMap, "shift map");
	}



}
void ShiftMapResize::alphaExp(int alpha)
{
	int w = m_shiftMap->width;
	int h = m_shiftMap->height;	

	bool hasAlpha = false;
	//是否含有alpha
	BwImageInt s_map(m_shiftMap);
	for (int y = 0; y<h; ++y) {
		for(int x = 0; x<w; ++x) {
			if (s_map[y][x] == alpha) {
				hasAlpha = true;
				break;
			}
		}
		if (hasAlpha) {
			break;
		}
	}
	if (!hasAlpha) {
		return;
	}
	IplImage *t_idImage = cvCreateImage(cvSize(w, h), IPL_DEPTH_32S, 1);
	BwImageInt id_map(t_idImage);
	for (int y = 0; y<h; ++y) {
		for (int x = 0; x<w; ++x) {
			id_map[y][x] = y * (w-2) + x - 1;
		}
	}

	//边界的像素保持不变
	int numV = (w-2)*h;
	EnergyInt *t_e = new EnergyInt(numV, numV*4);
	EnergyInt::Var *t_v = new EnergyInt::Var[numV];

	for (int i = 0; i<numV; ++i) {
		t_v[i] = t_e->add_variable();
	}

	RgbImage o_img(m_image);
	
	BwImage d_map(m_dataItem);
	BwImage g_map(m_gradient);
	//data item
	for (int y = 0; y<h; ++y) {
		for (int x = 1; x<w-1; ++x) {
			int id = id_map[y][x];
			int E0 = d_map[y][x+s_map[y][x]];
			int E1 = d_map[y][x+alpha];
			if (id == alpha) {
				E0 = INT_MAX/2;
				E1 = 0;
			}
			t_e->add_term1(t_v[id], E0, E1);
		}
	}

	//smooth item
	for (int y = 0; y<h; ++y) {
		for (int x = 1; x<w-1; ++x) {
			//从左到右
			if (x != w-2) {
				int ls = s_map[y][x];  //left shift map
				int rs = s_map[y][x+1];
				int E00 = 0;
				int E01 = 0;
				int E10 = 0;
				int E11 = 0;

				if (ls != rs) {
					E00 = distance(o_img, g_map, y, x+ls+1, y, x+1+rs)
						+ distance(o_img, g_map, y, x+1+rs-1, y, x+ls);
				}
				if (ls != alpha && ls < alpha) {
					E01 = distance(o_img, g_map, y, x+ls+1, y, x+1+alpha);
					    + distance(o_img, g_map, y, x+1+alpha-1, y, x+ls);
				}
				else if (ls > alpha) {
					E01 = INT_MAX/2;
				}
				if (rs != alpha && rs > alpha) {
					E10 = distance(o_img, g_map, y, x+alpha+1, y, x+1+rs)
						+ distance(o_img, g_map, y, x+1+rs-1, y, x+alpha);
				}
				else if (rs < alpha) {
					E10 = INT_MAX/2;
				}
				t_e->add_term2(t_v[id_map[y][x]], t_v[id_map[y][x+1]], 
								E00, E01, E10, E11);

			}
			//从上到下
			if (y != h-1) {
				int ts = s_map[y][x];
				int ds = s_map[y+1][x];
				int E00 = 0;
				int E01 = 0;
				int E10 = 0;
				int E11 = 0;
				if (ts != ds) {
					E00 = distance(o_img, g_map, y+1, x+ts, y+1, x+ds)
						+ distance(o_img, g_map, y, x+ds, y, x+ts);
				}
				if (ts != alpha) {
					E01 = distance(o_img, g_map, y+1, x+ts, y+1, x+alpha)
						+ distance(o_img, g_map, y, x+alpha, y, x+ts);

				}
				if (ds != alpha) {
					E10 = distance(o_img, g_map, y+1, x+alpha, y+1, x+ds)
						+ distance(o_img, g_map, y, x+ ds, y, x+alpha);
				}
				t_e->add_term2(t_v[id_map[y][x]], t_v[id_map[y+1][x]],
					           E00, E01, E10, E11);
			}
		}
	}

	int energy = t_e->minimize();
	printf("energy = %d\n", energy);


	if (energy < m_energy) {
		m_energy = energy;
		for (int y = 0; y<h; ++y) {
			for (int x = 1; x<w-1; ++x) {
				if (t_e->get_var(t_v[id_map[y][x]]) == GraphInt::SINK) {
					if (s_map[y][x] != alpha) {
						s_map[y][x] = alpha;
					}
				}
			}
		}
	}
	
	cvReleaseImage(&t_idImage);
	delete t_e;
	delete []t_v;
}

