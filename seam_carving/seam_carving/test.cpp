#include "test.h"
#include "image_template\common_include.h"

using cvx_space::RgbImageFloat;
using cvx_space::RgbImage;
using cvx_space::BwImage;
using cvx_space::BwImageFloat;

#include "sparse_zjucad/sparse/sparse.h"
#include "sparse_zjucad/sparse/zjucad/matrix/matrix.h"
#include "image_template/common_include.h"
#include "image_template/cvx_geometry.h"
#include "image_template/image_draw.h"
#include "image_template/cvx_io.h"
#include "image_template/cvx_sf.h"

using namespace::std;
using hj::sparse::spm_csc;
using zjucad::matrix::matrix;
using zjucad::matrix::zeros;
using cvx_space::MatDouble;
using cvx_space::MatInt;

#if 0
//计算texture程度
void gau(IplImage *orgImage, IplImage *&texture)
{
	int width = orgImage->width;
	int height = orgImage->height;
	texture = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	cvZero(texture);

	//梯度权值
	IplImage *gImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvZero(gImage);
	CvxSF::graColorInt(orgImage, gImage);

	IplImage *floatgImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	cvScale(gImage, floatgImage, 1.0);

	IplImage *gauA = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	IplImage *gauB = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	cvSmooth(floatgImage, gauA, CV_GAUSSIAN, 0, 0, 0.5, 0);
	cvSmooth(floatgImage, gauB, CV_GAUSSIAN, 0, 0, 2.0, 0);

	IplImage *tImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	IplImage *showImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	cvDiv(gauA, gauB, tImage, 1.0);

	double maxV = 0;
	cvMinMaxLoc(tImage, NULL, &maxV);
	cvScale(tImage, showImage, 255.0/maxV);

	IplImage *grayImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvCvtColor(orgImage, grayImage, CV_BGR2GRAY);

	vector<int> gray_data;
	gray_data.resize(width * height);
	BwImage g_img(gImage);
	for (int y = 0; y<height; ++y) {
		for (int x = 0; x<width; ++x) {
			gray_data[y*width + x] = g_img[y][x];
		}
	}
	std::sort(gray_data.begin(), gray_data.end());
	int index = width * height * 0.2;
	int threshold = gray_data[index];
	cout<<"threshold = "<<threshold<<endl;
	BwImage show_img(showImage);
	for (int y = 0; y<height; ++y) {
		for (int x = 0; x<width; ++x) {
			if (g_img[y][x] <= threshold) {
				show_img[y][x] = 0;
			}
		}
	}

	cvShowImage("texture", showImage);
	cvSaveImage("texture.bmp", showImage);

}
int main()
{
	IplImage *fogImage = cvLoadImage("org_img\\tree\\tree.png", 1);
	IplImage *bkgImage = cvLoadImage("org_img\\tree\\sky.png", 1);

	cvShowImage("fog", fogImage);
	cvShowImage("bkg", bkgImage);
	
	IplImage *texture = NULL;
	gau(fogImage, texture);

	cvWaitKey(0);
	return 1;
}

#endif

#if 0
//批量转换图像后缀
int main()
{
//	static bool travel_dir(const char *dir, std::vector<std::string> &fileName);
	vector<string> fileName;
	cout<<"input dir like nv\\*.h "<<endl;
	char dir[255] = {NULL};
	scanf("%s", dir);
	CjhIo::travel_dir(dir, fileName);
	cout<<"input dir like nv\ "<<endl;
	memset(dir, 0, sizeof(dir));
	scanf("%s", dir);
	for (int i = 0; i<fileName.size(); ++i) {
		string filePath = string(dir) + fileName[i];
	//	cerr<<"filePan = "<<filePath<<endl;

		IplImage *image = cvLoadImage(filePath.c_str(), 1);
		if (image) {
			string saveName = fileName[i];
			int size = saveName.size();
			saveName[size-1] = 'p';
			saveName[size-2] = 'm';
			saveName[size-3] = 'b';
			cvSaveImage((string(dir) + saveName).c_str(), image);
			cout<<"save file"<<endl;
			cvReleaseImage(&image);
		}
		else
		{
			cerr<<"can not save file "<<fileName[i]<<endl;
		}
	}		

	cvWaitKey(0);
	return 1;
}
#endif 

#if 0
bool endWith(const char *str, const char *suffix)
{
	return (strcmp(str + strlen(str) - strlen(suffix), suffix) == 0);
}

//多张图horizontal 拼接
int main()
{
//	bool CvxSF::horizontalCombine(char* fileName_one, char* fineName_two,
//		int interval, char *saveName)

	printf("horizontal combine images !\n");
	
	vector<IplImage *> imgVec;
	while(1)
	{
		char fileName[255] = "NULL";
		printf("please input an image name, 'q' for end of input image name\n");
		scanf("%s", fileName);
		if (fileName[0] == 'q' && fileName[1] == NULL) {
			break;
		}
		IplImage *orgImage = cvLoadImage(fileName, 1);
		if (!orgImage) {
			printf("can not open Image %s\n", fileName);
		}
		else
		{
			imgVec.push_back(orgImage);
		}
	}
	

	bool isOk = false;
	while(!isOk)
	{
		printf("input result image name end with '.jpg'\n");
		char savefile[255] = "NULL";
		scanf("%s", savefile);

		if (endWith(savefile, ".jpg")) {
			isOk = true;
			CvxSF::hotizontalCombine(imgVec, savefile, 10);
		}
		else
		{
			printf("file name must end with '.jpg' \n");			
		}
	}
	return 1;
}
#endif

#if 0
bool endWith(const char *str, const char *suffix)
{
	return (strcmp(str + strlen(str) - strlen(suffix), suffix) == 0);
}

