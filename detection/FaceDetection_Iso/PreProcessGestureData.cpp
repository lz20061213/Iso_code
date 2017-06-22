#pragma once
#include "PreProcessGestureData.h"
#include <fstream>
#include <iostream>
using namespace std;
PreProcessGestureData::PreProcessGestureData(void):faceDetector(".\\include\\SDK_FD\\model\\VIPLFaceDetector4.0.1.dat")
{
	eps = 5;
	faceX = 0;
	faceY = 0;
	facenum = 0;
	faceDetector.SetMinFaceSize(20);
	faceDetector.SetPerformanceLevel(5);//Լ��Խ���׼�⵽�������5��ԭ����4
	faceDetector.SetSpeedLevel(3);
	//faceDetector.SetView(VIPLFaceView::Frontal);
	faceDetector.SetView(VIPLFaceView::Frontal|VIPLFaceView::Profile);
}

PreProcessGestureData::~PreProcessGestureData(void)
{
	int size = vColor.size();
	for(int i=0;i<vColor.size();i++)
	{
		cvReleaseImage(&vColor[i]);
	}

	for(int i=0;i<vDepth.size();i++)
	{
		cvReleaseImage(&vDepth[i]);
	}
	for(int i = 0; i< size; i++)
	{
		vColor.pop_back();
		vDepth.pop_back();
	}
}

void PreProcessGestureData::ReleaseVector()
{
	int size = vColor.size();
	for(int i=0;i<vColor.size();i++)
	{
		cvReleaseImage(&vColor[i]);
	}
	for(int i=0;i<vDepth.size();i++)
	{
		cvReleaseImage(&vDepth[i]);
	}
	
	for(int i = 0; i< size; i++)
	{
		vColor.pop_back();
		vDepth.pop_back();
	}
}

void PreProcessGestureData::CvtGray(cv::Mat &src, cv::Mat &src_gray)
{
	if (src.channels() == 1)
		src_gray = src;
	else
		cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
}

void PreProcessGestureData::CvtVIPLData(cv::Mat &src, VIPLImageData &srcdata)
{
	srcdata.channels = src.channels();
	srcdata.width = src.cols;
	srcdata.height = src.rows;
	srcdata.data = src.data;
}

bool PreProcessGestureData::IsoOutheadDetectionVIPLSDK(IplImage* colorImage, CString filename, int countframe, int videoid)
{
	
	int confidence = 3;
	Mat src(colorImage);
	cv::Mat src_gray;
	CvtGray(src, src_gray);
	VIPLImageData src_gray_data;
	CvtVIPLData(src_gray, src_gray_data);
	time_t be = clock();//liu�� ����ʱ��
	facenum = faceDetector.Detect(src_gray_data);
	const VIPLFaceInfo *faces = faceDetector.GetDetections(&facenum, float(confidence));
	//��Ƶ������������ô��,��һ����ʶ����Ŷ���ߵ��������洢��FDResult[0]��
	ofstream of;
	if(facenum>0)
	{
		of.open(filename, ios::app);
		if(!of)
		{
			std::cout<<"can not write headfile"<<endl;
		}
		double currScore=0;
		int currentface=-1;
		for (int i = 0; i < facenum; i++)
		{
			if(faces[i].score > currScore)
			{
				currScore=faces[i].score;
				currentface=i;
			}
		}
		facenum=1;
		FDResult[0] = *(faces+currentface);

		faceX = int(FDResult[0].x+FDResult[0].width/2);
		faceY = int(FDResult[0].y+FDResult[0].height/2);
		of<<videoid<<" "<<countframe-1<<" "<<FDResult[0].x<<" "<<FDResult[0].y<<" "<<FDResult[0].x+FDResult[0].width<<" "<<FDResult[0].y+FDResult[0].height<<endl;

		of.close();
		return true;
	}
	else
	{

		faceX = 0;
		faceY = 0;
		
		//of<<countframe<<" "<<x0<<" "<<y0<<endl;
		cout<<"No face"<<endl;
		of.close();
		return false;
	}
	
	
}

