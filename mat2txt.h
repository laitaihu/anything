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
				
				ofstream file(name, ios::out);	//打开一个文件 	
				for (int nrows = 0; nrows < img.rows; nrows++)  //行遍历 	
				{
					float* aa = img.ptr<float>(nrows);
					for (int ncols = 0; ncols < img.cols; ncols++)  //列遍历 		
					{
						//Vec3b RGB_v = imgf.at<float>(nrows,ncols);	//用Vec3b也行，获得（x,y）处的像素值 			
						//cout << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //打印数据到控制台 			
						//file << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //输出数据到test.txt文件 
						file << "[" << aa[ncols * 3] << "," << aa[ncols * 3 + 1] << "," << aa[ncols * 3 + 2] << "]";
						//cout << "[" << aa[ncols * 3] << "," << aa[ncols * 3 + 1] << "," << aa[ncols * 3 + 2] << "]";
					}
					//cout << endl;	//输出一行像素值后换行 		
					file << endl;
				}
				file.close();				//关闭文件 	
				//cout << endl;  
				cout << "三通道存储成功";
			}break;
		case(1):
			{
				ofstream file(name, ios::out);	//打开一个文件 	
				for (int nrows = 0; nrows < img.rows; nrows++)  //行遍历 	
				{
					
					for (int ncols = 0; ncols < img.cols; ncols++)  //列遍历 		
					{
						//Vec3b RGB_v = imgf.at<float>(nrows,ncols);	//用Vec3b也行，获得（x,y）处的像素值 			
						//cout << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //打印数据到控制台 			
						//file << "("<<RGB_v.val[0]<<"," <<RGB_v.val[1]<<"," <<RGB_v.val[2]<<")";  //输出数据到test.txt文件 
						float aa = img.at<float>(nrows,ncols);
						file << "[" << aa << "]";
						//cout << "[" << aa[ncols * 3] << "," << aa[ncols * 3 + 1] << "," << aa[ncols * 3 + 2] << "]";
					}
					//cout << endl;	//输出一行像素值后换行 		
					file << endl;
				}
				file.close();				//关闭文件 	
				//cout << endl;  
				cout << "单通道存储成功";

			}break;
		default:break;
		}
	}
};

#endif