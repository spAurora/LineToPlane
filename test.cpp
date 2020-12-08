#include "gdal.h"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <stdio.h>
#include <iostream>

using namespace std;

int main()
{
	//ע�������ļ���ʽ����
	GDALAllRegister();
	OGRRegisterAll();

	 //���������������
	CPLSetConfigOption("SHAPE_ENCODING",""); 

	//�õ���Ӧ�ļ����ʹ�����
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");

	//���ļ�
	OGRDataSource* poDS = poDriver->Open( "D:\\bou2_4p.shp", NULL );

	if( poDS == NULL )
	{
		printf( "Open failed.\n%s" );
		return 0;
	}
	OGRLayer  *poLayer;
	poLayer = poDS->GetLayer(0); //��ȡ��
	OGRFeature *poFeature;

	poLayer->ResetReading();
	int i=0;
	while( (poFeature = poLayer->GetNextFeature()) != NULL )
	{
		if(poFeature->GetFieldAsDouble("AREA")<1) continue; //ȥ�������С��polygon
		i=i++;
		cout<<i<<"  ";
		OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
		int iField;
		int n=poFDefn->GetFieldCount(); //����ֶε���Ŀ��������ǰ�����ֶΣ�FID,Shape);
		for( iField = 0; iField <n; iField++ )
		{
			//���ÿ���ֶε�ֵ
			cout<<poFeature->GetFieldAsString(iField)<<"    ";
		}
		cout<<endl;
		OGRFeature::DestroyFeature( poFeature );
	}
	GDALClose( poDS );
	system("pause");

	return 0;
}