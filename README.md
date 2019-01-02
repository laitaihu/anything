# anything
2019.1.2增加mat图像数据存至txt模块，此模块对单、三通道图像均有效，使用方法：
		imgname.convertTo(imgname, CV_32F);
		imgname = imgname /255；
		在调用模块添加上面两句。