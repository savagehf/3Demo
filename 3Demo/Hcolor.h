#ifndef __HCOLOR_H__
#define __HCOLOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class HColor4f
{
public:
	float r,g,b,a;

	// fonctions c++

	HColor4f() : r(0),g(0),b(0),a(1) {}
	HColor4f(float _r,float _g,float _b,float _a=1.0f) 
	 : r(_r),g(_g),b(_b),a(_a) {}
	operator float *()
	{
		return (float*)this;
	}
	operator const float *() const
	{
		return (const float*)this;
	}
	HColor4f &operator+=(const HColor4f &c)
	{
		r+=c.r; g+=c.g; b+=c.b; a+=c.a;
		return *this;
	}
	HColor4f &operator-=(const HColor4f &c)
	{
		r-=c.r; g-=c.g; b-=c.b; a-=c.a;
		return *this;
	}
	HColor4f &operator*=(const HColor4f &c)
	{
		r*=c.r; g*=c.g; b*=c.b; a*=c.a;
		return *this;
	}
	HColor4f &operator *=(float f)
	{
		r*=f;
		g*=f;
		b*=f;
		a*=f;
		return *this;
	}
	void Blend(const HColor4f &c,float t)
	{
		r=r*(1.0f-t)+c.r*t;
		g=g*(1.0f-t)+c.g*t;
		b=b*(1.0f-t)+c.b*t;
		a=a*(1.0f-t)+c.a*t;
	}
	void Blend(const HColor4f &c1,const HColor4f &c2,float t)
	{
		r=c1.r*(1.0f-t)+c2.r*t;
		g=c1.g*(1.0f-t)+c2.g*t;
		b=c1.b*(1.0f-t)+c2.b*t;
		a=c1.a*(1.0f-t)+c2.a*t;
	}
	void Blend(const HColor4f &c)
	{
		float t=c.a;
		r=r*(1.0f-t)+c.r*t;
		g=g*(1.0f-t)+c.g*t;
		b=b*(1.0f-t)+c.b*t;
		a=a*(1.0f-t)+c.a;
	}
	void Blend(const HColor4f &c1,const HColor4f &c2)
	{
		float t=c2.a;
		r=c1.r*(1.0f-t)+c2.r*t;
		g=c1.g*(1.0f-t)+c2.g*t;
		b=c1.b*(1.0f-t)+c2.b*t;
		a=c1.a*(1.0f-t)+c2.a;
	}
	void Add(const HColor4f &c)
	{
		r+=c.r;
		g+=c.g;
		b+=c.b;
		a+=c.a;
	}
	void Add(const HColor4f &c1,const HColor4f &c2)
	{
		r=c1.r+c2.r;
		g=c1.g+c2.g;
		b=c1.b+c2.b;
		a=c1.a+c2.a;
	}
	void Add(const HColor4f &c,float t)
	{
		r+=c.r*t;
		g+=c.g*t;
		b+=c.b*t;
		a+=c.a*t;
	}
	void Add(const HColor4f &c1,const HColor4f &c2,float t)
	{
		r=c1.r+c2.r*t;
		g=c1.g+c2.g*t;
		b=c1.b+c2.b*t;
		a=c1.a+c2.a*t;
	}
	HColor4f operator*(const HColor4f &c) const
	{
		return HColor4f(r*c.r,g*c.g,b*c.b,a*c.a);
	}
	HColor4f operator+(const HColor4f &c) const
	{
		return HColor4f(r+c.r,g+c.g,b+c.b,a+c.a);
	}
	void Put() const
	{
		glColor4f(r,g,b,a);
	}
};

#endif
