#ifndef __HPOINT_H__
#define __HPOINT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ptypes.h"
#include <math.h>

typedef float HCoord;

class HPoint3D
{
public:
	HCoord x,y,z;

	// fonctions c++

	HPoint3D() :x(0),y(0),z(0) {}
	HPoint3D(HCoord _x,HCoord _y,HCoord _z) : x(_x),y(_y),z(_z) {}
	void Zero()
	{
		x=0; y=0; z=0;
	}
	void Clear()
	{
		x=0; y=0; z=0;
	}
	HCoord Length2() const
	{
		return sqr(x)+sqr(y)+sqr(z);
	}
	HCoord Length() const
	{
		return (HCoord)sqrt(Length2());
	}
	HPoint3D &operator+=(const HPoint3D &p)
	{
		x+=p.x; y+=p.y; z+=p.z;
		return *this;
	}
	HPoint3D &operator-=(const HPoint3D &p)
	{
		x-=p.x; y-=p.y; z-=p.z;
		return *this;
	}
	HPoint3D &operator*=(const HPoint3D &p)
	{
		x*=p.x; y*=p.y; z*=p.z;
		return *this;
	}
	HPoint3D &operator*=(HCoord c)
	{
		x*=c; y*=c; z*=c;
		return *this;
	}
	HPoint3D &operator/=(HCoord c)
	{
		x/=c; y/=c; z/=c;
		return *this;
	}
	HPoint3D operator+(const HPoint3D &p) const
	{
		return HPoint3D(x+p.x,y+p.y,z+p.z);
	}
	HPoint3D operator-(const HPoint3D &p) const
	{
		return HPoint3D(x-p.x,y-p.y,z-p.z);
	}
	HPoint3D operator-() const
	{
		return HPoint3D(-x,-y,-z);
	}
	HPoint3D operator*(HCoord c) const
	{
		return HPoint3D(x*c,y*c,z*c);
	}
	HPoint3D operator/(HCoord c) const
	{
		return HPoint3D(x/c,y/c,z/c);
	}
	HCoord operator*(const HPoint3D &p) const
	{
		return x*p.x+y*p.y+z*p.z;
	}
	void Normalize()
	{
		HCoord f=Length();
		if (f>0)
		{
			f=1/f;
			x*=f; y*=f; z*=f;
		}
	}
	void CrossProduct(const HPoint3D &a,const HPoint3D &b)
	{
		x = a.y * b.z - b.y * a.z;
		y = a.z * b.x - b.z * a.x;
		z = a.x * b.y - b.x * a.y;
	}

	void Lerp(const HPoint3D &a,const HPoint3D &b,float f)
	{
		float f2=1.0f-f;
		x = a.x * f2 + b.x * f;
		y = a.y * f2 + b.y * f;
		z = a.z * f2 + b.z * f;
	}

	void Put()
	{
		glVertex3f(x,y,z);
	}
};

typedef HPoint3D HVector3D;


class HPoint2D
{
public:
	HCoord x,y;

	// fonctions c++

	HPoint2D() :x(0),y(0) {}
	HPoint2D(HCoord _x,HCoord _y) : x(_x),y(_y) {}
	void Zero()
	{
		x=0; y=0;
	}
	void Clear()
	{
		x=0; y=0;
	}
	HCoord Length2() const
	{
		return sqr(x)+sqr(y);
	}
	HCoord Length() const
	{
		return (HCoord)sqrt(Length2());
	}
	HPoint2D &operator+=(const HPoint2D &p)
	{
		x+=p.x; y+=p.y;
		return *this;
	}
	HPoint2D &operator-=(const HPoint2D &p)
	{
		x-=p.x; y-=p.y;
		return *this;
	}
	HPoint2D &operator*=(const HPoint2D &p)
	{
		x*=p.x; y*=p.y;
		return *this;
	}
	HPoint2D &operator*=(HCoord c)
	{
		x*=c; y*=c;
		return *this;
	}
	HPoint2D &operator/=(HCoord c)
	{
		x/=c; y/=c;
		return *this;
	}
	HPoint2D operator+(const HPoint2D &p) const
	{
		return HPoint2D(x+p.x,y+p.y);
	}
	HPoint2D operator-(const HPoint2D &p) const
	{
		return HPoint2D(x-p.x,y-p.y);
	}
	HPoint2D operator-() const
	{
		return HPoint2D(-x,-y);
	}
	HPoint2D operator*(HCoord c) const
	{
		return HPoint2D(x*c,y*c);
	}
	HPoint2D operator/(HCoord c) const
	{
		return HPoint2D(x/c,y/c);
	}
	HCoord operator*(const HPoint2D &p) const
	{
		return x*p.x+y*p.y;
	}
	void Normalize()
	{
		HCoord f=Length();
		if (f>0)
		{
			f=1/f;
			x*=f; y*=f;
		}
	}
/*
	void CrossProduct(const HPoint2D &a,const HPoint2D &b)
	{
		x = a.y * b.z - b.y * a.z;
		y = a.z * b.x - b.z * a.x;
		z = a.x * b.y - b.x * a.y;
	}
*/
	void Lerp(const HPoint2D &a,const HPoint2D &b,float f)
	{
		float f2=1.0f-f;
		x = a.x * f2 + b.x * f;
		y = a.y * f2 + b.y * f;
	}

	void Put()
	{
		glVertex2f(x,y);
	}

};

typedef HPoint2D HVector2D;

#endif
