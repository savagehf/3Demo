#ifndef __HSCENE_H__
#define __HSCENE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HObject.h"
#include "HBBox.h"
#include "HTank.h"
#include "HMissile.h"
#include "HParticule.h"
#include "HTankAI.h"

#ifdef __HSCENE__
#define SCENE_API 
#else
#define SCENE_API extern
#endif

//SCENE_API HObject oTank;
//SCENE_API HObject oTankTurret;
//SCENE_API HObject oTankGun;

#define NTANK 1

//SCENE_API HObject oSol;
//SCENE_API HBBox   bbSol;
SCENE_API HObject oArena;
SCENE_API HBBox   bbArena;
SCENE_API HObject oSky;

SCENE_API HTank TTank[NTANK];
SCENE_API HTankAI * TTankAI[NTANK];
SCENE_API HTank *PlayerTank;

SCENE_API HMissile *lMissiles
#ifdef __HSCENE__
=NULL
#endif
;

SCENE_API HPartSyst PartSyst;

SCENE_API void NewMissile(HTank *tank=PlayerTank, float speed=100.0f);

SCENE_API void UpdateMissile(float t,float dt);

SCENE_API void AffMissile();

SCENE_API void FreeMissile();


#endif
