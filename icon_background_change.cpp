#include <opencv2/opencv.hpp> 
//#include <opencv2/xfeatures2d.hpp> 
//#include<opencv2/face.hpp> 
#include<iostream> 
#include<math.h> 
#include <string> 
#include<fstream> 
//using namespace cv::face; 
using namespace cv; 
using namespace std; 
//using namespace cv::xfeatures2d; 
int main() 
{ 
	Mat src = imread("a.jpg");
	imshow("src", src); //��װ���� 
	int width = src.cols; 
	int height = src.rows; 
	int samplecount = width * height; 
	int dims = src.channels(); //����Ϊsrc�����ص���������Ϊͨ������ÿ�����ݷֱ�Ϊsrc��bgr�����ϵ��� ������˳������� 
	Mat points(samplecount, dims, CV_32F, Scalar(10)); 
	int ind = 0; 
	for (int row = 0; row < height; row++) 
	{ 
		for (int col = 0; col < width; col++) 
		{ 
			ind = row * width + col;// 
			Vec3b bgr = src.at<Vec3b>(row, col); 
			points.at<float>(ind, 0) = static_cast<int>(bgr[0]);
			points.at<float>(ind, 1) = static_cast<int>(bgr[1]); 
			points.at<float>(ind, 2) = static_cast<int>(bgr[2]); 
		} 
	} 
	//����kmeans 
	int numCluster = 4; 
	Mat labels; Mat centers; 
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 0.1); 
	kmeans(points, numCluster, labels, criteria, 3, KMEANS_PP_CENTERS, centers); 
	//ȥ����+�������� 
	Mat mask = Mat::zeros(src.size(), CV_8UC1); 
	int index = src.rows * 2 + 2;//��ȡ��Ե�����ϵ㣬���￿2��λ�� 
	int cindex = labels.at<int>(index, 0);
	int height1 = src.rows; 
	int width1 = src.cols; 
	Mat dst;
	//�˵�������Χ����һЩ�ӵ㣬������Ҫ��ʴ�͸�˹ģ��ȡ���� 
	src.copyTo(dst); 
	for (int row = 0; row < height1; row++) 
	{ 
		for (int col = 0; col < width1; col++)
		{ 
			index = row * width1 + col; 
			int label = labels.at<int>(index, 0);
			if (label == cindex) 
			{ 
				dst.at<Vec3b>(row, col)[0] = 0; 
				dst.at<Vec3b>(row, col)[1] = 0;
				dst.at<Vec3b>(row, col)[2] = 0;
				mask.at<uchar>(row, col) = 0; 
			} 
			else 
			{ 
				dst.at<Vec3b>(row, col) = src.at<Vec3b>(row, col);
				mask.at<uchar>(row, col) = 255;
				//����������Ϊ��ɫ���Ա�������ĸ�ʴ���˹ģ�� 
			}
		} 
	} imshow("dst", dst);
	imshow("mask", dst); 
	//��ʴ+��˹ģ��
	Mat k = getStructuringElement(MORPH_RECT, Size(3, 3)); 
	erode(mask, mask, k); 
	GaussianBlur(mask, mask, Size(3, 3), 0, 0); 
	imshow("gaosimohu", mask); 
	//ͨ�����
	RNG rng(12345); 
	Vec3b color; 
	color[0] = 0;//rng.uniform(0, 255);
	color[1] =0;//rng.uniform(0, 255); 
	color[2] =255;//rng.uniform(0, 255);
	Mat result(src.size(), src.type()); 
	double w = 0.0; 
	int b = 0, g = 0, r = 0;
	int b1 = 0, g1 = 0, r1 = 0;
	int b2 = 0, g2 = 0, r2 = 0; 
	double time = getTickCount(); 
	for (int row = 0; row < height1; row++) 
	{
		for (int col = 0; col < width; col++)
		{ 
			int m = mask.at<uchar>(row, col);
			if (m == 255) 
			{ 
				result.at<Vec3b>(row, col) = src.at<Vec3b>(row, col);//ǰ�� 
			} 
			else if (m == 0) 
			{ 
				result.at<Vec3b>(row, col) = color; // ���� 
			}
			else 
			{//��Ϊ��˹ģ���Ĺ�ϵ������maskԪ�ص���ɫ���˺ڰ�ɫ���кڰױ�Ե����ģ����ķǺڰ�ֵ 
				w = m / 255.0; 
				b1 = src.at<Vec3b>(row, col)[0];
				g1 = src.at<Vec3b>(row, col)[1];
				r1 = src.at<Vec3b>(row, col)[2]; 
				b2 = color[0]; 
				g2 = color[0]; 
				r2 = color[0]; 
				b = b1 * w + b2 * (1.0 - w); 
				g = g1 * w + g2 * (1.0 - w); 
				r = r1 * w + r2 * (1.0 - w);
				result.at<Vec3b>(row, col)[0] = b;//���ձ�Ե��ɫֵ
				result.at<Vec3b>(row, col)[1] = g; 
				result.at<Vec3b>(row, col)[2] = r;
			} 
		} 
	} 
	cout << "time=" << (getTickCount() - time) / getTickFrequency() << endl; 
	imshow("backgroud repalce", result); 
	//imwrite("replace_background.jpg", result);
	waitKey(0);
 }
