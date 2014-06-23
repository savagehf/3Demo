#pragma once

#include "include/gl/glaux.h"
#include "Texture.h"
#include "trilist.h"
#include "ComDefinition.h"
#include "Point3D.h"
#include <vector>

using namespace std;

class CSence3
{
public:
	CSence3(HWND hWnd);
	~CSence3();
	enum EState
	{
		eState_Free					= 0,	//自由飞行状态
		eState_Route_1				= 1,	//任务1的闪烁线路阶段
		eState_Coll_Data_1			= 2,	//任务1的采集数据阶段
		eState_Route_2				= 3,	//任务2的闪烁路线阶段
		eState_Coll_Data_2			= 4,	//任务2的采集数据阶段
		eState_DrawMaxPane_1		= 5,	//任务1的最大值切面
		eState_DrawMaxPane_2		= 6,	//任务2的最大值切面
	};
	//terrain.
	BOOL Init();
	void Draw();
	void OnTimer(UINT nID);
	void OnKeyDown(WPARAM nChar);
	void OnKeyUp(WPARAM nChar);
	void Resize(int width, int height);

	void ResetExp();
	void SetStartExp(BOOL bStart);
	void SetStartPlane(BOOL bStart);

	//说明：开始任务一的绘制，会分解：
	//1.轨迹线闪烁绘制，2.飞行绘制，3.暂停+信号线绘制，4.终点到了，就停止绘制，不考虑循环了
	//如果在进行任务二绘制，那么任务一就只绘制一个平面了。
	void StartFlyTask1();

	//说明：开始任务二的绘制，分解如下：
	//1.轨迹线的少说绘制，2.飞行绘制，3.暂停+信号线绘制，4.终点到了，就停止绘制。
	//如果结束了，就绘制最终的静止状态的飞行平面。
	void StartFlyTask2();

	//模拟切面，计算脏弹位置，并显示闪烁脏弹位置
	void CalcFirePostion();
	
	

protected:
	BOOL InitOpenGL();
	void InitLight();
	void SetLogicalPalette();
	BOOL LoadPlaneModel();
	BOOL LoadSenceTex();
	void InitTerrain();
	AUX_RGBImageRec* LoadBMP(char *Filename);

	//Explosion.
	void InitExplosion();
	void calcMatrix(void);
	void buildRot(float theta, float x, float y, float z, float m[16]);
	void exploder(float x, float y, float z, float size, 
		float intensity, float opacity, float delay, float scale);
	void explode(void);
	void DrawExplosion();
	void DrawTerrain();
	void DrawSignal(Point3D start, Point3D end);
	void selectFont(int size, int charset, const char* face) ;
	void drawCNString(float x, float y, float z, const char* str);
	
	void DrawFlyFree();					//自由飞行
	void DrawRoute1();					//第一阶段的路线闪烁
	void DrawCollData1();				//第一阶段的数据采集
	void DrawRoute2();					//第二接单的路线闪烁
	void DrawCollData2();				//第二阶段的数据采集
	void DrawCalcMaxPane1();
	void DrawCalcMaxPane2();
	void DrawRoute1Points();
	void DrawRoute2Points();
	
	//void DrawStaticPlaneAndSignal();	//飞机静止状态下采集数据的信号模拟。
	//void DrawClippane1();
	void DrawRoutePane1();
	void DrawRoutePane2();
	void DrawMaxPane1();
	void DrawMaxPane2();
	void DrawBlingLine();
	void DrawBlingBomb();
	
	void SendDataToChart1(float fCurPos, float fDensity = 0);
	void SendDataToChart2(float fCurPos, float fDensity = 0);
	void CleanUp();
	BOOL ReleaseRes();

protected:
	GLfloat ABS(GLfloat A)
	{
		if (A < 0)
			A = -A; 
		return A;
	}


protected:

	HWND  m_hWnd;
	HDC   m_hDC;
	HGLRC m_hrgc;
	HPALETTE m_hPalette;		//OpenGL调色板

	//Init gl sence.
	GLUquadricObj *m_pQuadratic;
	GLuint	m_arrTexture[5];

	float corners[30][3];
	float traj[10][3];
	GLuint texnames[2];
	CTexture m_Texture;

	//explosion
	float m_fRotate ;
	float m_fOpacity;
	float m_fIntensity;
	float m_fSize;
	float m_fDelta;
	float m_fScale;


	//plane model
	CTriList m_planeModel;
	BOOL m_bPlaneLoaded;

	EState m_eState;		
	int   m_eDir;				//方向控制
	float m_fFlyStep;			//每次飞行的步长
	BOOL	m_bColorChange;		//更改线条颜色
	
	vector<float> m_vecRoute1Points;
	vector<float> m_vecRoute2Points;

	//绘制信号线
	float m_fSignalStep;


// 	BOOL m_bDrawPane1;
 	BOOL m_bDrawPane2;	
 	BOOL m_bDrawPane3;
// 	BOOL m_bDrawPane4;

	//draw state flag.
	BOOL m_bStartExplosion;
	BOOL m_bStartCollData;
	BOOL m_bExplover;
	int  m_nExplorCount;
};