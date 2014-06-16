#ifndef __HTANK_H__
#define __HTANK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HPoint.h"
#include "HObject.h"

class HTankAI;

class HTank
{
//protected:
public:

	static HObject oTank;
	static HObject oTurret;
	static HObject oGun;
	static HPoint3D posTank; // tank pos offset
	static HPoint3D posTurret; // turret pos relative to tank
	static HPoint3D posGun; // gun pos relative to turret

	static HBBox bbTank;
	static HBBox bbTurret;
	static HBBox bbGun;

	static HBBox bbClip;

	static HPoint3D endGun; // gun out

	static HPoint3D pCenter; // tank out

	static HPoint3D Grav;

	static const float TurretAngleSpeed;
	static const float GunAngleSpeed;
	static const float TurretAngleMin;
	static const float TurretAngleMax;
	static const float GunAngleMin;
	static const float GunAngleMax;

// Tank hierarchy is:
// Tank
//  |
//  -- Turret
//      |
//      -- Gun

public:

	static bool Load();
	static bool LoadTexture();

	static void SetArena(HBBox *bb);

//protected:

	HPoint3D Pos;// tank pos
	HPoint3D Vit;// tank depl vector

	float Speed; // tank speed
	float Angle; // tank rotation angle
	float AngleSpeed; // tank rotation speed

	float TurretAngle; // turret angle
	float GunAngle; // gun angle
	float TurretAngleGoal; // turret angle goal (the angle the user want to be)
	float GunAngleGoal; // gun angle goal (the angle the user want to be)
	
	//float CoveredDist;

public:

	HTank();

	void Aff(); // render tank

	void SetCam(HPoint3D pcam_in_turret); // set camera transformation with camera relative to turret
	float GetCamAngle();

	void DeplTank(float speed);
	void RotateTank(float da);
	void RotateTurret(float da);
	void RotateGun(float da);

	void Update(float t,float dt);

	void GetGunPos(HPoint3D *pos);
	void GetGunDir(HPoint3D *dir);

	void ApplyExplode(const HPoint3D &Impact, float force=400.0f);

	friend class HTankAI;

};

#endif
