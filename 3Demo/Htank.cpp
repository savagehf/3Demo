//-------------------------------------------------------------------
//  File:     HTank.cpp
//  Created:  September 09, 2000
//  Author:   Jeremie Allard (Hufo / N.A.A.)
//  Comments: Create and manage Tanks.
//-------------------------------------------------------------------
#include "stdafx.h"
#include "std.h"
#include "HDef.h"
#include "HTank.h"
//#include "IOFile.h"
//#include "imgLoadBMP.h" // Image loader handler for RTexture/RImage.

HObject HTank::oTank;
HObject HTank::oTurret;
HObject HTank::oGun;
//HPoint3D HTank::posTank(0.0f,1.0f,0.0f); // gun pos relative to turret
HPoint3D HTank::posTank(0.0f,0.0f,0.0f); // gun pos relative to turret
HPoint3D HTank::posTurret(0.0f,1.8f,0.0f); // turret pos relative to tank
//HPoint3D HTank::posGun(0.0f,2.3f,1.2f); // gun pos relative to turret
HPoint3D HTank::posGun(0.0f,-0.5f,1.2f); // gun pos relative to turret

HBBox HTank::bbTank;
HBBox HTank::bbTurret;
HBBox HTank::bbGun;
HBBox HTank::bbClip;
HPoint3D HTank::endGun;
HPoint3D HTank::pCenter;

HPoint3D HTank::Grav(0,-50,0);

const float HTank::TurretAngleSpeed=180.0f;
const float HTank::GunAngleSpeed=360.0f;

const float HTank::TurretAngleMin=-140.0f;
const float HTank::TurretAngleMax=140.0f;
const float HTank::GunAngleMin=-70.0f;
const float HTank::GunAngleMax=20.0f;

bool HTank::Load()
{
	HPoint3D pivot=posTank;
	Test(oTank  .LoadLWO("data/tank1.lwo"));
	{
		oTank.CalcBBox(&bbTank);
		pivot.y=posTank.y=-bbTank.a.y;
	  oTank  .Translate(-pivot);
	}
	Test(oTurret.LoadLWO("data/tank1_turret.lwo"));
	pivot+=posTurret;
	     oTurret.Translate(-pivot);
	Test(oGun   .LoadLWO("data/tank1_gun.lwo"));
	pivot+=posGun;
	     oGun   .Translate(-pivot);

	HVector3D light(0.5f,-1.0f,0.2f); light.Normalize();
	HColor4f clight(1.0f,1.0f,1.0f);
	HColor4f ambient(0.2f,0.2f,0.2f);

	oTank.ApplyLight(light,clight,ambient);
	oTurret.ApplyLight(light,clight,ambient);
	oGun.ApplyLight(light,clight,ambient);

	oTank.CalcBBox(&bbTank);
	oTurret.CalcBBox(&bbTurret);
	oGun.CalcBBox(&bbGun);

	endGun.x=(bbGun.a.x+bbGun.b.x)*0.5f;
	endGun.y=(bbGun.a.y+bbGun.b.y)*0.5f;
	endGun.z=bbGun.b.z;

	pCenter=(bbTank.a+bbTank.b)*0.5f;

	return r_ok;
}

bool HTank::LoadTexture()
{
	HTank::oTank.LoadTexture();
	HTank::oTurret.LoadTexture();
	HTank::oGun.LoadTexture();
	return r_ok;
}

void HTank::SetArena(HBBox *bb)
{
	bbClip=*bb;

	float hsize=(float)sqrt(sqr(max(bbTank.b.x,-bbTank.a.x))+sqr(max(bbTank.b.z,-bbTank.a.z)));

	bbClip.a.x+=hsize; bbClip.b.x-=hsize;
	bbClip.a.z+=hsize; bbClip.b.z-=hsize;
	bbClip.a.y-=bbTank.a.y; bbClip.b.y-=bbTank.b.y;
}