bool PreProcessGestureData::readVideo(CString colorPath, CString depthPath)
{
	//�����һ��RGB��Ƶ�ڴ�
	//GetFileFacePosition();//get face position
	MyColorPath = colorPath;
	CvCapture *capture = cvCreateFileCapture(colorPath);
	
  	int numFrames = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT); 
	if(numFrames < 1)
		return false;
	for(int i = 0; i < numFrames; i++)
	{
		IplImage* tempFrame = cvQueryFrame(capture);
		vColor.push_back(cvCloneImage(tempFrame));
	}
	cvReleaseCapture(&capture);

	capture = cvCreateFileCapture(depthPath);
	numFrames = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT); 
	for(int i = 0; i < numFrames; i++)
	{
		IplImage* tempFrame = cvQueryFrame(capture);
		vDepth.push_back(cvCloneImage(tempFrame));
	}
	cvReleaseCapture(&capture);	
	for(int i = 1;i < numFrames;i++)
		Rectify(vDepth[i], vColor[i]);


	//backgroundRemove();
	//CString saveName = colorPath.Right(9);
	//saveName = outputPath + "\\" + saveName;
	//IplImage* saveImage=cvCreateImage(cvSize(vColor[0]->width,vColor[0]->height),vColor[0]->depth,vColor[0]->nChannels);

	//cvNamedWindow("ShowImage",CV_WINDOW_AUTOSIZE);
	//	
	//CvVideoWriter *writer = 0;
	//writer = cvCreateVideoWriter(saveName, CV_FOURCC('M','P','4','2'), 10, cvSize(vColor[0]->width, vColor[0]->height), 1);
	
//	for(int i = 0; i < numFrames; i++)
//	{
////		IplImage* tempColor=cvCloneImage(vColor[i]);
//		IplImage* tempDepth=cvCloneImage(vDepth[i]);
//		cvSetImageROI(saveImage,cvRect(0,0,vColor[0]->width,vColor[0]->height));
//		cvCopyImage(tempDepth,saveImage);
//		cvWriteFrame(writer,saveImage);
//		//cvShowImage("ShowImage",saveImage);
//		//cvWaitKey(100);
//		cvReleaseImage(&tempDepth);
//
//	}
//
//	cvReleaseImage(&saveImage);
//	cvReleaseVideoWriter(&writer);
	return true;
}

