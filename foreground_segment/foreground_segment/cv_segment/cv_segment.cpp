#include <stdio.h>
#include <algorithm>
#include <math.h>
#include "disjoint_set.h"
#include "cv_segment.h"
#include "cvx_image\cvx_image.h"
#include "cv_edge.h"
//#include <time.h>

using cvx_space::RgbImageFloat;
using cvx_space::BwImage;
// threshold function
#define THRESHOLD(size, c) (c/size)

bool CvSegment::segment(IplImage *inputImage, IplImage *&outImage, int *labelN,
						float sigma , float k, int min_size)
{
	if (!inputImage || inputImage->nChannels != 3 || inputImage->depth != IPL_DEPTH_8U) {
		fprintf(stderr, "input image should be \n");
		return false;
	}
	if (outImage) {
		fprintf(stderr, "outImage image is generated inside \n");
		return false;
	}

	
	int width = inputImage->width;
	int height = inputImage->height;

	if (width > 1024 || height > 1024) {
		fprintf(stderr, "input image too large\n");
		return false;
	}

	IplImage *floatImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 3);
	cvConvert(inputImage, floatImage);

	sigma = max(sigma, 0.1);
	cvSmooth(floatImage, floatImage, CV_GAUSSIAN, 0, 0, sigma, sigma);

	// build graph
	cv_edge *edges = new cv_edge[width*height*4];
	int num = 0;
	RgbImageFloat f_img(floatImage);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x < width-1) {
				edges[num].a = y * width + x;
				edges[num].b = y * width + (x+1);
				edges[num].w = sqrt(square(f_img[y][x].r - f_img[y][x+1].r) +
					                square(f_img[y][x].g - f_img[y][x+1].g) +
									square(f_img[y][x].b - f_img[y][x+1].b));
				num++;
			}

			if (y < height-1) {
				edges[num].a = y * width + x;
				edges[num].b = (y+1) * width + x;
				edges[num].w = sqrt(square(f_img[y][x].r - f_img[y+1][x].r) +
									square(f_img[y][x].g - f_img[y+1][x].g) +
									square(f_img[y][x].b - f_img[y+1][x].b));
				num++;
			}

			if ((x < width-1) && (y < height-1)) {
				edges[num].a = y * width + x;
				edges[num].b = (y+1) * width + (x+1);
				edges[num].w = sqrt(square(f_img[y][x].r - f_img[y+1][x+1].r) +
									square(f_img[y][x].g - f_img[y+1][x+1].g) +
									square(f_img[y][x].b - f_img[y+1][x+1].b));
				num++;
			}

			if ((x < width-1) && (y > 0)) {
				edges[num].a = y * width + x;
				edges[num].b = (y-1) * width + (x+1);
				edges[num].w = sqrt(square(f_img[y][x].r - f_img[y-1][x+1].r) +
									square(f_img[y][x].g - f_img[y-1][x+1].g) +
									square(f_img[y][x].b - f_img[y-1][x+1].b));
				num++;
			}
		}
	}
	
	if (floatImage) {
		cvReleaseImage(&floatImage);		
	}
	
//	double tt = clock();
	// segment
	universe *u = CvSegment::segment_graph(width*height, num, edges, k);
//	fprintf(stderr, "cost time = %f\n", clock() - tt);
	
	// post process small components
	for (int i = 0; i < num; i++) {
		int a = u->find(edges[i].a);
		int b = u->find(edges[i].b);
		if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
			u->join(a, b);
	}
	delete [] edges;
	*labelN = u->num_sets();
	fprintf(stderr, "label Number = %d\n", *labelN);

	if (*labelN > 255) {
		fprintf(stderr, "error label number more than 255 !\n");
		delete u;
		return false;
	}

	
	std::vector<int> index;
	index.resize(width*height);
	for (int i = 0; i<width*height; ++i) {
		int comp = u->find(i);
		index[i] = comp;
	}
	delete u;
	
	std::vector<int> copyIndex = index;
	sort(index.begin(), index.end());
	std::vector<int>::iterator vec_ite = unique(index.begin(), index.end());
	index.erase(vec_ite, index.end());
	std::map<int, int> imap;
	for (int i = 0; i<index.size(); ++i) {
		imap[index[i]] = i;
	}

	outImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	BwImage out_img(outImage);
	for (int y = 0, i = 0; y<height; ++y) {
		for (int x = 0; x<width; ++x, ++i) {
			out_img[y][x] = imap[copyIndex[i]];			
		}
	}	
	return true;	
}
universe *CvSegment::segment_graph(int num_vertices, int num_edges, cv_edge *edges, float c)
{ 
	// sort edges by weight
	std::sort(edges, edges + num_edges);

	// make a disjoint-set forest
	universe *u = new universe(num_vertices);

	// init thresholds
	float *threshold = new float[num_vertices];
	for (int i = 0; i < num_vertices; i++)
		threshold[i] = THRESHOLD(1,c);


	// for each cv_edge, in non-decreasing weight order...
	for (int i = 0; i < num_edges; i++) {
		cv_edge *pedge = &edges[i];

		// components conected by this cv_edge
		int a = u->find(pedge->a);
		int b = u->find(pedge->b);
		if (a != b) {
			if ((pedge->w <= threshold[a]) &&
				(pedge->w <= threshold[b])) {
					u->join(a, b);
					a = u->find(a);
					threshold[a] = pedge->w + THRESHOLD(u->size(a), c);
			}
		}
	}

	// free up
	delete threshold;
	return u;
}