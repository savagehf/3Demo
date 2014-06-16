//-------------------------------------------------------------------
//  File:     HMissile.cpp
//  Created:  September 09, 2000
//  Author:   Jeremie Allard (Hufo / N.A.A.)
//  Comments: Create and manage Scene.
//-------------------------------------------------------------------
#include "stdafx.h"
#include "std.h"
#define __HSCENE__
#include "HDef.h"
#include "HScene.h"

SCENE_API void NewMissile(HTank *tank, float speed)
{
	HMissile *m=new HMissile(tank,speed);
	m->SetNext(lMissiles);
	lMissiles=m;
}

void CalcExplode(HMissile *m)
{
	m->Explode();
	HPoint3D Pos=m->Pos;
	for (int i=0;i<NTANK;i++)
	{
		TTank[i].ApplyExplode(Pos);
	}
}

SCENE_API void UpdateMissile(float t,float dt)
{
	HMissile *m;
	HMissile *last;
	m=lMissiles; last=NULL;
	while (m!=NULL)
	{
		bool kill=false;
		m->Update(t,dt);
		if (  m->Pos.x<=HMissile::bbClip.a.x || m->Pos.x>=HMissile::bbClip.b.x
		   || m->Pos.z<=HMissile::bbClip.a.z || m->Pos.z>=HMissile::bbClip.b.z
		   || m->Pos.y<=HMissile::bbClip.a.y)
		{
			CalcExplode(m);
			kill=true;
		}
		if (kill)
		{
			if (last==NULL)
				m=lMissiles=lMissiles->DeleteThis();
			else
			{
				last->DeleteNext();
				m=last->Next();
			}
		}
		else
		{
			last=m;
			m=m->Next();
		}
	}
}

SCENE_API void AffMissile()
{
	HMissile *m;
	m=lMissiles;
	while (m!=NULL)
	{
		m->Aff();
		m=m->Next();
	}
}

SCENE_API void FreeMissile()
{
	while (lMissiles!=NULL)
	{
		lMissiles=lMissiles->DeleteThis();
	}
}