void PreProcessGestureData::backgroundRemove()
{
	ofstream MyReadFileStream;
	string filename = "C:\\faceDepthData.txt";
	MyReadFileStream.open(filename, ios::app);

	int numFrames = vColor.size();
	double minDepth = 0;
	double faceDepth = 0;
	int cnt = 0;
	int peak1;
	int peak2;
	int Mypartition;
	int mid = 0;
	int grayMeanFirst;
	bool IsDtectFace = false;
	double DepthThreshold = 30;
	IplImage* grayImg=cvCreateImage(cvSize(vDepth[0]->width,vDepth[0]->height),vDepth[0]->depth,1);
	cvCvtColor(vDepth[mid], grayImg, CV_RGB2GRAY);
	GetHistAndPartition(grayImg, peak1, peak2, Mypartition, grayMeanFirst);

	for(int i = 0; i < numFrames; i++)
	{
		headDetectionVIPLSDK(vColor[i]);
		IplImage* tempImg = cvCreateImage(cvSize(vDepth[i]->width,vDepth[i]->height),vDepth[i]->depth,1);
		cvCvtColor(vDepth[i], tempImg, CV_RGB2GRAY);
		if(facenum > 0)
		{
			cnt++;
			int x0=int(FDResult[0].x + FDResult[0].width/2);
			int y0=int(FDResult[0].y + FDResult[0].height/2);
			MyReadFileStream<<MyColorPath<<" X0 = "<<x0<<" Y0 = "<<y0<<endl;
			//��Χ5*5�Ŀ�
			int beginx = max(0, x0 - 2);
			int beginy = max(0, y0 - 2);
			int endx = min(vColor[i]->width, x0 + 2);
			int endy = min(vColor[i]->height, y0 + 2);
			for(int k = beginx; k <= endx; k++)
				for(int t = beginy; t <= endy; t++)
				{
					int temp=(int)((uchar*)(tempImg->imageData + t * tempImg->widthStep))[k];
					faceDepth +=  temp;
				}

			faceDepth = faceDepth / 25;
			//���������������������������<eps����������޳��������,�����������Ļ���ֱ����hist�ķ���.
   			break;			
		}
		cvReleaseImage(&tempImg);
	}
	if(cnt == 0)//The prpogram can not detect the face,use the hist 
	{
		IsDtectFace = false;
		minDepth = Mypartition;

	}
	else
	{
		IsDtectFace = true;
		if(faceDepth < 50)
			DepthThreshold = 50;
		else if(faceDepth < 100)
			DepthThreshold = 40;
		else if(faceDepth < 150)
			DepthThreshold = 35;
		else if(faceDepth < 200)
			DepthThreshold = - 0.4 * faceDepth + 100;
		else 
			DepthThreshold = - faceDepth / 3 + 260 / 3;

		std::cout<<"faceDepth="<<faceDepth<<"  DepthThreshold="<<DepthThreshold<<endl;

		minDepth = faceDepth + DepthThreshold;
		MyReadFileStream<<"faceDepth = "<<faceDepth<<"  DepthThreshold = "<<DepthThreshold<<" minDepth = "<<minDepth<<endl;
		MyReadFileStream<<"peak1 = "<<peak1<<" peak2 = "<<peak2<<endl;
		if(minDepth < peak1 || minDepth > peak2)
		{

			minDepth = Mypartition;
		}
		else
		{
			int a1 = minDepth - peak1;
			int a2 = peak2 - minDepth;
			if(a2 >= 4*a1 || a1 >= 4*a2)
				minDepth = Mypartition;
		}
	}
	for(int k = 0; k < numFrames; k++)
	{
		double newminDepth = 0;
		if(IsDtectFace)//�����⵽�����Ļ��������ƽ���Ҷȱ仯������ֵ,
		{
			cvCvtColor(vDepth[k], grayImg, CV_RGB2GRAY);
			int grayMean;
			GetImageGrayMean(grayImg, grayMean);
			int changeGray = grayMeanFirst - grayMean;//
			changeGray = min(changeGray, 10);//changeGray�仯̫��������ĳЩ�����г������Ϊ10��������Ϊ��Ӧ�Ա���depth�ı仯������Ӧ����
			std::cout<<"cahnge = "<<changeGray<<endl;
			
			if(faceDepth < 200)
				newminDepth = minDepth - changeGray;
			else
				newminDepth = minDepth;
			if(newminDepth < faceDepth)
				newminDepth = faceDepth + DepthThreshold;//��������������
		}
		else
		{
			newminDepth = minDepth;
		}
		IplImage* tempImg = cvCreateImage(cvSize(vDepth[k]->width, vDepth[k]->height), vDepth[k]->depth, 1);
		cvCvtColor(vDepth[k], tempImg, CV_RGB2GRAY);
		for(int i = 0; i < vDepth[k]->height; i++)
			for(int j = 0; j < vDepth[k]->width; j++)
			{
				
				//�����ǵĴ����������minDepth + 30��Ϊ��������ô����ֵ����ɫ���򶼻���Ϊǰ���ˣ�֮��Ľ�����Ӧ��ֵ
				int tmp = ((uchar*)(tempImg->imageData+i*tempImg->widthStep))[j];
				if(tmp > newminDepth)
				{
					((uchar*)(vDepth[k]->imageData + i * vDepth[k]->widthStep))[j * 3] = 0;
					((uchar*)(vDepth[k]->imageData + i * vDepth[k]->widthStep))[j * 3 + 1] = 0;
					((uchar*)(vDepth[k]->imageData + i * vDepth[k]->widthStep))[j * 3 + 2 ] = 0;
				}
			}
		//cvCloneImage���������ڴ�
		IplImage* oneVedioImage = cvCloneImage(vDepth[k]);
		IplConvKernel *strel = cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_CROSS);
		//cvShowImage("ShowImage", vDepth[k]);
		//cvWaitKey();
		
		cvErode(oneVedioImage, oneVedioImage, strel, 1);

		cvDilate(oneVedioImage, oneVedioImage, strel, 1);

		cvDilate(oneVedioImage, oneVedioImage, strel, 1);
		cvDilate(oneVedioImage, oneVedioImage, strel, 1);
		cvDilate(oneVedioImage, oneVedioImage, strel, 1);
		//cvDilate(oneVedioImage, oneVedioImage, strel, 1);
		//cvShowImage("ShowImage", vDepth[k]);
		//cvWaitKey();
		cvCvtColor(oneVedioImage, tempImg, CV_RGB2GRAY);
		IplImage * dst = cvCreateImage(cvSize(vDepth[k]->width, vDepth[k]->height), vDepth[k]->depth, 1);

		GetMaxConnectedDomain(oneVedioImage, dst);//
		for(int i = 0; i < oneVedioImage->height; i++)
			for(int j = 0; j < oneVedioImage->width; j++)
			{
				if(((uchar*)(dst->imageData + i * dst->widthStep))[j] < 200)//����ͨ����
				{
					((uchar*)(oneVedioImage->imageData + i * oneVedioImage->widthStep))[j * 3] = 0;
					((uchar*)(oneVedioImage->imageData + i * oneVedioImage->widthStep))[j * 3 + 1] = 0;
					((uchar*)(oneVedioImage->imageData + i * oneVedioImage->widthStep))[j * 3 + 2 ] = 0;
				}

			}
		cvErode(oneVedioImage, oneVedioImage, strel, 1);
		cvErode(oneVedioImage, oneVedioImage, strel, 1); 
		cvErode(oneVedioImage, oneVedioImage, strel, 1);     
		cvCvtColor(oneVedioImage, tempImg, CV_RGB2GRAY);
		for(int i = 0; i < oneVedioImage->height; i++)
			for(int j = 0; j < oneVedioImage->width; j++)
			{
				int tmp = ((uchar*)(tempImg->imageData + i * tempImg->widthStep))[j];
				if(tmp == 0)
				{
					((uchar*)(vDepth[k]->imageData + i * vDepth[k]->widthStep))[j * 3] = 0;
					((uchar*)(vDepth[k]->imageData + i * vDepth[k]->widthStep))[j * 3 + 1] = 0;
					((uchar*)(vDepth[k]->imageData + i * vDepth[k]->widthStep))[j * 3 + 2 ] = 0;
				}
				      
			}
		cvErode(vDepth[k], vDepth[k], strel, 1);
		cvDilate(vDepth[k], vDepth[k], strel, 1);

		//cvShowImage("ShowImage1", vDepth[k]);
		//cvWaitKey(0);
		cvReleaseImage(&tempImg);
		cvReleaseImage(&dst);
		cvReleaseImage(&oneVedioImage);
		//cout<<k<<"/"<<numFrames<<endl;
	}
	cvReleaseImage(&grayImg);
	//cvReleaseVideoWriter(&TestWriter);
}

