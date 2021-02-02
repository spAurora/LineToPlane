#include <stdio.h>
#include <iostream>
#include <vector>
#include "gdal.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal_priv.h"
#include "ogr_core.h"

using namespace std;

OGRDataSource* CreateShapeFile(char* writePath, OGRSpatialReference* pOGRSpatialReference, OGRwkbGeometryType wkbtype = wkbPolygon)
{
	const char *pszDriverName = "ESRI Shapefile"; 
	OGRSFDriver *poDriver;

	OGRRegisterAll();  

	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	if (poDriver == NULL)  
	{  
		printf( "%s driver not available.\n", pszDriverName );
		exit(-1);
	}

	OGRDataSource *poDS;  
	poDS = poDriver->CreateDataSource(writePath, NULL);  
	if (poDS == NULL)  
	{  
		printf("Create shapefile failed\n");
		system("pause");
		exit(1);
	}
	OGRLayer *poLayer;  
	poLayer = poDS->CreateLayer("polygon1", NULL, wkbPolygon, NULL);  
	if (poLayer == NULL)  
	{  
		printf("Layer create failed\n");  
		system("pause");
		exit(1);  
	}
	return poDS;
}

/*
* @brief ConvertPolygonToPolyline        ��ͼ��ת��Ϊ��ͼ��
* @param[in] tString polylinePath        ת������ͼ���ļ�·��
* @param[in] OGRLayer* pLayer            Ҫת������ͼ���ļ�
* @param[in] Envelope envelope            Ҫת�����ݵķ�Χ
* @param[in] vector<long> vecFIDs        ѡ�е�Ҫ��ID�б�
* @param[in] pOGRSpatialReference        Ҫת�������ݵĿռ�ο�(���Ϊ�ձ�ʾ����ϵ��Ϣ����)
* @return bool                    �Ƿ�ɹ�
* @author
* @date
* @note 2015��11��04�� С�˴�����
*/
bool ConvertPolylineToPolygonEx(char* polylinePath,OGRLayer* pLayer,OGREnvelope envelope,vector<long> vecFIDs,OGRSpatialReference* pOGRSpatialReference)
{
	// �ж�
	if(pLayer==NULL) return false;

	// ����ϵ��ȡ
	OGRSpatialReference* pOGRSpatialReference_Source=pLayer->GetSpatialRef();
	bool isSameCoordSystem = false;
	if(pOGRSpatialReference == NULL)
	{
		pOGRSpatialReference=pOGRSpatialReference_Source;
		isSameCoordSystem=true;
	}
	else if(pOGRSpatialReference!=NULL && pOGRSpatialReference_Source!=NULL)
	{
		isSameCoordSystem=pOGRSpatialReference_Source->IsSame(pOGRSpatialReference);
	}

	// ����Shape�ļ�
	OGRDataSource* pOGRDataSource= CreateShapeFile(polylinePath,pOGRSpatialReference,wkbPolygon);
	if(pOGRDataSource==NULL) return false;

	OGRLayer* pOGRLayer=pOGRDataSource->GetLayer(0);
	if(pOGRLayer==NULL) return false;

	// ��ϲ�
	OGRFeature* pOGRFeature_Old;
	OGRGeometry* pTempGeometry=NULL;
	OGRGeometry* pTempGeometryUnion=NULL;

	// ��ǰѡ�񵼳�
	if(false==vecFIDs.empty()&&vecFIDs.size()>0)
	{
		for(int i=0;i<vecFIDs.size();i++)
		{
			pOGRFeature_Old=pLayer->GetFeature(vecFIDs[i]);
			pTempGeometry=pOGRFeature_Old->GetGeometryRef();
			if(false == isSameCoordSystem)pTempGeometry->transformTo(pOGRSpatialReference);

			if(pTempGeometryUnion==NULL) pTempGeometryUnion=pTempGeometry;
			else pTempGeometryUnion=pTempGeometryUnion->Union(pTempGeometry);
		}
	}
	else
	{
		//ȡ���жϿգ�ֻ�������ж������С�Ƿ����
		if(envelope.MaxX!=envelope.MinX)
		{
			pLayer->SetSpatialFilterRect(envelope.MinX,envelope.MinY,envelope.MaxX,envelope.MaxY);
		}

		pOGRFeature_Old=pLayer->GetNextFeature();
		while(NULL!= pOGRFeature_Old)
		{
			pTempGeometry=pOGRFeature_Old->GetGeometryRef();
			if(false == isSameCoordSystem)pTempGeometry->transformTo(pOGRSpatialReference);

			if(pTempGeometryUnion==NULL) 
				pTempGeometryUnion=pTempGeometry;
			else 
				pTempGeometryUnion=pTempGeometryUnion->Union(pTempGeometry);

			pOGRFeature_Old=pLayer->GetNextFeature();
		}
	}
	
	//��ת��
	OGRGeometry* pOGRGeometryUnion = pTempGeometryUnion->Polygonize();

	// ��ShapeFile�ļ������������
	OGRFeature* pOGRFeature_New;
	OGRGeometry* pOGRGeometry;
	OGRFeatureDefn* pOGRFeatureDefn=NULL;
	pOGRFeatureDefn=pOGRLayer->GetLayerDefn();

	OGRwkbGeometryType ogrGeometryType=pOGRGeometryUnion->getGeometryType();
	ogrGeometryType=wkbFlatten(ogrGeometryType);

	if(ogrGeometryType==OGRwkbGeometryType::wkbGeometryCollection||ogrGeometryType==OGRwkbGeometryType::wkbMultiPolygon)
	{
		OGRGeometryCollection* pOGRGeometryCollectionTarget=(OGRGeometryCollection*) pOGRGeometryUnion;
		int geometryCount=pOGRGeometryCollectionTarget->getNumGeometries();
		for(int i=0;i<geometryCount;i++)
		{
			pOGRFeature_New=OGRFeature::CreateFeature(pOGRFeatureDefn);
			pOGRGeometry=pOGRGeometryCollectionTarget->getGeometryRef(i);
			pOGRFeature_New->SetGeometry(pOGRGeometry);
			pOGRLayer->CreateFeature(pOGRFeature_New);

			OGRFeature::DestroyFeature(pOGRFeature_New);
			pOGRFeature_New=NULL;
		}
	}
	else if(ogrGeometryType==OGRwkbGeometryType::wkbPolygon)
	{
		pOGRFeature_New=OGRFeature::CreateFeature(pOGRFeatureDefn);
		pOGRGeometry=pOGRGeometryUnion;
		pOGRFeature_New->SetGeometry(pOGRGeometry);
		pOGRLayer->CreateFeature(pOGRFeature_New);

		OGRFeature::DestroyFeature(pOGRFeature_New);
		pOGRFeature_New=NULL;

	}
	OGRDataSource::DestroyDataSource(pOGRDataSource);

	// ����pTargetGeometrys
	OGRGeometryFactory::destroyGeometry(pTempGeometryUnion);
	pTempGeometryUnion=NULL;

	return true;
}

int main()
{
	char filePath[] = "D:\\420107.dxf";
	char writePath[] = "D:\\polygon.shp";
	OGRDataSource* poDS = nullptr;

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
	poDS = poDriver->Open(filePath, NULL);
	if(poDS == NULL )
	{
		printf( "Open failed.\n%s" );
		exit(-1);
	}

	OGRLayer *poLayer = poDS->GetLayer(0); //��ȡ��
	poLayer->ResetReading(); //����Ҫ������
	printf("Layercount: %d\n", poDS->GetLayerCount()); //�������Ŀ

	vector<long> vecFIDs;
	OGREnvelope envelope;
	envelope.MinX = envelope.MinY = envelope.MaxX = envelope.MaxY = 0;
	ConvertPolylineToPolygonEx(writePath, poLayer, envelope, vecFIDs, NULL);

	//�ͷ��ڴ�
	OGRDataSource::DestroyDataSource(poDS);
	poDS = nullptr;

	system("pause");

	return 0;
}