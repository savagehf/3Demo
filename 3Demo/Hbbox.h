#ifndef __HBBOX_H__
#define __HBBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HPoint.h"

#define FMAX 1e10

class HBBox
{
public:
	HVector3D a,b;

	HBBox() : a(FMAX,FMAX,FMAX), b(-FMAX,-FMAX,-FMAX) {}
	HBBox(HVector3D _a,HVector3D _b) : a(_a), b(_b) {}

	bool IsEmpty()
	{
		//return a.x>=FMAX || a.y>=FMAX || a.z>=FMAX
		//    || b.x<=FMAX || b.y<=FMAX || b.z<=FMAX;
		return (a.x>b.x || a.y>b.y || a.z>b.z);
	}

	void Clear()
	{
		a.x= FMAX; a.y= FMAX; a.z= FMAX;
		b.x=-FMAX; b.y=-FMAX; b.z=-FMAX;
	}
	HBBox &operator +=(const HBBox &p)
	{
		if (p.a.x<a.x) a.x=p.a.x;
		if (p.a.y<a.y) a.y=p.a.y;
		if (p.a.z<a.z) a.z=p.a.z;
		if (p.b.x>b.x) b.x=p.b.x;
		if (p.b.y>b.y) b.y=p.b.y;
		if (p.b.z>b.z) b.z=p.b.z;
		return *this;
	}
	HBBox operator +(const HBBox &p)
	{
		return HBBox(HVector3D(min(a.x,p.a.x)
		                          ,min(a.y,p.a.y)
															,min(a.z,p.a.z))
									,HVector3D(max(b.x,p.b.x)
									            ,max(b.y,p.b.y)
															,max(b.z,p.b.z))
									);
	}
	float Size()
	{
		if (IsEmpty()) return 0;
		HVector3D v=b-a;
		return v.Length();
	}

	void Aff();
};

#endif
