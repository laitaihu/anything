#pragma once
#ifndef BGR2HSI_H
#define BGR2HSI_H

#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

#define PI 3.1415
#define min(a,b) (a<b?a:b)

class bgr2hsi
{
public:
	static int convert(Mat image, Mat hsi)
	{
		
	if (!image.data) {
		cout << "Miss Data" << endl;
		return -1;
	}
	int nl = image.rows;
	int nc = image.cols;
	if (image.isContinuous()) {
		nc = nc * nl;
		nl = 1;
	}
	for (int i = 0; i < nl; i++) {
		uchar *src = image.ptr<uchar>(i);
		uchar *dst = hsi.ptr<uchar>(i);
		for (int j = 0; j < nc; j++) {
			float b = src[j * 3] / 255.0;
			float g = src[j * 3 + 1] / 255.0;
			float r = src[j * 3 + 2] / 255.0;
			float num = (float)(0.5*((r - g) + (r - b)));
			float den = (float)sqrt((r - g)*(r - g) + (r - b)*(g - b));
			float H, S, I;
			if (den == 0) {	//��ĸ����Ϊ0
				H = 0;
			}
			else {
				double theta = acos(num / den);
				if (b <= g)
					H = theta / (PI * 2);
				else
					H = (2 * PI - theta) / (2 * PI);
			}
			double minRGB = min(min(r, g), b);
			den = r + g + b;
			if (den == 0)	//��ĸ����Ϊ0
				S = 0;
			else
				S = 1 - 3 * minRGB / den;
			I = den / 3.0;
			//��S������H���������䵽[0,255]�����Ա�����ʾ;
			//һ��H������[0,2pi]֮�䣬S��[0,1]֮��
			dst[3 * j] = H * 255;
			dst[3 * j + 1] = S * 255;
			dst[3 * j + 2] = I * 255;
		}
	}
	return 0;

	}
};

#endif