void PreProcessGestureData::Rectify(IplImage* bgrFrameL, IplImage* bgrFrameR)
{
        double R1[3][3],R2[3][3],P1[3][4],P2[3][4];

        CvSize imageSize={0,0};
		//ʦ���������Ķ������
		double M1[3][3]={597.27, 0, 322.67, 0, 597.04, 232.64, 0, 0, 1};
        double M2[3][3]={529.45, 0, 319.35, 0, 530.51, 234.79, 0,0, 1};
        
        /*double D1[5]={3.71403933e-01, -1.29047451e+01, 0, 0, 1.56843994e+02};
        double D2[5]={4.39538926e-01, -1.70257187e+01, 0, 0, 2.23054718e+02};*/

		double D1[5]={0, 0, 0, 0, 0};
		double D2[5]={0, 0, 0, 0, 0};
        //double D2[5]={0.1884,-0.4865,0,-0.003,0.0007};

        double R[3][3]={0.9999,-0.0106,    0.0049,  0.0106,   0.9999,   0.0034, -0.0049,   -0.0034 ,  0.9999};
        double T[3]={25.0479, 0.2849, -2.0667};
        CvMat CvM1=cvMat(3,3,CV_64F,M1);
        CvMat CvM2=cvMat(3,3,CV_64F,M2);
        CvMat _D1=cvMat(1,5,CV_64F,D1);
        CvMat _D2=cvMat(1,5,CV_64F,D2);
        CvMat _R=cvMat(3,3,CV_64F,R);
        CvMat _T=cvMat(3,1,CV_64F,T);
        CvMat _R1=cvMat(3,3,CV_64F,R1);
        CvMat _R2=cvMat(3,3,CV_64F,R2);
        CvMat _P1=cvMat(3,4,CV_64F,P1);
        CvMat _P2=cvMat(3,4,CV_64F,P2);

        imageSize=cvGetSize(bgrFrameL);
        cvStereoRectify(&CvM1,&CvM2,&_D1,&_D2,imageSize,&_R,&_T,&_R1,&_R2,&_P1,&_P2,0,0);

        CvMat *mx1=cvCreateMat(imageSize.height,imageSize.width,CV_32F);
        CvMat *my1=cvCreateMat(imageSize.height,imageSize.width,CV_32F);
        CvMat *mx2=cvCreateMat(imageSize.height,imageSize.width,CV_32F);
        CvMat *my2=cvCreateMat(imageSize.height,imageSize.width,CV_32F);
        cvInitUndistortRectifyMap(&CvM1,&_D1,&_R1,&_P1,mx1,my1);
        cvInitUndistortRectifyMap(&CvM2,&_D2,&_R2,&_P2,mx2,my2);

        //IplImage *img1r=cvCreateImage(imageSize,8,3);
		//IplImage *img2r=cvCreateImage(imageSize,8,3);
        cvRemap(bgrFrameL,bgrFrameL,mx1,my1);
        cvRemap(bgrFrameR,bgrFrameR,mx2,my2);
		
		cvReleaseMat(&mx1);
		cvReleaseMat(&my1);
		cvReleaseMat(&mx2);
		cvReleaseMat(&my2);


  //      cvNamedWindow("1",CV_WINDOW_AUTOSIZE); 
  //      cvShowImage("1",img1r);  
  //      cvNamedWindow("2",CV_WINDOW_AUTOSIZE); 
  //      cvShowImage("2",img2r);  

  //	  cvNamedWindow("3",CV_WINDOW_AUTOSIZE); 
  //      cvShowImage("3",bgrFrameL);  
  //      cvNamedWindow("4",CV_WINDOW_AUTOSIZE); 
  //      cvShowImage("4",bgrFrameR);  
  //      waitKey();
}

