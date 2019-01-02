#pragma once
#ifndef MATT2TXT_H
#define MAT2TXT_H
#include "opencv2/opencv.hpp"
#include "iostream"
#include "fstream"
using namespace cv;
using namespace std;
class Store_mat2txt
{
public:
	static void storetxt(Mat  img, String  name)
	{
		const int chan = img.channels();
		switch(chan)
		{
		case(3):
			{
				
				ofstream file(name, ios::out);	//��һ���ļ� 	
				for (int nrows = 0; nrows < img.rows; nrows++)  //�б��� 	
				{
					float* aa = img.ptr<float>(nrows);
					for (int ncols = 0; ncols < img.cols; ncols++)  //�б��� 		
					{
						//Vec3b RGB_v = imgf.at<float>(nrows,ncols);	//��Vec3bҲ�У���ã�x,y����������ֵ 			
						//cout << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //��ӡ���ݵ�����̨ 			
						//file << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //������ݵ�test.txt�ļ� 
						file << "[" << aa[ncols * 3] << "," << aa[ncols * 3 + 1] << "," << aa[ncols * 3 + 2] << "]";
						//cout << "[" << aa[ncols * 3] << "," << aa[ncols * 3 + 1] << "," << aa[ncols * 3 + 2] << "]";
					}
					//cout << endl;	//���һ������ֵ���� 		
					file << endl;
				}
				file.close();				//�ر��ļ� 	
				//cout << endl;  
				cout << "��ͨ���洢�ɹ�";
			}break;
		case(1):
			{
				ofstream file(name, ios::out);	//��һ���ļ� 	
				for (int nrows = 0; nrows < img.rows; nrows++)  //�б��� 	
				{
					
					for (int ncols = 0; ncols < img.cols; ncols++)  //�б��� 		
					{
						//Vec3b RGB_v = imgf.at<float>(nrows,ncols);	//��Vec3bҲ�У���ã�x,y����������ֵ 			
						//cout << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //��ӡ���ݵ�����̨ 			
						//file << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //������ݵ�test.txt�ļ� 
						float aa = img.at<float>(nrows,ncols);
						file << "[" << aa << "]";
						//cout << "[" << aa[ncols * 3] << "," << aa[ncols * 3 + 1] << "," << aa[ncols * 3 + 2] << "]";
					}
					//cout << endl;	//���һ������ֵ���� 		
					file << endl;
				}
				file.close();				//�ر��ļ� 	
				//cout << endl;  
				cout << "��ͨ���洢�ɹ�";

			}break;
		default:break;
		}
	}
};

#endif