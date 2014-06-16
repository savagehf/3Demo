//-------------------------------------------------------------------
//  File:     HTank.cpp
//  Created:  September 09, 2000
//  Author:   Jeremie Allard (Hufo / N.A.A.)
//  Comments: Computer Controled Tanks.
//-------------------------------------------------------------------
#include "stdafx.h"
#include "std.h"
#include "HDef.h"
#include "HTankAI.h"
#include "HScene.h"

HTankAI_Random::HTankAI_Random(HTank *tk)
: HTankAI(tk), AngleSpeed(0), dAngleSpeed(0), Speed(0), dSpeed(0)
, Turret(0), dTurret(0), Gun(0), dGun(0), shoot_time(-10.0f)
{}


void HTankAI_Random::Update(float t,float dt)
{

	float frot=(float)pow(0.9f,dt);

	dAngleSpeed=dAngleSpeed*frot+(rnd()-0.5f)*dt*500.0f;
	AngleSpeed=AngleSpeed*frot+dAngleSpeed*dt;
	if (AngleSpeed<-90.0f) AngleSpeed=-90.0f;
	else if (AngleSpeed>90.0f) AngleSpeed=90.0f;

	if (AngleSpeed<=-45.0f || AngleSpeed>=45.0f)
		tank->RotateTank(AngleSpeed);
	else
		tank->RotateTank(0);

	dSpeed=dSpeed*frot+(rnd()-0.25f)*dt*10.0f;
	Speed=Speed*frot+dSpeed*dt;
	if (Speed<-5.0f) Speed=-5.0f;
	else if (Speed>10.0f) Speed=10.0f;

	if (Speed<=-1.0f || Speed>=1.0f)
		tank->DeplTank(Speed);
	else
		tank->DeplTank(0);

	dTurret=dTurret*frot+(rnd()-0.5f)*dt*500.0f;
	Turret=Turret*frot+dTurret*dt;
	if (Turret<-90) Turret=-90;
	else if (Turret>90) Turret=90;

	dGun=dGun*frot+(rnd()-0.5f)*dt*500.0f;
	Gun=Gun*frot+dGun*dt;
	if (Gun<-50) Gun=-50;
	else if (Gun>10) Gun=10;

	tank->RotateTurret(Turret-tank->TurretAngleGoal);
	tank->RotateGun(Gun-tank->GunAngleGoal);

	shoot_time+=dt;
	if (shoot_time>=0.0f)
	{
		NewMissile(tank);
		shoot_time=-rnd()*10.0f;
	}

}