/*
������������������� ��ƽ��ֵ,����image Ϊ����֮������ǰ���Ҷ�ͼ�񣬿�������Ƶ����֡���������ĵ�ƽ��ֵ��Ϊ�������ĵĹ���ֵ
*/
void PreProcessGestureData::GetFacePosiFromHist(IplImage *image, int &x, int &y)
{
	CvSize sz = cvGetSize(image);
	bool Loop1 = true;

	for(int i = 0; i < sz.height; i++)
	{
		if(!Loop1)
			break;
		for(int j = 0; j < sz.width; j++)
		{
			int tmp = ((uchar *)(image->imageData + i * image->widthStep))[j];
			if(tmp >= 6)//��ǰ��
			{
				int k;
				for(k = j; k < sz.width; k++)
				{
					int tmp1 = ((uchar *)(image->imageData + i * image->widthStep))[k];
					if(tmp1 == 0)//����
						break;
				}
				y = i;
				x = (j + k) / 2;
				Loop1 = false;
				break;
			}


		}
	}
	y = y + 40;//����ߵ�����30������
}

void PreProcessGestureData::getISoFacePositionHist(CString filename, int countframe, int videoid)
{
	IplImage* tempImg = cvCreateImage(cvSize(vDepth[0]->width, vDepth[0]->height), vDepth[0]->depth, 1);
	backgroundRemove();
	cvCvtColor(vDepth[0], tempImg, CV_RGB2GRAY);
	GetFacePosiFromHist(tempImg, faceX, faceY);
	ofstream of;
	of.open(filename, ios::app);
	of<<videoid<<" "<<countframe<<" "<<faceX<<" "<<faceY<<endl;
	of.close();
	cvReleaseImage(&tempImg);
}

