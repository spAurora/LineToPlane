#include "gdal.h"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <stdio.h>
#include <iostream>

using namespace std;

int main()
{
	//����GDAL_DATAĿ¼
	CPLSetConfigOption("GDAL_DATA","D:\\GDAL\\data");

	//����DXF������
	const char *pszDriverName = "DXF";

	//ע�������ļ���ʽ����
	GDALAllRegister();
	OGRRegisterAll();

	 //���������������
	CPLSetConfigOption("SHAPE_ENCODING",""); 

	//�õ���Ӧ�ļ����ʹ�����
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	if( poDriver == NULL )
	{
		printf( "%s driver not available.\n", pszDriverName );
		exit(-1);
	}

	//���ļ�
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