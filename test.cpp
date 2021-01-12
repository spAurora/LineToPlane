#include "gdal.h"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <stdio.h>
#include <iostream>

using namespace std;

/*
* @brief ConvertPolylineToPolygon        ת����Ϊ��
* @param[in] OGRGeometry* polygon        Ҫת������
* @return OGRGeometry*            ��������ת���ɹ������
* @author 
* @date 
* @note 
*/
OGRGeometry* ConvertPolylineToPolygon(OGRGeometry* polyline)
{
	// ������
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