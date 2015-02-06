#ifndef CV_SEGMENT_H
#define CV_SEGMENT_H 1
/*
	original implement is in http://people.cs.uchicago.edu/~pff/segment/
	this code is just rewrite with openCV 

	efficient graph-based image segmentation
	Efficient Graph-Based Image Segmentation
	Pedro F. Felzenszwalb and Daniel P. Huttenlocher
	International Journal of Computer Vision, Volume 59, Number 2, September 2004		
*/

struct _IplImage;
class universe;
struct _cv_edge;

class CvSegment
{
public:
	
	/*
	* Segment an image
	*
	* Returns a label image representing the segmentation.
	*
	* inputImage: image to segment. width and height must less than 1024
	* sigma: to smooth the image.
	* k: constant for treshold function.
	* min_size: minimum component size (enforced by post-processing stage).
	* labelN: number of connected components (labels) in the segmentation.
	*/
	static bool segment(_IplImage *inputImage, _IplImage *&outImage, int *labelN,
		                float sigma = 0.5, float k = 500, int min_size = 400);

private:
	/*
	* Segment a graph
	*
	* Returns a disjoint-set forest representing the segmentation.
	*
	* num_vertices: number of vertices in graph.
	* num_edges: number of edges in graph
	* edges: array of edges.
	* c: constant for treshold function.
	*/
	static universe *segment_graph(int num_vertices, int num_edges, _cv_edge *edges, float c);
	
};
#endif