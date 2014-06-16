#ifndef __HOBJECT_H__
#define __HOBJECT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HColor.h"
#include "HPoint.h"
#include "HBBox.h"
#include "RTexture.h"

class HTexCoord2f
{
public:
	void Put() const;
	union
	{
		struct
		{
			float u,v;
		};
		float t[2];
	};
	HTexCoord2f()
	: u(0.0f),v(0.0f)
	{}
	HTexCoord2f(float uu,float vv=0.0f)
	: u(uu),v(vv)
	{}
};

class HVertex;
class HMatTexture
{
public:
	void CalcMapping(HVertex *v) const;
	bool Loadable() const;
	bool Usable() const;
	void Use() const;
	// Mapping
	enum MapType { None=0,Plan,Cyl,Sphe } map;
	enum MapAxe { Axe0=0,AxeX=1,AxeY=2,AxeZ=4 } axe;
	HPoint3D ctr,size;
	//bool warpx,warpy;
	// Texture elle même
	RTexture *txt;
	char *name;

	HMatTexture();
	~HMatTexture();
	void FileName(char *fname);
	bool Load();
	void Free();
};

#define PUT_COORD    1
#define PUT_NORMAL   2
#define PUT_COLOR    4
#define PUT_TEXCOORD 8


class HMaterial
{
public:
	void CalcMapping(HVertex *v) const;
	void FreeTexture();
	bool LoadTexture();
	void MaterialFogPlane();
	static bool UseArray;
	void SetFlagColor(bool b);
	bool GetFlagColor();
	bool SetDynLighting(bool b);
	bool GetDynLighting();
	bool IsLighting() const;
	int Put() const;
	enum
	{
		Smooth=1,
		Double=2,
		Lighting=4,
		Blend=8,
		Additive=16,
		Color=32,
		CTxt=64,
		DynLighting=128
	};
	char name[16];
	int flag;
	HColor4f ambient;
	HColor4f diffuse;
	HColor4f specular;
	HColor4f emission;
	int shininess;
#ifndef NOTEXTURE
	HMatTexture ctxt;
#endif
	HMaterial()
	: ambient (0.2f,0.2f,0.2f)
	, diffuse (0.8f,0.8f,0.8f)
	, specular(0.0f,0.0f,0.0f)
	, emission(0.0f,0.0f,0.0f)
	, shininess(0)
	, flag(Smooth)
	{}
};
class HVertex
{
public:
	void ClearColor(const HColor4f &c=HColor4f(0.0f,0.0f,0.0f,1.0f));
	void ApplyLight(const HMaterial &m,const HPoint3D &lp,const HColor4f &lc,const HColor4f &lighta=HColor4f(0.2f,0.2f,0.2f,1.0f));
	void BlendFog(const HPoint3D &cam,const HColor4f &fogc=HColor4f(1.0f,1.0f,1.0f,1.0f));
	void BlendFog0(const HPoint3D &cam,const HColor4f &fogc=HColor4f(1.0f,1.0f,1.0f,1.0f));
	void ApplyFogPlane(const HPoint3D &cam,const HColor4f &fogc=HColor4f(1.0f,1.0f,1.0f,1.0f));
	HColor4f c;
	HPoint3D n;
	HPoint3D p;
	//HColor4f clight;
	//HColor4f cfog;
#ifndef NOTEXTURE
	HTexCoord2f t;
#endif
	void Put(int pdata) const;
};

class HFace
{
public:
	void CalcNormal();
	void Put(int pdata) const;
	void Aff(int pdata) const;
	HVertex *v[3];
	HPoint3D n;
	HMaterial *m;
};

class HSurface : public HMaterial
{
public:
	void CalcMapping();
	void ApplyLight(const HPoint3D &lp,const HColor4f &lc,const HColor4f &lighta=HColor4f(0.2f,0.2f,0.2f,1.0f));
	void AffStrip() const;
	void Aff() const;
	//HMaterial m;
	int nbf;
	HFace *fc;
	int *fc_i;
	int nbp;
	HVertex *pt;
	HSurface() : fc(NULL), nbf(0), nbp(0), pt(NULL) {}
};

class HObject
{
public:
	void CalcMapping();
	void FreeTexture();
	bool LoadTexture();
	void SetFlagColor(bool b);
	void SetDynLighting(bool b);
	//void PopColor();
	//void PushColor();
	void ClearColor(const HColor4f &c=HColor4f(0.0f,0.0f,0.0f,1.0f));
	void ApplyLight(const HPoint3D &lp,const HColor4f &lc,const HColor4f &lighta=HColor4f(0.2f,0.2f,0.2f,1.0f));
	void SetYMax(float ymax);
	void SetYMin(float ymin);
	void Translate(const HPoint3D &vt);
	void Aff() const;
	void Free();
	HObject();
	~HObject();
	bool LoadLWO(char* szFileName);
	void AffStrip() const;

	int nbp,nbf,nbs;
	HVertex *pt;
	HFace *fc;
	HSurface *sf;

	void CalcBBox(HBBox *bb);
};

#endif