HTank::HTank()
: Pos(0,0,0), Speed(0), Angle(0), AngleSpeed(0), TurretAngle(0), GunAngle(0)
, TurretAngleGoal(0), GunAngleGoal(0)
{
}

void HTank::Aff()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

		glTranslatef(Pos.x+posTank.x,Pos.y+posTank.y,Pos.z+posTank.z);
		glRotatef(Angle,0,1,0); // rotate around Y (horizontal)
		oTank.Aff();

		glTranslatef(posTurret.x,posTurret.y,posTurret.z);
		glRotatef(TurretAngle,0,1,0); // rotate around Y (horizontal)
		oTurret.Aff();

		glTranslatef(posGun.x,posGun.y,posGun.z);
		glRotatef(GunAngle,1,0,0); // rotate around X (vertical)
		oGun.Aff();

	glPopMatrix();
}

void HTank::SetCam(HPoint3D pcam_in_turret) // set camera transformation with camera relative to turret
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		glRotatef(180,0,1,0);
		glTranslatef(-pcam_in_turret.x,-pcam_in_turret.y,-pcam_in_turret.z);
		//TRACE("1.camAngle.x=%.2f, camAngle.y=%.2f, camAngle.z=%.2f\n", -pcam_in_turret.x, -pcam_in_turret.y, -pcam_in_turret.z);
		glRotatef(-TurretAngleGoal,0,1,0); // rotate around Y (horizontal)
		//TRACE("2.Rotate with Y axis angle=%.2f\n", -TurretAngleGoal);
		glTranslatef(-posTurret.x,-posTurret.y,-posTurret.z);
		//TRACE("3.Translate posTurret.x=%.2f, posTurrent.y=%.2f, posTurrent.z=%.2f\n", -posTurret.x,-posTurret.y,-posTurret.z);
		glRotatef(-Angle,0,1,0); // rotate around Y (horizontal)
		//TRACE("4.Rotate with Y Angle=%.2f\n", -Angle);
		glTranslatef(-Pos.x,-Pos.y,-Pos.z);
		//TRACE("5.Tranlate Pos.x=%.2f, Translate Pos.y=%.2f, Translate Pos.z=%.2f\n", -Pos.x,-Pos.y,-Pos.z);

}

float HTank::GetCamAngle()
{
	return -180+TurretAngleGoal+Angle;
}

void HTank::DeplTank(float speed)
{
	Speed=speed;
}

void HTank::RotateTank(float da)
{
	AngleSpeed=da;
}

void HTank::RotateTurret(float da)
{
	TurretAngleGoal+=da;
	while (TurretAngleGoal>TurretAngleMax && TurretAngleGoal-360.0f>TurretAngleMin)
	{
		TurretAngleGoal-=360.0f;
	}
	while (TurretAngleGoal<TurretAngleMin && TurretAngleGoal+360.0f<TurretAngleMax)
	{
		TurretAngleGoal+=360.0f;
	}
}

void HTank::RotateGun(float da)
{
	GunAngleGoal+=da;
	if (GunAngleGoal>GunAngleMax)
		GunAngleGoal=GunAngleMax;
	else if (GunAngleGoal<GunAngleMin)
		GunAngleGoal=GunAngleMin;
}

