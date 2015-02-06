#ifndef TEST_H
#define TEST_H

//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>
#include "cvx_image\cvx_image.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <utility>
#include <map>
#include <fstream>
#include <ctime>
#include <valarray>
#include <string>
#include <vector>
#include <algorithm>
#include "image_template\cvx_sf.h"
#include "nv\nvmath.h"
#include "seam_carving\forward_seam.h"


using std::ofstream;
using std::streambuf;
using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::priority_queue;
using std::make_pair;
using std::valarray;
using std::ostream_iterator;
using std::ifstream;
using std::string;
using std::vector;

using nv::vec3i;


ofstream   logFile("cvlog.txt"); //, ios::app  
//�ı�ostream �� rdbuf ���Ϳ����ض����ˣ��������ж� fstream �� stringstream ��û�á�
//streambuf   *outbuf = cout.rdbuf(logFile.rdbuf());//�䶨��logFille��   
//streambuf   *errbuf = cerr.rdbuf(logFile.rdbuf());  


using cvx_space::RgbImage;

#endif