int main()
{
	char fileName[256];
	memset(fileName, NULL, sizeof(fileName));

	bool bwork = true;	
	while(bwork)
	{
		fprintf(stdout, "input one image name, 'q' to exit\n");
		bool bopen = false;
		fscanf(stdin, "%s", fileName);
		if (fileName[0] == 'q' && fileName[1] == NULL) {
			fprintf(stdout, "complete work\n");
			bwork = false;
		}
		else 
		{
			IplImage *orgImage = cvLoadImage(fileName, 1);
			if (orgImage) {
				IplImage *salImage = cvCreateImage(cvGetSize(orgImage), IPL_DEPTH_8U, 1);
				cvZero(salImage);
				CvxSF::FTsaliency(orgImage, salImage);
				fprintf(stdout, "saliency map complete !\n");
				fprintf(stdout, "input saving file name end with '.jpg', or d for default name\n");
				char saveFile[256];
				memset(saveFile, NULL, sizeof(saveFile));

				bool bsaved = false;
				while(!bsaved && fscanf(stdin, "%s", saveFile))
				{
					if (saveFile[0] == 'd' && saveFile[1] == NULL) {
						strcpy(saveFile, fileName);
						strcpy(saveFile + strlen(saveFile) - 4, "_sal.jpg");
						cvSaveImage(saveFile, salImage);
						fprintf(stdout, "save saliency map as %s\n", saveFile);
						if (orgImage) {
							cvReleaseImage(&orgImage);
						}
						if (salImage) {
							cvReleaseImage(&salImage);
						}
						bsaved = true;
					}
					else if (endWith(saveFile, ".jpg")) {
						cvSaveImage(saveFile, salImage);
						fprintf(stdout, "save saliency map as %s\n", saveFile);
						if (orgImage) {
							cvReleaseImage(&orgImage);
						}
						if (salImage) {
							cvReleaseImage(&salImage);
						}
						bsaved = true;
					}
					if (!bsaved) {
						fprintf(stdout, "save file %s failed !");
						memset(saveFile, NULL, sizeof(saveFile));
						fprintf(stdout, "input saving file name end with '.jpg', or d for default name\n");
					}
				}		
			}
			else
			{
				fprintf(stderr, "can't open image name = %s\n", fileName);
				memset(fileName, NULL, sizeof(fileName));
				fscanf(stdin, "%s", fileName);
			}
		}
	}

	cvWaitKey(0);
	return 1;

}
#endif

#if 0
//将能量图转换为HSV的形式
int main()
{
	char fileA[] = "org_img\\energy_map\\58_sobel.png";
	char fileB[] = "org_img\\energy_map\\58_saliency.png";
	char fileC[] = "org_img\\energy_map\\58_balanced_e.png";
	//
	/*IplImage *energyA = cvLoadImage(fileA, 0);
	IplImage *energyB = cvLoadImage(fileA, 0);
	IplImage *energyC = cvLoadImage(fileA, 0);*/

//	CvxSF::horizontalCombine(fileA, fileB);
//	CvxSF::horizontalCombine("combined.jpg", fileC);

	IplImage *energy = cvLoadImage(fileC, 0);

	int w = energy->width;
	int h = energy->height;

	double minV;
	double maxV;
	cvMinMaxLoc(energy, &minV, &maxV);

	BwImage e_img(energy);
	for (int y = 0; y<h; ++y) {
		for (int x = 0; x<w; ++x) {
			e_img[y][x] = 255 - e_img[y][x];
		}
	}
	cerr<<"minV = "<<minV<<endl;
	cerr<<"maxV = "<<maxV<<endl;
	IplImage *hsvE = NULL;
	ImageDraw::hsvLabelBand120_malloc(hsvE, energy, maxV);
	cvSaveImage("hsvC.bmp", hsvE);
	cvShowImage("hsvE", hsvE);
	/*IplImage *barImage = cvCreateImage(cvSize(20, 255), IPL_DEPTH_8U, 1);
	cvZero(barImage);

	int width = barImage->width;
	int height = barImage->height;
	BwImage bar_img(barImage);
	for (int y = 0; y<height; ++y) {
		for (int x = 0; x<width; ++x) {
			bar_img[y][x] = y;
		}
	}
	
	IplImage *hsvBar = NULL;
	ImageDraw::hsvLabelBand120_malloc(hsvBar, barImage, 255);
	cvSaveImage("hsvBar.bmp", hsvBar);
	cvShowImage("hsvBar", hsvBar);*/

	cvWaitKey(0);
	return 1;
}
#endif

#if 0
//seam carving 做image stitch
int main()
{
	/*char *fileA = "org_img\\stitch_jpg\\1.jpg";
	char *fileB = "org_img\\stitch_jpg\\2.jpg";
	char *fileC = "org_img\\stitch_jpg\\3.jpg";
	char *fileD = "org_img\\stitch_jpg\\4.jpg";*/

	char *fileA = "org_img\\stitch_jpg\\small\\0_small.jpg";
	char *fileB = "org_img\\stitch_jpg\\small\\1_small.jpg";
	char *fileC = "org_img\\stitch_jpg\\small\\2_small.jpg";
	char *fileD = "org_img\\stitch_jpg\\small\\3_small.jpg";

	//计算每个图像开始点和终止点，并计算overlap区域
	IplImage *image[4] = {NULL, NULL, NULL, NULL};
	image[0] = cvLoadImage(fileA, 1);
	image[1] = cvLoadImage(fileB, 1);
	image[2] = cvLoadImage(fileC, 1);
	image[3] = cvLoadImage(fileD, 1);

	int bdyPos[4][2];          //四幅图像的startX和endX
	IplImage *singleImage[4];  //四幅图像单体提出
	int h = image[0]->height;
	for (int i = 0; i<4; ++i) {
		int w = image[i]->width;
		int h = image[i]->height;

		cerr<<"i = "<<i<<endl;
		int startX = 0;
		int endX = 0;
		int curWidth = 0;
		RgbImage cur_img(image[i]);
		for (int x = 0; x<w; ++x) {
			if (!(cur_img[0][x].r >= 250 
				  && cur_img[0][x].g >= 250
				  && cur_img[0][x].b >= 250) &&
				  !(cur_img[h-1][x].r >= 250 
				    && cur_img[h-1][x].g >= 250
				    && cur_img[h-1][x].b >= 250)) {
				cerr<<"startX = "<<x<<endl;
				startX = x;
				break;
			}
		}
		for (int x = w-1; x>=0; --x) {
			if (!(cur_img[0][x].r >= 250 
				&& cur_img[0][x].g >= 250
				&& cur_img[0][x].b >= 250) &&
				!(cur_img[h-1][x].r >= 250 
				  && cur_img[h-1][x].g >= 250
				  && cur_img[h-1][x].b >= 250)) {
				cerr<<"endX = "<<x<<endl;
				endX = x;
				break;
			}
		}
		bdyPos[i][0] = startX;
		bdyPos[i][1] = endX;
		
		curWidth = endX - startX + 1;
		singleImage[i] = cvCreateImage(cvSize(curWidth, h), IPL_DEPTH_8U, 3);
		cvZero(singleImage[i]);

		cvSetImageROI(image[i], cvRect(startX, 0, curWidth, h));
		cvCopy(image[i], singleImage[i], NULL);
		cvResetImageROI(image[i]);

		/*cerr<<endl;
		curWidth = endX - startX + 1;
		IplImage *curImage = cvCreateImage(cvSize(curWidth, h), IPL_DEPTH_8U, 3);
		cvZero(curImage);

		cvSetImageROI(image[i], cvRect(startX, 0, curWidth, h));
		cvCopy(image[i], curImage, NULL);
		cvResetImageROI(image[i]);

		char temp[64];
		string str;
		sprintf(temp, "%d", i);
		string sName(temp);
		sName = sName + string(".jpg");
		cvSaveImage(sName.c_str(), curImage);	*/	
	}
	//写文件
	FILE *fp = fopen("small.txt", "w");
	assert(fp);
	fprintf(fp, "%d\n", 4);
	for (int i = 0; i<4; ++i) {
		fprintf(fp, "%d %d\n", bdyPos[i][0], bdyPos[i][1]);
	}
	fclose(fp);
	//读文件
	/*FILE *fr = fopen("large.txt", "r");
	assert(fr);
	int num = 0;
	fscanf(fr, "%d", &num);
	int *pos = new int[num*2];
	for (int i = 0; i<num; ++i) {
		fscanf(fr, "%d %d", &pos[2*i], &pos[2*i+1]);
	}

	for (int i = 0; i<num; ++i) {
		printf("%d %d\n", pos[2*i], pos[2*i+1]);
	}*/




	cvWaitKey(0);
	return 1;
}
#endif

