#include "gdal.h"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <stdio.h>
#include <iostream>

using namespace std;

int main()
{
	//注册所有文件格式驱动
	GDALAllRegister();
	OGRRegisterAll();

	 //解决中文乱码问题
	CPLSetConfigOption("SHAPE_ENCODING",""); 

	//得到对应文件类型处理器
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");

	//打开文件
	OGRDataSource* poDS = poDriver->Open( "D:\\bou2_4p.shp", NULL );

	if( poDS == NULL )
	{
		printf( "Open failed.\n%s" );
		return 0;
	}
	OGRLayer  *poLayer;
	poLayer = poDS->GetLayer(0); //读取层
	OGRFeature *poFeature;

	poLayer->ResetReading();
	int i=0;
	while( (poFeature = poLayer->GetNextFeature()) != NULL )
	{
		if(poFeature->GetFieldAsDouble("AREA")<1) continue; //去掉面积过小的polygon
		i=i++;
		cout<<i<<"  ";
		OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
		int iField;
		int n=poFDefn->GetFieldCount(); //获得字段的数目，不包括前两个字段（FID,Shape);
		for( iField = 0; iField <n; iField++ )
		{
			//输出每个字段的值
			cout<<poFeature->GetFieldAsString(iField)<<"    ";
		}
		cout<<endl;
		OGRFeature::DestroyFeature( poFeature );
	}
	GDALClose( poDS );
	system("pause");

	return 0;
}