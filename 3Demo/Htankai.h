#ifndef __HTANKAI_H__
#define __HTANKAI_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HTank.h"

class HTankAI // abstract base class for computer controled tank
{
public:
	HTank *tank;

	HTankAI(HTank *tk) : tank(tk) {}

	virtual ~HTankAI() {}

	virtual void Update(float t,float dt)=0;

};

class HTankAI_Random : public HTankAI
{
public:

	float AngleSpeed;
	float dAngleSpeed;

	float Speed;
	float dSpeed;

	float Turret,dTurret;
	float Gun,dGun;

	float shoot_time;

	HTankAI_Random(HTank *tk);

	virtual void Update(float t,float dt);

};

#endif