#if 0
//ForwardSeam
int main()
{
	if (freopen("cvlog.txt", "w", stdout) == NULL)
	{
		printf("re direction error\n");
		exit(-1);
	}
	
#if 1
//	char *path = "org_img\\org_img_num\\";
	char *path = "org_img\\fs_img\\";
	for (int i = 23; i<= 23; ++i)
	{
		char temp[64];
		string str;
		sprintf(temp, "%d", i);
		string orgName(temp);
		orgName = string(path) + orgName + string(".png");

		string fgName;
		fgName = string(path) + string("fg_") + string(temp) + string(".bmp");
	//	cerr<<"fgName = "<<fgName<<endl;		

		
		float ratio = 0.34;	//横向缩小比率               
		IplImage* orgImage = cvLoadImage(orgName.c_str(), 1); //原图
		IplImage* fgImage = cvLoadImage(fgName.c_str(), 0);   //前景区域图

		

		if (fgImage) {
			cerr<<"using foreground map "<<endl;
		}		

		if (!orgImage)
		{
			cvReleaseImage(&orgImage);

			if (fgImage) {
				cvReleaseImage(&fgImage);
			}
			continue;
		}
		int width  = orgImage->width;
		int height = orgImage->height;			
		int delta_w = width * ratio;
		//结果图
		IplImage *scImage = cvCreateImage(cvSize(width - delta_w, height), IPL_DEPTH_8U, 3);
		char *savePath = "save_img\\";   //结果路径
		sprintf(temp, "%d", i);
		string saveName(temp);

		//backward sc
		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::L1NORM, 
				ForwardSeam::BACKWARD_4, true);
			cvSaveImage((string(savePath) + saveName + "no_bsc.png").c_str(), scImage);

		}

		//forward sc
		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::NO_ENERGY, 
				ForwardSeam::FORWARD_4, true);
			cvSaveImage((string(savePath) + saveName + "no_fsc.png").c_str(), scImage);
		}		

		//forward 8 sc
		/*if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::NO_ENERGY, 
				ForwardSeam::FORWARD_8_DIRECT_SUB, false);
			cvSaveImage((string(savePath) + saveName + "no_8_dir_sub.png").c_str(), scImage);
		}*/
		//不同权重
		/*if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
						ForwardSeam::L1NORM_SALIENT, 
						ForwardSeam::FORWARD_4, false, 0.0);
			cvSaveImage((string(savePath) + saveName + "w_00.bmp").c_str(), scImage);
		}*/
		
		//forward 8 sc,单独测试
		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::NO_ENERGY, 
				ForwardSeam::FORWARD_8_BI_SIMILAR, true);
			cvSaveImage((string(savePath) + saveName + "no_8_bi_smi.png").c_str(), scImage);
		}
		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::NO_ENERGY, 
				ForwardSeam::FORWARD_4_VAR_GRA, true);
			cvSaveImage((string(savePath) + saveName + "no_4_fsc.png").c_str(), scImage);
		}
		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::NO_ENERGY, 
				ForwardSeam::FORWARD_4_VAR_DIF, true);
			cvSaveImage((string(savePath) + saveName + "no_4_var_gra_dif.png").c_str(), scImage);
		}
		
		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::NO_ENERGY, 
				ForwardSeam::FORWARD_8_VAR_DIF, false);
			cvSaveImage((string(savePath) + saveName + "no_8_var_gra_dif.png").c_str(), scImage);

		}
		

		//salient forward sc
		if (1) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::SALIENT, 
				ForwardSeam::FORWARD_4, false);
			cvSaveImage((string(savePath) + saveName + "sal_sc.bmp").c_str(), scImage);	
		}
		
		//
		//combine
		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::L1NORM_SALIENT, 
				ForwardSeam::FORWARD_8_BI_SIMILAR, false);  //FORWARD_8_DIRECT_SUB
			cvSaveImage((string(savePath) + saveName + "gra_sal_sc_8_bi.bmp").c_str(), scImage);
		}		

		if (0) {
			ForwardSeam::shrink(orgImage, delta_w, scImage, NULL,
				ForwardSeam::L1NORM_SALIENT, 
				ForwardSeam::FORWARD_4, false);  //FORWARD_8_DIRECT_SUB
			cvSaveImage((string(savePath) + saveName + "gra_sal_sc_4.bmp").c_str(), scImage);
		}		

		if (0) {
			if (fgImage) {
				//combine + fg
				ForwardSeam::shrink(orgImage, delta_w, scImage, fgImage,
					ForwardSeam::L1NORM_SALIENT, 
					ForwardSeam::FORWARD_8_BI_SIMILAR, false);
				cvSaveImage((string(savePath) + saveName + "gra_sal_sc_8_bi_fg.bmp").c_str(), scImage);		
			}		
		}
		

		//综合
		/*typedef enum
		{
		FORWARD_4 = 0,
		FORWARD_8 = 1,
		DI_DIR_FORWARD_8 = 2
		}operatortype;*/
		/*ForwardSeam::shrink(orgImage, delta_w, scImage, ForwardSeam::L1NORM_SALIENT, 
			ForwardSeam::FORWARD_8);
		cvSaveImage((string(savePath) + saveName + "gra_sal_sc_8.jpg").c_str(), scImage);*/

		if (orgImage) {
			cvReleaseImage(&orgImage);
		}
		if (scImage)
		{
			cvReleaseImage(&scImage);
		}
		if (fgImage) {
			cvReleaseImage(&fgImage);
		}
		

		cerr<<" i = "<<i<<"complete "<<endl;
	}
#endif

	cvWaitKey(0);
	return 1;
}
#endif



	








