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
	
	void DrawPlane();
	void DrawStaticPlaneAndSignal();
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
	HPALETTE m_hPalette;		//OpenGLµ÷É«°å

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

	//draw plane and pane.
	int   m_eDir;
	float m_fSignalStep;
	float m_fFlyStep;
	BOOL m_bIsGettingData;
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