//�õ�ͳ�ƻҶ�ֱ��ͼ������֮��ȵ����ָ��
void PreProcessGestureData::GetHistAndPartition(IplImage *image, int &peak1, int &peak2, int &Mypartition,int &grayMean)
{

	if(image->nChannels > 1)
		cvCvtColor(image, image, CV_BGR2GRAY);  
	CvSize sz = cvGetSize(image);

	int hist[256];
	int p1[256];//��һ����ֵ�ֲ�ֱ��ͼ
	int p2[256];//�ڶ�����ֵ�ֲ�ֱ��ͼ
	int scale_image = 256;
	for(int i = 0; i < scale_image; i++)
		hist[i] = 0;
	for(int i = 0; i < sz.height; i++)
		for(int j = 0; j < sz.width; j++)
		{
			int tmp = ((uchar *)(image->imageData + i * image->widthStep))[j];
			hist[tmp] ++;
		}
		//�Ҷ�ֵС�ڵ���eps�Ĳ�������ͳ���ڣ��Ѿ���Ϊ����
		double graySum = 0;
		double grayCount = 0;
		for(int i = eps + 1; i < scale_image; i++)
		{
			graySum += hist[i] * i;
			grayCount += hist[i];
		}
		grayMean = (int)(graySum / grayCount);
		double Mymax = -1;
		for(int i = eps + 1; i <= grayMean; i++)
		{
			if(Mymax < hist[i])
			{
				peak1 = i;
				Mymax = hist[i];
			}

		}
		Mymax = -1;
		for(int i = grayMean + 1; i < scale_image; i++)
		{
			if(Mymax < hist[i])
			{
				peak2 = i;
				Mymax = hist[i];
			}
		}
		Mypartition = (peak1 + peak2) / 2;
}

bool PreProcessGestureData::headDetectionVIPLSDK(IplImage* colorImage)
{

	int confidence=3;//ԽСԽ�ã��������Խ�࣬ԭ����11
	Mat src(colorImage);
	cv::Mat src_gray;
	CvtGray(src, src_gray);
	VIPLImageData src_gray_data;
	CvtVIPLData(src_gray, src_gray_data);
	time_t be = clock();
	facenum = faceDetector.Detect(src_gray_data);
	const VIPLFaceInfo *faces = faceDetector.GetDetections(&facenum, float(confidence));
	//��Ƶ������������ô��,��һ����ʶ����Ŷ���ߵ��������洢��FDResult[0]��
	if(facenum>0)
	{
		double currScore=0;
		int currentface=-1;
		for (int i = 0; i < facenum; i++)
		{
			if(faces[i].score > currScore)
			{
				currScore=faces[i].score;
				currentface=i;
			}
		}
		facenum=1;
		FDResult[0] = *(faces+currentface);
	}
	if (facenum == 0)
		return false;
	return true;
}

