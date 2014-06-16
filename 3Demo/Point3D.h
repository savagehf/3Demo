// Point3D.h: interface for the Point3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT3D_H__683E618B_46B6_4FFF_B49D_9F95FDB8CAD5__INCLUDED_)
#define AFX_POINT3D_H__683E618B_46B6_4FFF_B49D_9F95FDB8CAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Point3D  
{
public:
	float x,y,z;
	Point3D();
	Point3D(float x, float y, float z);
	void ToArray3f(float* array);
	void FromArray3f(float* array);
	virtual ~Point3D();

};

#endif // !defined(AFX_POINT3D_H__683E618B_46B6_4FFF_B49D_9F95FDB8CAD5__INCLUDED_)
