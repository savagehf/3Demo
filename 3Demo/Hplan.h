#ifndef __HPLAN_H__
#define __HPLAN_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HPoint.h"

class HPlan
{
public:
	HVector3D n;
	HCoord d; // value for equation n.m=d
	HPoint3D p;
	HPlan() : n(1,0,0),d(0),p(0,0,0) {}
	void Clear()
	{
		n=HVector3D(1,0,0);
		d=0;
		p=HVector3D(0,0,0);
	}
	void Calc()
	{
		d=n*p;
	}
	void Calc(HPoint3D *p1, HPoint3D *p2, HPoint3D *p3)
	{
		p=*p1;
		HVector3D va=(*p2)-(*p1);
		HVector3D vb=(*p3)-(*p1);
		n.CrossProduct(va,vb);
		d=n*p;
	}
	void Normalize()
	{
		n.Normalize();
		Calc();
	}
	bool IsAbove(const HPoint3D &pt)
	{
		return (pt*n)>=d;
	}
	void CalcInter(const HPoint3D &p1,const HPoint3D &p2,HPoint3D *out)
	{
		float d1=p1*n;
		float d2=p2*n;
		out->Lerp(p1,p2,(d1-d)/(d1-d2));
	}
	void Invert()
	{
		n*=-1;
		d*=-1;
	}
};

#endif
