#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	Mat color_img, depth_img;
	//���������ûҶ�ͼ������������ݣ�ԽԶ������Խ����
	depth_img.create(240, 320, CV_8UC1);
	color_img.create(240, 320, CV_8UC3);
	//1����ʼ��NUI��ע�⣺���ﴫ��Ĳ����Ͳ�һ���ˣ���DEPTH
	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH| NUI_INITIALIZE_FLAG_USES_COLOR);
	HRESULT color_hr = hr; //NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR);//ֻ�ܳ�ʼ��һ��NuiInitialize���
	if (FAILED(hr))
	{
		cout << "NuiInitialize failed" << endl;
		return hr;
	}

	//2�������¼���� 
	//������ȡ��һ֡���ź��¼����������KINECT�Ƿ���Կ�ʼ��ȡ��һ֡����
	HANDLE nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE depthStreamHandle = NULL; //����ͼ���������ľ����������ȡ����

	HANDLE nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE colorStreamHandle = NULL;

	//3����KINECT�豸�����ͼ��Ϣͨ��������depthStreamHandle��������ľ�����Ա����Ժ��ȡ
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_320x240,
		0, 2, nextDepthFrameEvent, &depthStreamHandle);
	if (FAILED(hr))//�ж��Ƿ���ȡ��ȷ 
	{
		cout << "Could not open depth depth_img stream video" << endl;
		NuiShutdown();
		return hr;
	}
	color_hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_320x240,
		0, 2, nextColorFrameEvent, &colorStreamHandle);
	if (FAILED(color_hr))//�ж��Ƿ���ȡ��ȷ 
	{
		cout << "Could not open color color_img stream video" << endl;
		NuiShutdown();
		return hr;
	}
	namedWindow("depthImage", CV_WINDOW_AUTOSIZE);
	namedWindow("color_img",CV_WINDOW_AUTOSIZE);
	//4����ʼ��ȡ������� 
	while (1)
	{
		const NUI_IMAGE_FRAME * pImageFrame = NULL;
		const NUI_IMAGE_FRAME * color_pImageFrame = NULL;
		//4.1�����޵ȴ��µ����ݣ��ȵ��󷵻�
		if (WaitForSingleObject(nextDepthFrameEvent, INFINITE) == 0)
		{
			//4.2���ӸղŴ���������������еõ���֡���ݣ���ȡ�������ݵ�ַ����pImageFrame
			hr = NuiImageStreamGetNextFrame(depthStreamHandle, 0, &pImageFrame);
			
			if (FAILED(hr))
			{
				cout << "Could not get depth depth_img" << endl;
				NuiShutdown();
				return -1;
			}

			INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
			NUI_LOCKED_RECT LockedRect;

			//4.3����ȡ����֡��LockedRect���������������ݶ���pitchÿ���ֽ�����pBits��һ���ֽڵ�ַ
			//���������ݣ����������Ƕ����ݵ�ʱ��kinect�Ͳ���ȥ�޸���
			pTexture->LockRect(0, &LockedRect, NULL, 0);

			//4.4��ȷ�ϻ�õ������Ƿ���Ч
			if (LockedRect.Pitch != 0)
			{
				//4.5��������ת��ΪOpenCV��Mat��ʽ
				for (int i = 0; i<depth_img.rows; i++)
				{
					uchar *ptr = depth_img.ptr<uchar>(i);  //��i�е�ָ��
													   //���ͼ�����ݺ������ָ�ʽ���������صĵ�12λ��ʾһ�����ֵ����4λδʹ�ã�
													   //ע��������Ҫת������Ϊÿ��������2���ֽڣ��洢��ͬ�������ɫ��Ϣ��һ����
					uchar *pBufferRun = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
					USHORT * pBuffer = (USHORT*)pBufferRun;
					for (int j = 0; j<depth_img.cols; j++)
					{

						ptr[(depth_img.cols-1)*depth_img.channels() - j] =  (BYTE)(256 * pBuffer[j] / 0x0fff);  //ֱ�ӽ����ݹ�һ������
					}
				}
				//cout << "��ֵ�� " << (sum/a++) << endl;
				imshow("depthImage", depth_img); //��ʾͼ�� 
			}
			else
			{
				cout << "Buffer length of received texture is bogus\r\n" << endl;
			}
			//5����֡�Ѿ��������ˣ����Խ������
			pTexture->UnlockRect(0);
			//6���ͷű�֡���ݣ�׼��ӭ����һ֡ 
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

			if (color_LockedRect.Pitch != 0) { //4.5��������ת��ΪOpenCV��Mat��ʽ 
				for (int i = 0; i < color_img.rows; i++) {
					uchar *ptr = color_img.ptr<uchar>(i);
					//��i�е�ָ�� //ÿ���ֽڴ���һ����ɫ��Ϣ��ֱ��ʹ��uchar 
					uchar *pBuffer = (uchar*)(color_LockedRect.pBits) + i * color_LockedRect.Pitch;
					for (int j = 0; j < color_img.cols; j++) {
						ptr[(color_img.cols - 1) * color_img.channels() - 3 * j    ] = pBuffer[4 * j]; //�ڲ�������4���ֽڣ�0-1-2��BGR����4������δʹ�� 
						ptr[(color_img.cols - 1) * color_img.channels() - 3 * j + 1] = pBuffer[4 * j + 1];
						ptr[(color_img.cols - 1) * color_img.channels() - 3 * j + 2] = pBuffer[4 * j + 2];
					}
				}
				imshow("colorImage", color_img); //��ʾͼ�� 
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
	//7���ر�NUI���� 
	NuiShutdown();
	//system("pause");
	return 0;
}