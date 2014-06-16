// Point3D.cpp: implementation of the Point3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Point3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Point3D::Point3D() {
}

Point3D::~Point3D(){
}

Point3D::Point3D(float x, float y, float z){
	this->x = x;
	this->y = y;
	this->z = z;
}

void Point3D::ToArray3f(float* array){
	array[0] = x;	array[1] = y;	array[2] = z;
}

void Point3D::FromArray3f(float* array){
	x = array[0];	y = array[1];	z = array[2];
}