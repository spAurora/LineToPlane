#include<iostream>  
#include"gdal.h"  
#include"gdal_priv.h"  

using namespace std;  

int main(){  

	cout << "hello world" << endl;  

	char* fileName = "D:\\LC81200382013335LGN00_BQA.TIF";  
	GDALDataset *poDataset;     //GDAL���ݼ�  
	GDALAllRegister();  

	poDataset = (GDALDataset*)GDALOpen(fileName, GA_ReadOnly);  

	if (poDataset == NULL){  
		cout << "�ļ��򿪴���" << endl;  
	}  
	else{  
		cout << "�ļ��򿪳ɹ�" << endl;  
	}  

	system("pause");  
}  