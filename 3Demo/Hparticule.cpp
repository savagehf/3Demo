//-------------------------------------------------------------------
//  File:     HParticule.cpp
//  Created:  September 09, 2000
//  Author:   Jeremie Allard (Hufo / N.A.A.)
//  Comments: Create and manage Particules.
//-------------------------------------------------------------------
#include "stdafx.h"
#include "std.h"
#include "HDef.h"
#include "HParticule.h"
#include "HMissile.h"
#include "imgLoadBMP.h" // Image loader handler for RTexture/RImage.

RTexture HParticule::txt;
HTexCoord2f HParticule::tcPart[HParticule::NPTYPE][2];
HPoint3D HParticule::Grav(0,-50,0);
HBBox HParticule::bbClip;

HPoint3D HParticule::aff_dp[4];

bool HParticule::Load()
{
	int i=0;
	for (int y=0;y<4;y++)
		for (int x=0;x<4;x++)
		{
			tcPart[i][0].u=x*0.25f;
			tcPart[i][0].v=y*0.25f;
			tcPart[i][1].u=(x+1)*0.25f;
			tcPart[i][1].v=(y+1)*0.25f;
			++i;
		}

	return r_ok;
}

bool HParticule::LoadTexture()
{
	Test(txt.Create("data/particule.bmp", imgLoadBMP ));
/*
  {
    RImage imgTmp;
    imgTmp.Create( "data/particule.bmp", imgLoadBMP );
    txt.Create( &imgTmp, &imgTmp ); 

    imgTmp.Toast();
  }
*/
	return r_ok;
}

void HParticule::SetGravity(float g)
{
	Grav.y=-g;
}

HParticule::HParticule()
: m(0.0f), type(0), size(0.1f), ds(0.0f), c(1,1,1,1), da(0.0f), time(10.0f)
{
}

void HParticule::Clear()
{
	Pos.Clear();
	Vit.Clear();
	m=0.0f;
	type=0;
	size=0.1f;
	ds=0.0f;
	c.r=1; c.g=1; c.b=1; c.a=1;
	da=0.0f;
	time=10.0f;
}

void HParticule::Aff0(const HPoint3D &dx,const HPoint3D &dy)
{
	txt.Use();
	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_TEXTURE_2D);

	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//glDisable(GL_BLEND);

	glDisable(GL_LIGHTING);

	glDepthMask(0);

	aff_dp[0]=-dx+dy;
	aff_dp[1]= dx+dy;
	aff_dp[2]=-dx-dy;
	aff_dp[3]= dx-dy;
	//aff_dx=dx;
	//aff_dy=dy;
	glMatrixMode(GL_MODELVIEW);
}

void HParticule::Aff() // render missile
{

	c.Put();
	//glColor4f(1,1,1,1);
	float s=size;
	HTexCoord2f *tc=tcPart[type];
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(tc[0].u,tc[0].v);
		glVertex3f(Pos.x+aff_dp[0].x*s,Pos.y+aff_dp[0].y*s,Pos.z+aff_dp[0].z*s);
		glTexCoord2f(tc[1].u,tc[0].v);
		glVertex3f(Pos.x+aff_dp[1].x*s,Pos.y+aff_dp[1].y*s,Pos.z+aff_dp[1].z*s);
		glTexCoord2f(tc[0].u,tc[1].v);
		glVertex3f(Pos.x+aff_dp[2].x*s,Pos.y+aff_dp[2].y*s,Pos.z+aff_dp[2].z*s);
		glTexCoord2f(tc[1].u,tc[1].v);
		glVertex3f(Pos.x+aff_dp[3].x*s,Pos.y+aff_dp[3].y*s,Pos.z+aff_dp[3].z*s);
	glEnd();
/*
	glPushMatrix();

		glTranslatef(Pos.x,Pos.y,Pos.z);
		HMissile::obj.Aff();

	glPopMatrix();
*/
}

void HParticule::AffEnd()
{
	glDepthMask(1);
}

void HParticule::Update(float t,float dt)
{
	Vit+=Grav*(m*dt);
	Pos+=Vit*dt;
	//size*=pow(ds,dt);
	//c.a*=pow(da,dt);
	size+=ds*dt;
	c.a+=da*dt;
	time-=dt;
}

HPartSyst::HPartSyst()
:np(0), npmax(0), tp(NULL)
{
}

HPartSyst::~HPartSyst()
{
	Free();
}

void HPartSyst::Alloc(int nbp)
{
	Free();
	npmax=nbp;
	tp=new HParticule[npmax];
}

void HPartSyst::Free()
{
	npmax=0;
	np=0;
	if (tp!=NULL)
	{
		delete[] tp;
		tp=NULL;
	}
}

void HPartSyst::Aff(const HPoint3D &dx,const HPoint3D &dy)
{
	if (np>0)
	{
		HParticule::Aff0(dx,dy);
		HParticule *p=tp;
		int n=np;
		do
		{
			p->Aff();
			++p;
		}
		while (--n);
		HParticule::AffEnd();
	}
}

void HPartSyst::Update(float t,float dt)
{
	if (np>0)
	{
		HParticule *p=tp;
		int n=np;
		do
		{
			p->Update(t,dt);
			if (p->time<=0)
			{
				*p=tp[--np]; // delete particule by overwrite it
			}
			else
				++p; // goto next particule
		}
		while (--n);
	}
}

HParticule *HPartSyst::AddParticule()
{
	if (tp==NULL) return NULL;
	else if (np<npmax)
	{
		HParticule *p=&(tp[np++]);
		p->Clear();
		return p;
	}
	else
	{
		HParticule *p=&(tp[(rand()*npmax)/RAND_MAX]);
		p->Clear();
		return p;
	}
}
