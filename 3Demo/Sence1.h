#pragma once

#include "include/gl/glaux.h"

#include "Texture.h"
#include "Point3D.h"
#include "trilist.h"
#include "ComDefinition.h"

enum eDirection
{
	enmDir_X_desc = 0,
	enmDir_Z_asec = 1,
	enmDir_X_asec = 2,
	enmDir_Z_desc = 3,
};

class CSence1
{
public:
	CSence1(HWND hWnd);
	~CSence1();

	//初始化particule.
	void Setfire();
	void Init();
	bool RenderParticule();
	void ChangeViewSize(int w, int h);
	bool appInit();
	bool appPreInit();

	void OnTimer(UINT nIDEvent);

	AUX_RGBImageRec* LoadBMP(char *Filename);
	BOOL LoadSenceTex();
	void InitTerrain();
	void InitLight();
	BOOL LoadPlaneModel();
	void ResetExp();
	void SetStartExp(BOOL bStart);
	void SetStartPlane(BOOL bStart);

	void DrawTerrain();
	void DrawPlane();
	void DrawStaticPlaneAndSignal();
	
	void DrawSignal(Point3D start, Point3D end);
	void drawCNString(float x, float y, float z, const char* str);
	void selectFont(int size, int charset, const char* face) ;
	void DrawClippane1();
	void DrawClippane2();
	void DrawClippane3();
	void DrawClippane4();

	void CleanUp();
	BOOL ReleaseRes();

protected:
	HWND  m_hWnd;
	HDC   m_hDC;
	HGLRC m_hrgc;
	HPALETTE m_hPalette;		//OpenGL调色板

	//Init gl sence.
	GLUquadricObj *m_pQuadratic;
	GLuint	m_arrTexture[3];


	//plane model
	CTriList m_planeModel;
	BOOL m_bPlaneLoaded;

	//draw plane and pane.
	int   m_eDir;
	float m_fSignalStep;
	float m_fFlyStep;
	BOOL m_bIsGettingData;
	BOOL m_bDrawPane1;
	BOOL m_bDrawPane2;	
	BOOL m_bDrawPane3;
	BOOL m_bDrawPane4;

	BOOL m_bStartExplosion;
	BOOL m_bStartCollData;

	//仅仅用来旋转下tank的向上的角度一次。
	bool m_bOnce;
};