void HTank::Update(float t,float dt)
{
	float a=(float)DEG2RAD(Angle);
	HPoint3D dir((float)sin(a),0,(float)cos(a));

	float frot;

	if (Pos.y>bbClip.a.y)
		frot=0.9f;
	else
	{
		float v=Vit.Length();
		if (v>0)
		{
			float d=dir*Vit/v;
			frot=(d>0)?0.2f+0.3f*d:0.2f;
		}
		else frot=0.2f;
	}
	Vit*=(float)pow(frot,dt);

	dir*=Speed*dt;

	Vit+=dir;

	Vit+=Grav*dt;

	//Pos+=dir;
	Pos+=Vit*dt;
	Angle+=AngleSpeed*dt;

	if (Pos.x>bbClip.b.x)
	{
		Pos.x=bbClip.b.x; Vit.x*=-0.9f;
	}
	else if (Pos.x<bbClip.a.x)
	{
		Pos.x=bbClip.a.x; Vit.x*=-0.9f;
	}
	if (Pos.y>bbClip.b.y)
	{
		Pos.y=bbClip.b.y; Vit.y*=-1.0f;
	}
	else if (Pos.y<bbClip.a.y)
	{
		Pos.y=bbClip.a.y; Vit.y*=-0.2f;
	}
	if (Pos.z>bbClip.b.z)
	{
		Pos.z=bbClip.b.z; Vit.z*=-0.9f;
	}
	else if (Pos.z<bbClip.a.z)
	{
		Pos.z=bbClip.a.z; Vit.z*=-0.9f;
	}

	if (TurretAngleGoal<TurretAngle)
	{
		TurretAngle-=TurretAngleSpeed*dt;
		if (TurretAngle<TurretAngleGoal)
			TurretAngle=TurretAngleGoal;
	}
	else if (TurretAngleGoal>TurretAngle)
	{
		TurretAngle+=TurretAngleSpeed*dt;
		if (TurretAngle>TurretAngleGoal)
			TurretAngle=TurretAngleGoal;
	}
	if (TurretAngle<TurretAngleMin)
		TurretAngle=TurretAngleMin;
	else if (TurretAngle>TurretAngleMax)
		TurretAngle=TurretAngleMax;

	if (GunAngleGoal<GunAngle)
	{
		GunAngle-=GunAngleSpeed*dt;
		if (GunAngle<GunAngleGoal)
			GunAngle=GunAngleGoal;
	}
	else if (GunAngleGoal>GunAngle)
	{
		GunAngle+=GunAngleSpeed*dt;
		if (GunAngle>GunAngleGoal)
			GunAngle=GunAngleGoal;
	}
	if (GunAngle<GunAngleMin)
		GunAngle=GunAngleMin;
	else if (GunAngle>GunAngleMax)
		GunAngle=GunAngleMax;
}

void HTank::GetGunPos(HPoint3D *pos)
{
	HPoint3D p,p2;
	float a;
	a=DEG2RAD(GunAngle);
	p.x=endGun.x;
	p.y=endGun.y*(float)cos(a)-endGun.z*(float)sin(a);
	p.z=endGun.y*(float)sin(a)+endGun.z*(float)cos(a);
	p+=posGun;

	a=-DEG2RAD(TurretAngle);
	p2.x=p.x*(float)cos(a)-p.z*(float)sin(a);
	p2.y=p.y;
	p2.z=p.x*(float)sin(a)+p.z*(float)cos(a);
	p2+=posTurret;

	a=-DEG2RAD(Angle);
	p.x=p2.x*(float)cos(a)-p2.z*(float)sin(a);
	p.y=p2.y;
	p.z=p2.x*(float)sin(a)+p2.z*(float)cos(a);
	p+=posTank;
	p+=Pos;

	*pos=p;
}

void HTank::GetGunDir(HPoint3D *dir)
{
	HPoint3D p,p2;
	float a;
	a=DEG2RAD(GunAngle);
	p.x=0;
	p.y=-(float)sin(a);
	p.z=(float)cos(a);

	a=-DEG2RAD(TurretAngle);
	p2.x=p.x*(float)cos(a)-p.z*(float)sin(a);
	p2.y=p.y;
	p2.z=p.x*(float)sin(a)+p.z*(float)cos(a);

	a=-DEG2RAD(Angle);
	p.x=p2.x*(float)cos(a)-p2.z*(float)sin(a);
	p.y=p2.y;
	p.z=p2.x*(float)sin(a)+p2.z*(float)cos(a);

	*dir=p;
}

void HTank::ApplyExplode(const HPoint3D &Impact, float force)
{
	HPoint3D v=Pos-Impact;
	float l2=v.Length2();
	v.y*=4.0f;
	Vit+=v*(force/(l2));
}
