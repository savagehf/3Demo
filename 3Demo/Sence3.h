#pragma once

#include "include/gl/glaux.h"
#include "Texture.h"
#include "trilist.h"
#include "ComDefinition.h"
#include "Point3D.h"

class CSence3
{
public:
	CSence3(HWND hWnd);
	~CSence3();
	enum ETaskType
	{
		eTaskNone	= 0,
		eTask1		= 1,
		eTask2		= 2,
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
	void StartFlyTask1()
	{
		m_eTaskType = eTask1;
	}

	//说明：开始任务二的绘制，分解如下：
	//1.轨迹线的少说绘制，2.飞行绘制，3.暂停+信号线绘制，4.终点到了，就停止绘制。
	//如果结束了，就绘制最终的静止状态的飞行平面。
	void StartFlyTask2()
	{
		m_eTaskType = eTask2;
	}
	

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
	
	void DrawPlane();					//自由飞行
	void DrawPlaneInTask1();			//执行任务一
	void DrawPlaneInTask2();			//执行任务二
	void DrawStaticPlaneAndSignal();	//飞机静止状态下采集数据的信号模拟。
	void DrawClippane1();
	void DrawClippane2();
	void DrawClippane3();
	void DrawClippane4();

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

	//自由飞行阶段的控制
	int   m_eDir;				//方向控制
	float m_fFlyStep;			//每次飞行的步长
	BOOL m_bStartSimulateFly;	//TRUE=TASK1/2的模拟飞行 ：FALSE=自由飞行。
	
	//区分Task1、2
	ETaskType m_eTaskType;		
	
	//task1的飞行控制
	BOOL m_bDrawLine1;			//task1的飞行轨迹绘制
	BOOL m_bDrawTask1Pane;		//task1飞行结束了，绘制平面1


	//task2的飞行控制
	BOOL m_bDrawLine2;			//task2的飞行轨迹绘制
	BOOL m_bDrawTask2Pane;		//task2飞行结束了，绘制平面2
	

	//绘制信号线
	float m_fSignalStep;
	BOOL m_bIsGettingData;		//是否在暂停采集数据


	BOOL m_bDrawPane1;
	BOOL m_bDrawPane2;	
	BOOL m_bDrawPane3;
	BOOL m_bDrawPane4;

	//draw state flag.
	BOOL m_bStartExplosion;
	BOOL m_bStartCollData;
	BOOL m_bExplover;
	int  m_nExplorCount;
};