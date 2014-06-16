#ifndef __HPARTICULE_H__
#define __HPARTICULE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HPoint.h"
#include "HObject.h"
#include "HTank.h"
#include "RTexture.h"

class HParticule
{
protected:

	static RTexture txt;

public:

	enum {
		TFlare=0,
		TExplo=1,
		T2=2,
		T3=3,
		T4=4,
		T5=5,
		T6=6,
		T7=7,
		TFog=8,
		NFog=8,
		NPTYPE=16
	};

protected:

	static HTexCoord2f tcPart[NPTYPE][2];

	static HPoint3D Grav;

public:
	static HBBox bbClip;

	//static HPoint3D aff_dx,aff_dy;
	static HPoint3D aff_dp[4];

public:

	static bool Load();
	static bool LoadTexture();

	static void SetGravity(float g=10.0f);

	static void SetArena(HBBox *bb);

//protected:
public:

	HPoint3D Pos; // tank pos
	HPoint3D Vit;

	float m;

	int type;

	float size,ds;

	HColor4f c;

	float da; // dalpha/dt

	float time; // time to death

public:

	HParticule();

	void Clear();

	static void Aff0(const HPoint3D &dx,const HPoint3D &dy); // init rendering

	void Aff(); // render

	static void AffEnd();

	void Update(float t,float dt);

};

class HPartSyst // Particule System
{
//protected:
public:

	int npmax,np;
	HParticule *tp;

public:

	HPartSyst();
	~HPartSyst();

	void Alloc(int nbp=1000);

	void Free();

	void Aff(const HPoint3D &dx,const HPoint3D &dy);

	void Update(float t,float dt);

	HParticule *AddParticule();

};

#endif
