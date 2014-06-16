#pragma once


#define TIMER_DRAW_EXP		200
#define TIMER_EXP_OVER		201
#define TIMER_DRAW_PLANE	202
#define TIMER_GET_DATA		203
#define TIMER_DRAW_SIGNAL	204
#define TIMER_SIGNAL_EXPIRE 205
#define TIMER_DRAW_PANE1	206
#define TIMER_DRAW_PANE2	207

//tank particule
#define TIMER_FIRE_PARTICULE 301

const float piover180 = 0.0174532925f;

struct vertex
{
	float x, y, z;
};

struct sDataItem
{
	float fLatitude;
	float fLogitude;
	float fHeight;
	float fDensity;
};