/*
input:src the original image(RGB or grey),dst is MaxConnectedDomain image
function:The function can get a image max connected domain
*/
void PreProcessGestureData::GetMaxConnectedDomain(IplImage *src, IplImage *dst)
{
	if(src->nChannels > 1)
		cvCvtColor(src, dst, CV_BGR2GRAY);  
	else
		dst = cvCloneImage(src);
	//   cvNamedWindow("�Ҷ�ͼ��");  
	//   cvShowImage("�Ҷ�ͼ��", dst);  
	//cvWaitKey();
	//cvThreshold(dst, dst, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);//OTSU��ֵ��   
	cvThreshold(dst, dst, 5, 255.0, CV_THRESH_BINARY);//eps = 5;
	IplConvKernel *element = cvCreateStructuringElementEx(5, 5, 0, 0, CV_SHAPE_ELLIPSE);  
	cvMorphologyEx(dst, dst, NULL, element, CV_MOP_OPEN);//�����㣬ȥ���ȽṹԪ��С�ĵ�     
	cvReleaseStructuringElement(&element);  
	//   cvNamedWindow("��ֵͼ��");  
	//   cvShowImage("��ֵͼ��", dst);  
	//cvWaitKey();
	int w,h;  
	CvSize sz = cvGetSize(dst);  
	int color = 254; // ����0����,������Ϊ255,����254     
	for (w = 0; w < sz.width; w++)    
	{    
		for (h = 0; h < sz.height; h++)    
		{    
			if (color > 0)    
			{    
				if (CV_IMAGE_ELEM(dst, unsigned char, h, w) == 255)    
				{    
					// ����ͨ��������ɫ     
					cvFloodFill(dst, cvPoint(w, h), CV_RGB(color, color, color));  
					color--;  
				}    
			}    
		}    
	}    
	//cvNamedWindow("�����ɫ���ͼ��");  
	//   cvShowImage("�����ɫ���ͼ��", dst);  
	//cvWaitKey();
	int colorsum[255] = {0};  
	for (w=0; w<sz.width; w++)    
	{    
		for (h=0; h<sz.height; h++)    
		{    
			if (CV_IMAGE_ELEM(dst, unsigned char, h, w) > 0)    
			{    
				colorsum[CV_IMAGE_ELEM(dst, unsigned char, h, w)]++;//ͳ��ÿ����ɫ������     
			}    
		}    
	}    
	std::vector<int> v1(colorsum, colorsum+255);//�������ʼ��vector     
	//������������Ⱦɫ��ע��max_element��ʹ�÷���     
	int maxcolorsum = max_element(v1.begin(), v1.end()) - v1.begin();  
	//printf("%d\n",maxcolorsum);  

	for (w=0; w<sz.width; w++)    
	{    
		for (h=0; h<sz.height; h++)    
		{    
			if (CV_IMAGE_ELEM(dst, unsigned char, h, w) == maxcolorsum)    
			{    
				CV_IMAGE_ELEM(dst, unsigned char, h, w) = 255;  
			}    
			else    
			{    
				CV_IMAGE_ELEM(dst, unsigned char, h, w) = 0;  
			}    
		}    
	}    
	//   cvNamedWindow("�����ͨ��ͼ");  
	//   cvShowImage("�����ͨ��ͼ", dst);  
	//cvWaitKey();  
	//   cvDestroyAllWindows();  
	return ;
}

//image �ǻҶ�ͼ��
void  PreProcessGestureData::GetImageGrayMean(IplImage *image, int &grayMean)
{


	CvSize sz = cvGetSize(image);

	int hist[256];
	int scale_image = 256;
	for(int i = 0; i < scale_image; i++)
		hist[i] = 0;
	for(int i = 0; i < sz.height; i++)
		for(int j = 0; j < sz.width; j++)
		{
			int tmp = ((uchar *)(image->imageData + i * image->widthStep))[j];
			hist[tmp] ++;
		}
		//�Ҷ�ֵС�ڵ���eps�Ĳ�������ͳ���ڣ��Ѿ���Ϊ����
		double graySum = 0;
		double grayCount = 0;
		for(int i = eps + 1; i < scale_image; i++)
		{
			graySum += hist[i] * i;
			grayCount += hist[i];
		}
		grayMean = (int)(graySum / grayCount);
}