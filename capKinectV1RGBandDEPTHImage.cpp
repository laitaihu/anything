#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	Mat color_img, depth_img;
	//这里我们用灰度图来表述深度数据，越远的数据越暗。
	depth_img.create(240, 320, CV_8UC1);
	color_img.create(240, 320, CV_8UC3);
	//1、初始化NUI，注意：这里传入的参数就不一样了，是DEPTH
	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH| NUI_INITIALIZE_FLAG_USES_COLOR);
	HRESULT color_hr = hr; //NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR);//只能初始化一个NuiInitialize句柄
	if (FAILED(hr))
	{
		cout << "NuiInitialize failed" << endl;
		return hr;
	}

	//2、定义事件句柄 
	//创建读取下一帧的信号事件句柄，控制KINECT是否可以开始读取下一帧数据
	HANDLE nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE depthStreamHandle = NULL; //保存图像数据流的句柄，用以提取数据

	HANDLE nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE colorStreamHandle = NULL;

	//3、打开KINECT设备的深度图信息通道，并用depthStreamHandle保存该流的句柄，以便于以后读取
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_320x240,
		0, 2, nextDepthFrameEvent, &depthStreamHandle);
	if (FAILED(hr))//判断是否提取正确 
	{
		cout << "Could not open depth depth_img stream video" << endl;
		NuiShutdown();
		return hr;
	}
	color_hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_320x240,
		0, 2, nextColorFrameEvent, &colorStreamHandle);
	if (FAILED(color_hr))//判断是否提取正确 
	{
		cout << "Could not open color color_img stream video" << endl;
		NuiShutdown();
		return hr;
	}
	namedWindow("depthImage", CV_WINDOW_AUTOSIZE);
	namedWindow("color_img",CV_WINDOW_AUTOSIZE);
	//4、开始读取深度数据 
	while (1)
	{
		const NUI_IMAGE_FRAME * pImageFrame = NULL;
		const NUI_IMAGE_FRAME * color_pImageFrame = NULL;
		//4.1、无限等待新的数据，等到后返回
		if (WaitForSingleObject(nextDepthFrameEvent, INFINITE) == 0)
		{
			//4.2、从刚才打开数据流的流句柄中得到该帧数据，读取到的数据地址存于pImageFrame
			hr = NuiImageStreamGetNextFrame(depthStreamHandle, 0, &pImageFrame);
			
			if (FAILED(hr))
			{
				cout << "Could not get depth depth_img" << endl;
				NuiShutdown();
				return -1;
			}

			INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
			NUI_LOCKED_RECT LockedRect;

			//4.3、提取数据帧到LockedRect，它包括两个数据对象：pitch每行字节数，pBits第一个字节地址
			//并锁定数据，这样当我们读数据的时候，kinect就不会去修改它
			pTexture->LockRect(0, &LockedRect, NULL, 0);

			//4.4、确认获得的数据是否有效
			if (LockedRect.Pitch != 0)
			{
				//4.5、将数据转换为OpenCV的Mat格式
				for (int i = 0; i<depth_img.rows; i++)
				{
					uchar *ptr = depth_img.ptr<uchar>(i);  //第i行的指针
													   //深度图像数据含有两种格式，这里像素的低12位表示一个深度值，高4位未使用；
													   //注意这里需要转换，因为每个数据是2个字节，存储的同上面的颜色信息不一样，
					uchar *pBufferRun = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
					USHORT * pBuffer = (USHORT*)pBufferRun;
					for (int j = 0; j<depth_img.cols; j++)
					{

						ptr[(depth_img.cols-1)*depth_img.channels() - j] =  (BYTE)(256 * pBuffer[j] / 0x0fff);  //直接将数据归一化处理
					}
				}
				//cout << "均值： " << (sum/a++) << endl;
				imshow("depthImage", depth_img); //显示图像 
			}
			else
			{
				cout << "Buffer length of received texture is bogus\r\n" << endl;
			}
			//5、这帧已经处理完了，所以将其解锁
			pTexture->UnlockRect(0);
			//6、释放本帧数据，准备迎接下一帧 
			NuiImageStreamReleaseFrame(depthStreamHandle, pImageFrame);
		}

		if (WaitForSingleObject(nextColorFrameEvent, INFINITE) == 0) {
			color_hr = NuiImageStreamGetNextFrame(colorStreamHandle, 0, &color_pImageFrame);
			if (FAILED(color_hr))
			{
				cout << "Could not get color depth_img" << endl;
				NuiShutdown();
				return -1;
			}
			INuiFrameTexture * colorpTexture = color_pImageFrame->pFrameTexture;
			NUI_LOCKED_RECT color_LockedRect;

			colorpTexture->LockRect(0, &color_LockedRect, NULL, 0);

			if (color_LockedRect.Pitch != 0) { //4.5、将数据转换为OpenCV的Mat格式 
				for (int i = 0; i < color_img.rows; i++) {
					uchar *ptr = color_img.ptr<uchar>(i);
					//第i行的指针 //每个字节代表一个颜色信息，直接使用uchar 
					uchar *pBuffer = (uchar*)(color_LockedRect.pBits) + i * color_LockedRect.Pitch;
					for (int j = 0; j < color_img.cols; j++) {
						ptr[(color_img.cols - 1) * color_img.channels() - 3 * j    ] = pBuffer[4 * j]; //内部数据是4个字节，0-1-2是BGR，第4个现在未使用 
						ptr[(color_img.cols - 1) * color_img.channels() - 3 * j + 1] = pBuffer[4 * j + 1];
						ptr[(color_img.cols - 1) * color_img.channels() - 3 * j + 2] = pBuffer[4 * j + 2];
					}
				}
				imshow("colorImage", color_img); //显示图像 
			}
			else {
				cout << "Buffer length of received texture is bogus\r\n" << endl;
			}
			colorpTexture->UnlockRect(0);
			NuiImageStreamReleaseFrame(colorStreamHandle, color_pImageFrame);
		}

		if (cvWaitKey(20) == 27) {
			
			break;
		}		
	}
	//7、关闭NUI链接 
	NuiShutdown();
	//system("pause");
	return 0;
}