#include "gdal.h"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <stdio.h>
#include <iostream>

using namespace std;

/*
* @brief ConvertPolylineToPolygon        转换线为面
* @param[in] OGRGeometry* polygon        要转换的面
* @return OGRGeometry*            　　　　转换成功后的线
* @author 
* @date 
* @note 
*/
OGRGeometry* ConvertPolylineToPolygon(OGRGeometry* polyline)
{
	// 线生成
	OGRwkbGeometryType sourceGeometryType=polyline->getGeometryType();
	sourceGeometryType=wkbFlatten(sourceGeometryType);

	OGRwkbGeometryType targetGeometryType;
	switch(sourceGeometryType)
	{
	case OGRwkbGeometryType::wkbLineString:
		{
			OGRLineString* pOGRLineString=(OGRLineString*) polyline;
			targetGeometryType = OGRwkbGeometryType::wkbPolygon;

			OGRPolygon* pOGRPolygon=(OGRPolygon*)OGRGeometryFactory::createGeometry(targetGeometryType);

			OGRLinearRing pOGRLinearRing;
			int pointCount=pOGRLineString->getNumPoints();
			double x=0; double y=0;
			for(int i=0;i<pointCount;i++)
			{    
				x=pOGRLineString->getX(i);
				y=pOGRLineString->getY(i);
				pOGRLinearRing.addPoint(x,y);
			}
			pOGRLinearRing.closeRings();
			pOGRPolygon->addRing(&pOGRLinearRing);
			return pOGRPolygon;
		}
	case OGRwkbGeometryType::wkbMultiLineString:
		{
			targetGeometryType = OGRwkbGeometryType::wkbMultiPolygon;
			OGRMultiPolygon* pOGRMultiPolygon=(OGRMultiPolygon*)OGRGeometryFactory::createGeometry(targetGeometryType);

			OGRGeometryCollection* pOGRPolylines=(OGRGeometryCollection*)polyline;
			int geometryCount=pOGRPolylines->getNumGeometries();

			for(int i=0;i<geometryCount;i++)
			{
				OGRGeometry* pOGRGeo=ConvertPolylineToPolygon(pOGRPolylines->getGeometryRef(i));
				pOGRMultiPolygon->addGeometry(pOGRGeo);
			}

			return pOGRMultiPolygon;
		}
	default:
		return NULL;
	}

	return NULL;
}

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