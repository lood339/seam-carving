#ifndef CV_EDGE_H
#define CV_EDGE_H 1

typedef struct _cv_edge{
	float w;    //a,b的Rgb相差越大，w越大
	int a, b;   //a,b为相邻pixel在图像中的index
} cv_edge;

bool operator<(const cv_edge &a, const cv_edge &b) {
	return a.w < b.w;
}

template <class T>
inline T square(const T &x) { return x*x; };

#endif