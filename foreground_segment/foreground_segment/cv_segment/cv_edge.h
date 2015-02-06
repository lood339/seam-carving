#ifndef CV_EDGE_H
#define CV_EDGE_H 1

typedef struct _cv_edge{
	float w;    //a,b��Rgb���Խ��wԽ��
	int a, b;   //a,bΪ����pixel��ͼ���е�index
} cv_edge;

bool operator<(const cv_edge &a, const cv_edge &b) {
	return a.w < b.w;
}

template <class T>
inline T square(const T &x) { return x*x; };

#endif