#if 0
int main()
{
		char fileName[] = "org_img\\face.bmp";
	//	char fileName[] = "org_img\\three_girl.bmp";
	//	char fileName[] = "org_img\\buildings.bmp";
	//	char fileName[] = "org_img\\house_inner.bmp";
	//	char fileName[] = "org_img\\sant.jpg";
	//char fileName[] = "org_img\\house_girl.bmp";
	IplImage* orgImage = cvLoadImage(fileName, 1);
	int width  = orgImage->width;
	int height = orgImage->height;
	int delta_w = width * 0.4;
	IplImage *normalImage = cvCreateImage(cvSize(width - delta_w, height), IPL_DEPTH_8U, 3);
	IplImage *scImage = cvCreateImage(cvSize(width - delta_w, height), IPL_DEPTH_8U, 3);
	IplImage *antiDistorImage = cvCreateImage(cvSize(width - delta_w, height), IPL_DEPTH_8U, 3);

	cvResize(orgImage, normalImage);
	cvSaveImage("distort_img\\normal.jpg", normalImage);

	ForwardSeam::shrink(orgImage, delta_w, scImage);
	cvSaveImage("distort_img\\scImage.jpg", scImage);
	
	ForwardSeam::shrinkEneryScatter(orgImage, delta_w, antiDistorImage);
	cvSaveImage("distort_img\\antiDistor.jpg", antiDistorImage);


	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
	char fileName[] = "org_img\\saliency_posa.jpg";
//	char fileNameA[] = "org_img\\posa.jpg";
	char fileName[] = "org_img\\jon.bmp";
//	char fileNameA[] = "org_img\\jon.bmp";
	IplImage *srcImage = cvLoadImage(fileName, 0);
	IplImage *srcColorImage = cvLoadImage(fileNameA, 1);

#if 1
	ImageQuad myIO;
	myIO.loadImage(srcImage);
	myIO.reSizeImage(20);

	IplImage *maskImage = cvCreateImage(cvGetSize(srcColorImage), IPL_DEPTH_8U, 1);
	cvSet(maskImage, cvScalarAll(255), NULL);
	
	cvShowImage("maskImage", maskImage);
	cvSaveImage("posa_maskImage.bmp", maskImage);

#endif
	cvWaitKey(0);	
	return 1;
}
#endif
 




#if 0
int main()
{
//	char srcFileName[] = "org_img\\airplane.jpg";
	char srcFileName[] = "org_img\\13.jpg";
//	char dstFileName[] = "org_img\\lena.bmp";

	IplImage *srcImage = cvLoadImage(srcFileName, 0);
//	IplImage *dstImage = cvLoadImage(dstFileName, 0);
//	CvxSF::changeMagAndPha(srcImage, dstImage);
	
	IplImage *salImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F, 1);
	cvZero(salImage);
//	CvxSF::PFT(srcImage, salImage);
	CvxSF::SRA(srcImage, salImage);


	cvWaitKey(0);
	return 1;
}
#endif


#if 0
int main()
{
	int i = 0 , j = 0 , k = 0;
   CvScalar temp;
   //double temp_get;
   CvMat *mat_A = cvCreateMat(2 , 3 , CV_32FC2);   //   2行3列的2通道矩阵
   cvZero(mat_A);
   IplImage *rgbImage = cvCreateImage(cvSize(3, 2), IPL_DEPTH_8U, 3);
   cvZero(rgbImage);
   RgbImage rgb_img(rgbImage);
   for(i = 0 ; i < 2 ; i++)
   {
      for (j = 0 ; j < 3 ; j++)
      {
         temp = cvScalar(i * 3 + j , /*i * 3 + j + 1*/0 , 0 , 0);      //   必须凑齐4个参数，本以为是2通道只需要2个参数的，这样容易混淆
         cvSet2D(mat_A , i , j , temp);
		 rgb_img[i][j].r = i * 3 + j;
		 rgb_img[i][j].g = /*i * 3 + j + 1*/0;
      }
   }
   //IplImage *dftImage = cvCreateImage(cvSize(3, 2), IPL_DEPTH_32F, 3);
   //cvZero(dftImage);
   //CvxSF::rgbDFT(rgbImage, dftImage);
   ////归一化
   //CvxSF::normalize(dftImage);

   //IplImage *inverse_dftImage = cvCreateImage(cvSize(3, 2), IPL_DEPTH_32F, 3);
   //cvZero(inverse_dftImage);
   //CvxSF::IDFTfloat(dftImage, inverse_dftImage);
   


   for(i = 0 ; i < 2 ; i++)
   {
      for (j = 0 ; j < 3 ; j++)
         printf("%f + %fi   " , cvGet2D(mat_A , i , j).val[0] , cvGet2D(mat_A , i , j).val[1]);
      printf("\n");
   }

   printf("\n ===============2DFT==============\n\n");
   cvDFT(mat_A , mat_A , CV_DXT_FORWARD , 0);   //   2维傅里叶变换



   for(i = 0 ; i < 2 ; i++)
   {
      for (j = 0 ; j < 3 ; j++)
         printf("%f + %fi   " , cvGet2D(mat_A , i , j).val[0] , cvGet2D(mat_A , i , j).val[1]);
      printf("\n");
   }

   printf("\n ===============2DIFT==============\n\n");
   cvDFT(mat_A , mat_A , CV_DXT_INV_SCALE , 0);   //   2维傅里叶逆变换  CV_DXT_INV_SCALE

   for(i = 0 ; i < 2 ; i++)
   {
      for (j = 0 ; j < 3 ; j++)
         printf("%f + %fi   " , cvGet2D(mat_A , i , j).val[0] , cvGet2D(mat_A , i , j).val[1]);
      printf("\n");
   }

 //  CvxSF::normalize(inverse_dftImage);
//	CvxMemTracker mem;
#if 0
   IplImage *colorImage = cvLoadImage("org_img\\child.bmp", 1);
   IplImage *grayImage = cvCreateImage(cvGetSize(colorImage), IPL_DEPTH_8U, 1);
   cvCvtColor(colorImage, grayImage, CV_BGR2GRAY);
   int const width = colorImage->width;
   int const height = colorImage->height;
   RgbImage color_img(colorImage);
   BwImage  gray_img(grayImage);
   for(int y = 0; y<height; ++y)
   {
	   for(int x = 0; x<width; ++x)
	   {
		   color_img[y][x].r = gray_img[y][x];
		   color_img[y][x].g = 0;
		   color_img[y][x].b = 0;
	   }
   }

   IplImage *dft_twoImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 3);
   CvxSF::rgbDFT(colorImage, dft_twoImage);
   CvxSF::normalize(dft_twoImage);

   IplImage *dft_inverseTwoImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 3);
   cvZero(dft_inverseTwoImage);

   CvxSF::IDFTfloat(dft_twoImage, dft_inverseTwoImage);

   float max_float = 0;
   RgbImageFloat inverse_img(dft_inverseTwoImage);
   for(int y = 0; y<height; ++y)
   {
	   for(int x = 0; x<width; ++x)
	   {
		   if(inverse_img[y][x].b > max_float)
		   {
			   max_float = inverse_img[y][x].b;
		   }
	   }
   }
   printf("%f \n", max_float);
   IplImage *showImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
   cvZero(showImage);
   BwImage show_img(showImage);
   for(int y = 0; y<height; ++y)
   {
	   for(int x = 0; x<width; ++x)
	   {
		   show_img[y][x] = inverse_img[y][x].b /(max_float) *255;
	   }
   }
   CvxSF::showImage(showImage);
