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
* @brief ConvertPolygonToPolyline        线图层转换为面图层
* @param[in] tString polylinePath        转换后面图层文件路径
* @param[in] OGRLayer* pLayer            要转换的线图层文件
* @param[in] Envelope envelope            要转换数据的范围
* @param[in] vector<long> vecFIDs        选中的要素ID列表
* @param[in] pOGRSpatialReference        要转换的数据的空间参考(如果为空表示坐标系信息不变)
* @return bool                    是否成功
* @author
* @date
* @note 2015年11月04日 小八创建；
*/
bool ConvertPolylineToPolygonEx(char* polylinePath,OGRLayer* pLayer,OGREnvelope envelope,vector<long> vecFIDs,OGRSpatialReference* pOGRSpatialReference)
{
	// 判断
	if(pLayer==NULL) return false;

	// 坐标系读取
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

	// 创建Shape文件
	OGRDataSource* pOGRDataSource= CreateShapeFile(polylinePath,pOGRSpatialReference,wkbPolygon);
	if(pOGRDataSource==NULL) return false;

	OGRLayer* pOGRLayer=pOGRDataSource->GetLayer(0);
	if(pOGRLayer==NULL) return false;

	// 面合并
	OGRFeature* pOGRFeature_Old;
	OGRGeometry* pTempGeometry=NULL;
	OGRGeometry* pTempGeometryUnion=NULL;

	// 当前选择导出
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
		//取消判断空，只保留了判断最大最小是否相等
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
	
	//线转面
	OGRGeometry* pOGRGeometryUnion = pTempGeometryUnion->Polygonize();

	// 在ShapeFile文件中添加数据行
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

	// 销毁pTargetGeometrys
	OGRGeometryFactory::destroyGeometry(pTempGeometryUnion);
	pTempGeometryUnion=NULL;

	return true;
}

int main()
{
	char filePath[] = "D:\\420107.dxf";
	char writePath[] = "D:\\polygon.shp";
	OGRDataSource* poDS = nullptr;

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
	poDS = poDriver->Open(filePath, NULL);
	if(poDS == NULL )
	{
		printf( "Open failed.\n%s" );
		exit(-1);
	}

	OGRLayer *poLayer = poDS->GetLayer(0); //读取层
	poLayer->ResetReading(); //遍历要素重置
	printf("Layercount: %d\n", poDS->GetLayerCount()); //输出层数目

	vector<long> vecFIDs;
	OGREnvelope envelope;
	envelope.MinX = envelope.MinY = envelope.MaxX = envelope.MaxY = 0;
	ConvertPolylineToPolygonEx(writePath, poLayer, envelope, vecFIDs, NULL);

	//释放内存
	OGRDataSource::DestroyDataSource(poDS);
	poDS = nullptr;

	system("pause");

	return 0;
}