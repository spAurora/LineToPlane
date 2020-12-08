#include "gdal.h"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <stdio.h>
#include <iostream>

using namespace std;

int main()
{
	//设置GDAL_DATA目录
	CPLSetConfigOption("GDAL_DATA","D:\\GDAL\\data");

	//定义DXF的驱动
	const char *pszDriverName = "DXF";

	//注册所有文件格式驱动
	GDALAllRegister();
	OGRRegisterAll();

	 //解决中文乱码问题
	CPLSetConfigOption("SHAPE_ENCODING",""); 

	//得到对应文件类型处理器
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		exit(-1);
	}

	//打开文件
	OGRDataSource* poDS = poDriver->Open( "D:\\420107.dxf", NULL );
	if( poDS == NULL )
	{
		printf( "Open failed.\n%s" );
		exit(-1);
	}

	/****************/


	system("pause");

	return 0;
}