#endif



	 



//	cout<<"cost time = "<<clock() - tt<<endl;

//	cvReleaseImage(&srcImg);
	cvWaitKey(0);
	cvDestroyAllWindows();
	return 1;
}
#endif

#if 0
//测试 ShiftMapCompare
int main()
{
//	CvxMemTracker mem;
//	char srcFileName[] = "org_img\\bird.bmp";
//	char dstFileName[] = "org_img\\bird_shift_result_240.bmp";

	char srcFileName[] = "org_img\\g_paper_s.jpg";
	char dstFileName[] = "org_img\\g_paper_t.jpg";

	IplImage* srcImage = cvLoadImage(srcFileName, 1);
	IplImage* dstImage = cvLoadImage(dstFileName, 1);
	IplImage* shiftMapImage = cvCreateImage(cvGetSize(dstImage), IPL_DEPTH_32S, 1);
	cvZero(shiftMapImage);
	double tt = clock();
	BDWarp::blendMatch(srcImage, dstImage, shiftMapImage);
	cerr<<"BDWarp::blendMatch cost time"<<clock() - tt<<endl;
	tt = clock();

//	ShiftMap::getShiftMap(srcFileName, shiftMapImage);
//	BDWarp::patchMatch(srcImage, dstImage, 1);
//	cerr<<"BDWarp::patchMatch cost time"<<clock() - tt<<endl;

//	tt = clock();
//	BDWarp::patchMatchTS(dstImage, srcImage, 1);
//	

	cvWaitKey(0);
	return -1;
}
#endif
#if 0
int main()
{
#if 0
	char tsResultFileName[] = "org_img\\eagle_ts.bmp";
	int delta_X = 200;

	IplImage* tsResultImage = cvLoadImage(tsResultFileName, 1);
	IplImage* shiftMapImage = cvCreateImage(cvSize(tsResultImage->width - delta_X, tsResultImage->height),
		IPL_DEPTH_32S, 1);
	assert(tsResultImage);
	assert(shiftMapImage);
	ShiftMap::reverseShiftMap(tsResultImage, shiftMapImage, delta_X);
#endif

#if 0
	//测试最近邻居算法
//	char srcFileName[] = "org_img\\bird.bmp";
	char srcFileName[] = "org_img\\face.bmp";

	IplImage* srcImage = cvLoadImage(srcFileName, 1);
	IplImage* dstImage = cvCreateImage(cvSize(srcImage->width/2, srcImage->height), IPL_DEPTH_8U, 3);
	IplImage* shiftImage = cvCreateImage(cvGetSize(dstImage), IPL_DEPTH_32S, 1);

	cvResize(srcImage, dstImage, CV_INTER_NN);
	BDWarp::noBlendMatch(srcImage, dstImage, shiftImage);

//	CvxSF::showImage(dstImage, "dstImage");

#endif

#if 1
	char srcFileName[] = "org_img\\bird.bmp";
	char dstFileName[] = "org_img\\bird_shift_result_240.bmp";

	IplImage* srcImage = cvLoadImage(srcFileName, 1);
	IplImage* dstImage = cvLoadImage(dstFileName, 1);

	IplImage* shiftImage = cvCreateImage(cvGetSize(dstImage), IPL_DEPTH_32S, 1);
	double tt = clock();
//	BDWarp::blendMatch(srcImage, dstImage, shiftImage);
	cerr<<"cost time"<<clock() - tt<<endl;
	IplImage* interNnImage = cvCreateImage(cvSize(240, srcImage->height), IPL_DEPTH_8U, 3);
	cvResize(srcImage, interNnImage, CV_INTER_NN);
//	CvxSF::showImage(interNnImage);
	BDWarp::noBlendMatch(srcImage, interNnImage, shiftImage);

//	BDWarp::patchMatch(srcImage, dstImage, 1);

	char tsResultFileName[] = "org_img\\bird_ts.bmp";
	int delta_X = 80;

	IplImage* tsResultImage = cvLoadImage(tsResultFileName, 1);
	IplImage* shiftMapImage = cvCreateImage(cvSize(tsResultImage->width - delta_X, tsResultImage->height),
		      IPL_DEPTH_32S, 1);
	assert(tsResultImage);
	assert(shiftMapImage);
//	ShiftMap::reverseShiftMap(tsResultImage, shiftMapImage, delta_X);
#endif


	

	

	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main( int argc, char** argv )
{
	CvxMemTracker mem;
//	char fileName[] = {"org_video\\mouse_org.avi"};
	char fileName[] = {"org_video\\mouse_210.avi"};
//	char fileName[] = {"org_video\\golf_ds.avi"};
//	char fileName[] = {"org_video\\140_multi_result.avi"};
//	char fileName[] = {"org_video\\100_multi_result.avi"};
//	char fileNameA[] = {"org_video\\golf_ds.avi"};
//	char fileName[] = {"org_video\\golf_ten.avi"};
//	char fileName[] = {"org_video\\golf_twenty.avi"};
//	char resultFileName[] = {"org_video\\mouse_retarget.avi"};
//	CJHVideo myCV;
//	myCV.loadFile(resultFileName);

//	CvxSF::horizontalCombineVideo(fileNameA, fileName);

#if 1
	double tt = clock();
	LevelVideoCarve mLVC;
	mLVC.loadFile(fileName);
	mLVC.getIntervalImage(2);
	mLVC.multiIntervalShrink(10);

//	mLVC.intervalShrink(1);


//	mLVC.getImage();
//	mLVC.shrink(1);

	cout<<"cost time = "<<clock() - tt<<endl;

#endif
	cvWaitKey(0);	
	return 1;				
}
#endif




#if 0
int main()
{
	MultiOperator myMultiop;
	//	char fileName[] = {"pagoda.bmp"};
	//	char fileName[] = {"orchid.bmp"};
	//	char fileName[] = {"org_img\\face.bmp"};
	char fileName[] = {"org_img\\g_paper_s.jpg"};
	//	char fileName[] = {"pigeons_400.bmp"};

	//	char fileName[] = {"jon.bmp"};
	//	char fileName[] = {"eagle.bmp"};
	//	char fileName[] = {"buildings.bmp"};
	//	char fileName[] = {"bird.bmp"};
	int delta_x = 160;
	myMultiop.loadFile(fileName);
	//	double tt = clock();
	IplImage* srcImg = cvLoadImage(fileName, 1);
	int width = srcImg->width;
	int height = srcImg->height;
	IplImage* dstImg = cvCreateImage(cvSize(width - 54 - 198 - 31, height), IPL_DEPTH_8U, 3);
	myMultiop.shrink(54, 198, 31, dstImg);
	CvxSF::showImage(dstImg);
	//	printf(" multiop cost time = %f \n", clock() - tt);

#if 0
	cvGui myGui;
	int posSC = 1;
	int posCR = 1;
	myGui.setWindow("bar");
	myGui.loadFile(fileName);
	myGui.setShrinkNumber(delta_x);
	myGui.setTrackbar(10, 100);

#endif

	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
	CvxMemTracker mem;
	char srcFileName[] = {"org_img\\g_paper_s.jpg"};
	char dstFileName[] = {"org_img\\g_paper_t.jpg"};

	/*char srcFileName[] = {"org_img\\three_girl_src.jpg"};
	char dstFileName[] = {"org_img\\three_girl_dst.jpg"};*/
//	char dstFileName[] = {"g_three_op.jpg"};
//	char dstFileName[] = {"g_three_op_min_dif.jpg"};

//	
//	char dstFileNameB[] = {"three_girl_dst.jpg"};


	/*char srcFileName[] = {"org_img\\eagle.bmp"};
	char dstFileName[] = {"org_img\\eagle_target.bmp"};*/


	int patchSize = 4;
	IplImage* imageSRC = cvLoadImage(srcFileName, 1);
	IplImage* imageDST = cvLoadImage(dstFileName, 1);
//	IplImage* imageA = cvLoadImage(dstFileNameA, 1);
//	IplImage* imageB = cvLoadImage(dstFileNameB, 1);

	cerr<<BDWarp::compare(imageSRC, imageDST, patchSize)<<endl;	

//	CvxSF::stretchPatch(imageA, patchSize);
//	CvxSF::stretchPatch(imageB, patchSize);
	
//	CvxSF::showImage(imageB);
#if 0
	BDWarp::patchMatch(imageSRC, imageDST, patchSize);
 //   BDWarp::patchMatchTS(imageB, imageA, patchSize);
#endif
#if 0
	double tt = clock();
	cerr<<"dif in paper = "<<BDWarp::compare(imageSRC, imageA, patchSize)<<endl;
	cerr<<"dif caculated = "<<BDWarp::compare(imageSRC, imageB, patchSize)<<endl;	
	
	cerr<<"cost time = "<<clock() - tt<<endl;
#endif

	cvReleaseImage(&imageSRC);
	cvReleaseImage(&imageDST);

	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
	/*char fileName_A[] = {"girls\\three_girl_src.jpg"};
	char fileName_B[] = {"girls\\three_girl_dst.jpg"};
	char fileName_C[] = {"girls\\g_ts.jpg"};
	char fileName_D[] = {"girls\\g_st.jpg"};*/
#if 0
	char fileName_A[] = {"eagles\\eagle.bmp"};
	char fileName_B[] = {"eagles\\eagle_target.bmp"};
	char fileName_C[] = {"eagles\\e_ts.jpg"};
	char fileName_D[] = {"eagles\\e_st.jpg"};

	CvxSF::horizontalCombine(fileName_A, fileName_B);
	CvxSF::horizontalCombine("combined.jpg", fileName_C);
	CvxSF::horizontalCombine("combined.jpg", fileName_D);
#endif

	CvxSF::verticalCombine("eagles_combined.jpg", "girls_combined.jpg");

	cvWaitKey(0);
	return 1;
}
#endif







#if 0
int main()
{
//	char fileName[] = {"org_img\\taj.bmp"};
//	char fileName[] = {"org_img\\jon.bmp"};
	char fileName[] = {"org_img\\bird.bmp"};
//	char fileName[] = {"org_img\\orchid.bmp"};
//	char fileName[] = {"org_img\\face.bmp"};

#if 1
	int delta_x = 160;
	ThreeStepGui myTSG;
	myTSG.setWindow("threeStep");
	myTSG.loadFile(fileName);
	myTSG.setShrinkNumber(delta_x);
	myTSG.setTrackbar(50, 100);
#endif
#if 0
//	CJHImage::horizontalCombine("scaleThenSCThenSC_result_50.jpg", "scaleThenSCThenSC_result_70.jpg");
//	CJHImage::horizontalCombine("combined.jpg", "scaleThenSCThenSC_result_80.jpg");
//	CJHImage::horizontalCombine("combined.jpg", "scaleThenSCThenSC_result_100.jpg");
	CJHImage::horizontalCombine(fileName, "multi_op_result.jpg");
	CJHImage::horizontalCombine("combined.jpg", "scaleThenSCThenSC_crop_result.jpg");
	CJHImage::verticalCombine("four_birds.jpg", "combined.jpg");
#endif



	cvWaitKey(0);
	return 1;
}
#endif
#if 0
int main()
{
//	char fileName[] = {"org_img\\waterfall.bmp"};
//	char fileName[] = {"org_img\\rain.bmp"};
//	char fileName[] = {"org_img\\jon.bmp"};
	char fileName[] = {"org_img\\taj.bmp"};
	//	char fileName[] = {"org_img\\bird.bmp"};
//	char fileName[] = {"org_img\\orchid.bmp"};
//	char fileName[] = {"org_img\\face.bmp"};

//	char fileName[] = {"org_img\\eagle.bmp"};
	int thickness = 20;
	IplImage* srcImg = cvLoadImage(fileName, 1);
	int width = srcImg->width;
	int height = srcImg->height;
	IplImage* dstImg = cvCreateImage(cvSize(width - thickness*2, height - thickness*2),
		IPL_DEPTH_8U, 3);
	IplImage* normalCropImg = cvCreateImage(cvSize(width - thickness*2, height - thickness*2),
		IPL_DEPTH_8U, 3);
	BoundaryCrop::optimalCrop(srcImg, thickness, dstImg);
	BoundaryCrop::crop(srcImg, thickness, normalCropImg);

	CJHImage::showImage(srcImg, "org");
	CJHImage::showImage(dstImg, "optCropResult");
	CJHImage::showImage(normalCropImg, "cropResult");
	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
	MaxSeam myMS;
	CJHImage myCJHI;

//	char fileName[] = {"org_img\\waterfall.bmp"};
//	char fileName[] = {"org_img\\rain.bmp"};
	char fileName[] = {"org_img\\jon.bmp"};
//	char fileName[] = {"org_img\\taj.bmp"};
//	char fileName[] = {"org_img\\bird.bmp"};
//	char fileName[] = {"org_img\\orchid.bmp"};
//	char fileName[] = {"org_img\\eagle.bmp"};

//	char fileName[] = {"org_img\\pagoda.bmp"};

	IplImage* srcImg = cvLoadImage(fileName, 1);
	int deltaX = 200;
	myMS.loadFile(fileName, false);
//	myMS.levelEnlarge(80);

	myMS.scaleThenSCThenSL(deltaX, 150);

//	myCJHI.loadFile(fileName, 1);
//	myCJHI.sobel();

#if 1
	InterFace myIF;
	myIF.LoadFile(fileName);	
	IplImage* dstImg = cvCreateImage(cvSize(srcImg->width - deltaX, srcImg->height),
		IPL_DEPTH_8U, 3);
	myIF.shrink(deltaX, dstImg);
	CJHImage::showImage(dstImg, "interFaceResult");	
	cvSaveImage("SC_result.jpg", dstImg);
#endif

	CJHImage::horizontalCombine("SC_result.jpg", "scaleThenSCThenSC_result.jpg", 10);
	CJHImage::verticalCombine("combined.jpg", fileName);

	cvWaitKey(0);
	return 1;
}
#endif


#if 0
int main()
{
//	char fileName[] = {"alpha_expansion_image\\black_white.jpg"};
//	char fileName[] = {"alpha_expansion_image\\head.jpg"};
	char fileName[] = {"org_img\\black_white_changed.jpg"};
	int width = 10;
	int height = 5;
	int num_pixels = width*height;
	int num_labels = 7;

	AlphaExpansion myAE;
//	myAE.gc_optimization(width, height, num_pixels, num_labels);
//	myAE.GeneralGraph_DArraySArraySpatVarying(width, height, num_pixels, num_labels);

	myAE.loadFile(fileName);
	myAE.getDataAndSmooth(5);
//	myAE.initLabel(10);
//	myAE.add_node_edges();
//	myAE.min_cut();
//	myAE.showBorder();

//	myAE.showLabel();

	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
//	char fileName[] = {"shift_map_image\\pigeons_100.bmp"};
//	char fileName[] = {"shift_map_image\\pigeons_200.bmp"};
//	char fileName[] = {"shift_map_image\\pigeons_400.bmp"};
//	char fileName[] = {"shift_map_image\\pigeons.jpg"};
	char fileName[] = {"org_img\\pagoda.bmp"};

	GraphCut myGC;
	myGC.loadFile(fileName);
	myGC.setNewWidth(599);

	cvWaitKey(0);
	return 1;
}
#endif


#if 0
int main()
{

//	char fileName[] = {"shift_map_image\\pigeons_100.bmp"};
//	char fileName[] = {"shift_map_image\\pigeons_200.bmp"};
	char fileName[] = {"shift_map_image\\pigeons_400.bmp"};
//	char fileName[] = {"shift_map_image\\pigeons.jpg"};
//	char fileName[] = {"pry.bmp"};

//	char fileName[] = {"child_enlarged.bmp"};
	IplImage* srcImg = cvLoadImage(fileName, 1);
	int width = srcImg->width;
	int height = srcImg->height;	

//	IplImage* dstImg = cvCreateImage(cvSize(width/2, height/2), IPL_DEPTH_8U, 3);
//	CJHImage::pryLevelDown(srcImg, dstImg);	
//	cvSaveImage("pry.bmp", dstImg);
	
	ShiftMapGraph mySMG;
	mySMG.loadFile(fileName);
	mySMG.setTargetSize(width/2, height);

	int dEnergy = mySMG.getDataEnergy();
	cerr<<"dEnergy =  "<<dEnergy<<endl;

	long long int totalE =  mySMG.getEnergy();
	cerr<<"totalE = "<<totalE<<endl;

	long long int max_ll = LLONG_MAX;
	cout<<max_ll<<endl;

	



//	cvNamedWindow("test", 1);
//	cvShowImage("test", srcImg);



	cvWaitKey(0);
	return 1;
}
#endif


#if 0
int main()
{
//	char fileName[] = {"sant.jpg"};
	char srcfileName[] = {"child.bmp"};
//	char dstFileName[] = {"feature_child.bmp"};  //feature_child.bmp
	char dstFileName[] = {"shrinkMid_child.bmp"};
//	char dstFileName[] = {"seam_child.jpg"}; 
//	char dstFileName[] = {"child_no_mid.jpg"};
 
//	char dstFileName[] = {"child_naive.jpg"};
	IplImage * src = cvLoadImage(srcfileName, 1);

	IplImage * dst = cvLoadImage(dstFileName, 1);

	float ret = 0;
	ret = BDWarp::compare(src, dst);
	float ret2 = 0;
	ret2 = BDWarp::compare(dst, src);
	cerr<<"ret ="<<ret<<endl;
	cerr<<"ret2 = "<<ret2<<endl;
	cerr<<"ret + ret2 = "<<ret + ret2<<endl;

	cvWaitKey(0);
	return 1;
}
#endif


#if 0
int main()
{
	LevelSeamInterface myLevelSeam;
	char fileName[] = {"eagle_small.png"};
	//	char fileName[] = {"child.bmp"};
	//	char fileName[] = {"pagoda.bmp"};
	myLevelSeam.loadFile(fileName);
	double tt = clock();
	myLevelSeam.shrinkMid(3);
	printf("cost time %f\n", clock()- tt);
	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
	char fileName[] = {"child.jpg"};
//	char fileName[] = {"pagoda.bmp"};
	IplImage* myImage = cvLoadImage(fileName , 1);
//	CJHImage::columnEnergyD(myImage);

	CJHImage::horizontalCombine("child_original.jpg", "naive.jpg", 10);
	CJHImage::horizontalCombine("combined.jpg", "no_mid_split.jpg", 10);
	CJHImage::horizontalCombine("combined.jpg", "mid_split.jpg", 10);


	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{	
//	char fileName[] = {"gym.jpg"};
//	char fileName[] = {"child.jpg"};
	char fileName[] = {"eagle_small.png"};
//	char fileName[] = {"pagoda.bmp"};
//	char fileName[] = {"pagoda_bottom_up.bmp"};
	NaiveSeamCarving myNSC;
	myNSC.loadFile(fileName);
	vector<pair<int, int>> seam;
	myNSC.shrink(2);
#if 0
	InterFace myInterface;
	myInterface.LoadFile(fileName);
	myInterface.setNewWidth(180);
#endif

	cvWaitKey(0);
	return 1;
}
#endif



#if 0
int main()
{
//	char fileName[] = {"face.bmp"};
	char fileName[] = {"pagoda.bmp"};
	IplImage* src = cvLoadImage(fileName, 1);
	assert(src);
	int width = src->width;
	int height = src->height;

	IplImage* dst = cvCreateImage(cvSize(width/2, height/2), IPL_DEPTH_8U, 3);
	cvPyrDown(src, dst, CV_GAUSSIAN_5x5);

	width = dst->width;
	height = dst->height;

	IplImage* upImg = cvCreateImage(cvSize(width*2, height*2), IPL_DEPTH_8U, 3);
	cvPyrUp(dst, upImg);

	cvNamedWindow("src", 1);
	cvShowImage("src", src);

	cvNamedWindow("dst", 1);
	cvShowImage("dst", dst);

	cvNamedWindow("up", 1);
	cvShowImage("up", upImg);


	cvWaitKey(0);
	return 1;
}
#endif





#if 0
int main()
{
	InterFace myInterface;
//	char fileName[] = {"gym.jpg"};
//	char fileName[] = {"child.jpg"};
//	char fileName[] = {"buildings.bmp"};
	char fileName[] = {"orchid.bmp"};
//	char fileName[] = {"white_black.bmp"};
	myInterface.LoadFile(fileName);

	myInterface.setNewWidth(200);

	cvWaitKey(0);

	return 1;
}
#endif

#if 0
int main()
{
	StepMap::getMap(400, 400);

}
#endif
#if 0
int main()
{
	Circle myCircle;
	myCircle.setPart(36);
	myCircle.generateVertex();

	D3DQuad myQuad;
	myQuad.initD3D();
	myQuad.initUniformMesh(myCircle, "gym.jpg");

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			myQuad.render();
		}
	
	}
	return 1;
}
#endif

