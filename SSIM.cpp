#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

Scalar getMSSIM(Mat inputimg1, Mat inputimg2);
void video2img_SSIM(const char * videofile, double ssim_threshold);
void video2img_STEP(const char * videofile, int step);

int main(int argc, char * argv[])
{
	char * videofile;
	double ssim_threshold = 0.9;
	if(argc >= 3)
	{
		strcpy(videofile,argv[1]); 
		sscanf(argv[2],"%lf",&ssim_threshold);
	}
	video2img_SSIM(videofile, ssim_threshold);
	//video2img_STEP("fire1.avi", 10);
/****test ssim****
	Mat blurimg1;
	Mat blurimg2;
	Mat srcimg = imread("10.jpg");
	//blurimg1 = imread("30.jpg");
	blur(srcimg, blurimg1, Size(5,5));
	printf("blur has been done!\n");
	Scalar MSSIM = getMSSIM(srcimg, blurimg1);
	printf("getMSSIM done!\n");
	double SSIM = (MSSIM.val[2] + MSSIM.val[1] + MSSIM.val[0]) / 3 * 100;
	printf("channel1 ssim is %f \n", MSSIM.val[0] * 100);
	printf("channel2 ssim is %f \n", MSSIM.val[1] * 100);
	printf("channwl3 ssim is %f \n", MSSIM.val[2] * 100);
	printf("avetage ssim is %f \n", SSIM);
	imshow("srcimg", srcimg);
	imshow("blur5*5", blurimg1);
******************/
	waitKey(0);
	return 0;
}

Scalar getMSSIM(Mat inputimg1, Mat inputimg2)
{
	Mat i1 = inputimg1;
	Mat i2 = inputimg2;
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I1_2 = I1.mul(I1);
	Mat I2_2 = I2.mul(I2);
	Mat I1_I2 = I1.mul(I2);
	
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11,11), 1.5);
	GaussianBlur(I2, mu2, Size(11,11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11,11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11,11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11,11), 1.5);
	sigma12 -= mu1_mu2;
	
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);
	
	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);
	return mssim;
}

void video2img_SSIM(const char * videofile, double ssim_threshold)	//video to images by ssim threshold, arg1: the name of video; arg2: the ssim threshold 
{
	double ssim = 0;
	VideoCapture cap(videofile);
	
	long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
	printf("total frame number is %ld \n", totalFrameNumber);
	
	Mat frame, reference;
	bool flags = true;
	long currentFrames = 0;
	Scalar MSSIM;
	double SSIM;

	while (flags)
	{
		stringstream Path;
		cap.read(frame);
		
		Path << "./output/" << videofile << currentFrames << ".jpg";
		printf("current frame number: %ld \n", currentFrames);


		if (currentFrames == 0)
		{
			imwrite(Path.str(), frame);
			reference = frame.clone();
		}
		else
		{
			MSSIM = getMSSIM(frame, reference);
			SSIM = (MSSIM.val[2] + MSSIM.val[1] + MSSIM.val[0]) / 3;
			printf("the ssim is %f \n", SSIM);
			if (SSIM <= ssim_threshold)
			{
				imwrite(Path.str(), frame);
				reference = frame.clone();
			}
		}

		currentFrames++;
		if (currentFrames >= totalFrameNumber)
		{
			flags = false;
		}
		
	}
	printf("end of the loop! \n");
}

void video2img_STEP(const char * videofile, int step)	//video to images by step, arg1: the name of video; arg2: the step 
{
	double ssim = 0;
	VideoCapture cap(videofile);
	
	long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
	printf("total frame number is %ld \n", totalFrameNumber);
	
	Mat frame, reference;
	bool flags = true;
	long currentFrames = 0;
	while (flags)
	{
		stringstream Path;
		cap.read(frame);
		reference = frame;
		Path << "./fire/" << currentFrames << ".jpg";
		printf("current frame number: %ld \n", currentFrames);
		
		if (currentFrames % step == 0)
		{
			imwrite(Path.str(), frame);
			reference = frame;
		}
		currentFrames++;
		if(currentFrames >= totalFrameNumber)
		{
			flags = false;
		}
		
	}
}
