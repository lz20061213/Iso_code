#include "ImgProcess.h"
#include <iostream>
#include <fstream>

int main()
{
	cv::Mat src = cv::imread("img1.jpg");
	cv::Mat src_nomark = src.clone();
	cv::namedWindow("ss1", 1);
	cv::imshow("ss1", src);
	cv::waitKey(0);

	int facenum = 0;
	ImgProcess ImgProcessor;

	//�����ֱ�Ϊ��С��������̬��performance level ��speed level���������÷�����鿴�ο��ĵ�
	//��̬��0��ʾ׼���棬1��ʾ���棬2 ��ʾ���桢���涼��⣬
	//����ֻ��׼�����SDK��һ������Ӱ�죬����Ϊ0����
	ImgProcessor.SetFDPara(20, 0, 4, 3);


	VIPLFaceInfo FDResult[1024];

	//������⣬���һ������Ϊ���Ŷ�
	ImgProcessor.GetFDResult(src, facenum, FDResult, 11);
	if (facenum == 0)
		return 0;
	ImgProcessor.DrawFDResult(src, facenum, FDResult);
	cv::namedWindow("ss1", 1);
	cv::imshow("ss1", src);
	cv::waitKey(0);

	return 0;
}