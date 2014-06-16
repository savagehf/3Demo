//-------------------------------------------------------------------
//  File:     HMissile.cpp
//  Created:  September 09, 2000
//  Author:   Jeremie Allard (Hufo / N.A.A.)
//  Comments: Create and manage Missiles.
//-------------------------------------------------------------------

#include "stdafx.h"
#include "std.h"
#include "HDef.h"
#include "HMissile.h"

HObject HMissile::obj;
HPoint3D HMissile::Grav(0,-50,0);
HBBox HMissile::bbClip;
HPartSyst * HMissile::psys=NULL;

bool HMissile::Load()
{
	HPoint3D pivot(0,1,0);
	Test(obj.LoadLWO("data/missile1.lwo"));

	HVector3D light(0.5f,-1.0f,0.2f); light.Normalize();
	HColor4f clight(1.0f,1.0f,1.0f);
	HColor4f ambient(0.2f,0.2f,0.2f);

	obj.ApplyLight(light,clight,ambient);

	return r_ok;
}

bool HMissile::LoadTexture()
{
	obj.LoadTexture();
	return r_ok;
}

void HMissile::SetGravity(float g)
{
	Grav.y=-g;
}

void HMissile::SetArena(HBBox *bb)
{
	bbClip=*bb;
}

void HMissile::SetPartSyst(HPartSyst *ps)
{
	psys=ps;
}

HMissile::HMissile(HTank *tank,float speed)
: source(tank), next(NULL)
{
	tank->GetGunPos(&Pos);
	tank->GetGunDir(&Vit); Vit*=speed/Vit.Length();
}

void HMissile::DeleteNext()
{
	if (next!=NULL)
		next=next->DeleteThis();
}

HMissile *HMissile::DeleteThis()
{
	HMissile *n=next;
	delete this;
	return n;
}

void HMissile::Aff() // render missile
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

		glTranslatef(Pos.x,Pos.y,Pos.z);
		obj.Aff();

	glPopMatrix();
}

void HMissile::Update(float t,float dt)
{
	Vit+=Grav*dt;
	Pos+=Vit*dt;
	if (psys!=NULL)
	{
		HParticule *p=psys->AddParticule();
		if (p!=NULL)
		{
			p->type=HParticule::TFog+(HParticule::NFog*rand())/RAND_MAX;
			//p->type=HParticule::TFlare;
			p->Pos=Pos;
			p->c.a=0.4f;
			p->time=2.0f;
			p->size=1.0f;
			p->da=-p->c.a/p->time;
			p->ds=2.0f/p->time;
		}
	}
}

void HMissile::Explode() // add particules for explosion
{
	if (psys==NULL) return;
	HParticule *p;

	p=psys->AddParticule();

	if (p!=NULL)
	{
		p->type=HParticule::TFog+(HParticule::NFog*rand())/RAND_MAX;
		p->Pos=Pos;
		p->time=5.0f;
		p->c.a=1.0f;
		p->da=-p->c.a/p->time;
		p->size=0.0f;
		p->ds=60.0f/p->time;
		//p->Vit.y=p->ds*0.5f;
		p->Vit.y=Vit.y*(-0.04f);
	}

	for (int i=0;i<10;i++)
	{
		p=psys->AddParticule();
		if (p!=NULL)
		{
			p->type=HParticule::TExplo;
			p->Pos=Pos;
			p->Vit=Vit*0.1f; p->Vit.y*=-0.4f;
			float nr=p->Vit.Length();
			p->Vit.x+=(rnd()-0.5f)*nr;
			p->Vit.y+=(rnd()-0.5f)*nr;
			p->Vit.z+=(rnd()-0.5f)*nr;
			p->m=0.01f;
			p->time=2.0f+rnd();
			p->c.a=1.0f;
			p->da=-p->c.a/p->time;
			p->size=0.0f; //5.0f+5.0f*rnd();
			p->ds=(10.0f+10.0f*rnd())/p->time;
		}
	}
}