#if 0
int main()
{
	Trape myTrape;
	char fileName[] = {"child_enlarge_result.bmp"};
	myTrape.loadFile(fileName);
	myTrape.setHeight(300);
	
	/*vector<D3DXVECTOR3> vec; 
	vector<pair<float, float>> tex;
	myTrape.getVUV(vec, tex);*/

	D3DQuad myQuad;
	myQuad.initD3D();
//	myQuad.loadImage("child.jpg");
//	myQuad.loadImage("child_enlarged_normal.bmp");
//	myQuad.loadImage(fileName);
//	myQuad.initUniformMesh(myTrape, true, "child.jpg");
	myQuad.initUniformMesh(myTrape, false, "child_enlarged.bmp");

	CJHImage::horizontalCombine("no_seam_carving.jpg", "seam_carving.jpg");
	CJHImage::horizontalCombine("child.jpg", "combined.jpg");
	CJHImage::verticalCombine("example_in_paper.jpg","combined.jpg");

	/*MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			myQuad.render();
		}
	}*/

	return 1;
}
#endif

#if 0
int main()
{
	LevelSeamInterface myLSInterface;
//	char fileName[] = {"child_small.bmp"};
//	char fileName[] = {"child.bmp"};
	char fileName[] = {"eagle_small.png"};
//	char fileName[] = {"pagoda_bottom_up.bmp"};
//	char fileName[] = {"pagoda.bmp"};
	myLSInterface.loadFile(fileName);
	myLSInterface.shrink(3);
//	myLSInterface.enlarge(1);
#if 0
	IplImage* accu = cvLoadImage("accu.bmp", 0);
	IplImage* dst  = cvCreateImage(cvGetSize(accu), IPL_DEPTH_32S, 1);
	cvConvertScale(accu, dst, 5000, 0);
	cvSaveImage("accu_dst.bmp", dst);
#endif
	cvWaitKey(0);

	return 1;
}
#endif

