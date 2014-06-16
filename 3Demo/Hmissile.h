#ifndef __HMISSILE_H__
#define __HMISSILE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HPoint.h"
#include "HObject.h"
#include "HTank.h"
#include "HParticule.h"

class HMissile
{
//protected:
public:

	static HObject obj;

	static HPoint3D Grav;

public:
	static HBBox bbClip;

	static HPartSyst * psys;

public:

	static bool Load();
	static bool LoadTexture();

	static void SetGravity(float g=10.0f);

	static void SetArena(HBBox *bb);

	static void SetPartSyst(HPartSyst *ps);

//protected:
public:

	HPoint3D Pos;// tank pos
	HPoint3D Vit;

	HTank *source; // tank number

	HMissile *next; // next missile in chain

public:

	HMissile(HTank *tank,float speed=1.0f);

	void Aff(); // render missile

	void Update(float t,float dt);

	void Explode(); // add particules for explosion

	HMissile *Next()
	{
		return next;
	}

	void SetNext(HMissile *m)
	{
		next=m;
	}

	void DeleteNext();

	HMissile *DeleteThis();

};

#endif
