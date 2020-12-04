#include<iostream>  
#include"gdal.h"  
#include"gdal_priv.h"  

using namespace std;  

int main(){  

	cout << "hello world" << endl;  

	char* fileName = "D:\\LC81200382013335LGN00_BQA.TIF";  
	GDALDataset *poDataset;     //GDAL数据集  
	GDALAllRegister();  

	poDataset = (GDALDataset*)GDALOpen(fileName, GA_ReadOnly);  

	if (poDataset == NULL){  
		cout << "文件打开错误" << endl;  
	}  
	else{  
		cout << "文件打开成功" << endl;  
	}  

	system("pause");  
}  