#if 0
int main()
{
	InterFace myInterface;
	char fileName[] = {"child.jpg"};
 	myInterface.LoadFile(fileName);

	myInterface.setNewWidth(200);

	cvWaitKey(0);

	return 1;
}
#endif

#if 0
int main()
{
	CJHImage myImage;
//	char fileName[] = {"pagoda_bottom_up.bmp"};
//	char fileName[] = {"pagoda.bmp"};
	char fileName[] = {"child_small.bmp"};
	//	char fileName[] = {"beach.jpg"};
	//	char fileName[] = {"sant.jpg"};
//	char fileName[] = {"child_inverse.jpg"};

	myImage.loadFile(fileName, 1);
	myImage.sobel();
	IplImage* energy = myImage.getPEnergy();

	LevelSeam myLevelSeam;
	myLevelSeam.setEnergyMap(energy);

	IplImage* in_img = cvLoadImage(fileName, 1);

	myLevelSeam.enlarge(in_img, 1200, 3);

	cvReleaseImage(&in_img);
	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
	CJHImage myImage;
	char fileName[] = {"pagoda_bottom_up.bmp"};
//	char fileName[] = {"lena.bmp"};
//	char fileName[] = {"child.jpg"};
//	char fileName[] = {"beach.jpg"};
//	char fileName[] = {"sant.jpg"};
//	char fileName[] = {"ta_dao.jpg"};


	myImage.loadFile(fileName, 1);
	myImage.sobel();
	IplImage* energy = myImage.getPEnergy();

	LevelSeam myLevelSeam;
	myLevelSeam.setEnergyMap(energy);

	vector<pair<int, int>> seam;

	int step_y  = 0;
	int delta_y = 3;
	bool is_left = true;
	for(int i=0; i<myImage.getHeight(); ++i)
	{
		
		if(i % delta_y == 0)
		{	
			myLevelSeam.shrinkFromSubMap(delta_y*step_y, step_y,seam, is_left);
			is_left = !is_left;
			++step_y;			
		}
	}

	myImage.showMap(seam);
	myImage.showMap(seam, 1.0, false);	

	cvWaitKey(0);
	return 1;
}
#endif

#if 0
int main()
{
	CJHImage myImage;
	char fileName[] = {"pagoda_bottom_up.bmp"};
//	char fileName[] = {"pagoda.bmp"};
//	char fileName[] = {"child.jpg"};


	myImage.loadFile(fileName, 1);
	myImage.sobel();
	IplImage* energy = myImage.getPEnergy();

	LevelSeam myLevelSeam;
	myLevelSeam.setEnergyMap(energy);

	vector<pair<int, int>> seam;

	bool is_symmetrical = true;
	for(int i = 0; i<myImage.getHeight() ; i++)
	{
		if(i % 5 == 0)
		{
			myLevelSeam.balancedShrink(i, seam, is_symmetrical);
		}
		is_symmetrical = !is_symmetrical;
	}	
	myImage.showMap(seam);
	myImage.showMap(seam, 1.0, false);

	cvWaitKey(0);
	return 1;
}
#endif

