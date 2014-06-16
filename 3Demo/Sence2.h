//////////////////////////////////////////////////////////////////////////
//Create Data: 2014Äê5ÔÂ24ÈÕ
//Refacotr: Lee
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Texture.h"
#include "Point3D.h"
#include "trilist.h"

#define TIMER_DRAW_EXP		200
#define TIMER_EXP_OVER		201
#define TIMER_DRAW_PLANE	202



#define MAP_X       32        // size of map along x-axis
#define MAP_Z       32        // size of map along z-axis
#define MAP_SCALE   25.0f     // the scale of the terrain map
#define PI          3.14159
#define NUM_CACTI   40

class CSence2
{
public:
	CSence2(HWND hWnd);
	~CSence2();

	BOOL    Init();
	BOOL    Draw();

	void	OnTimer(UINT nID);
	void	OnKeydown(WPARAM wPara);
	void	OnKeyUp(WPARAM wPara);
	void	Resize(int width, int height);
	void	ResetExp();
	void	SetStartExp(BOOL bStart);
	void	SetStartPlane(BOOL bStart);
protected:
	BOOL	InitOpenGL();
	void	InitLight();
	void    LoadTexture(char *filename, GLuint &texture);
	float   GetHeight(float x, float z);
	void    DrawCacti();
	void    DrawSand();
	void    InitializeTerrain();
	BOOL    Cleanup();
	BOOL	ReleaseRes();
	unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	unsigned char *LoadBitmapFileWithAlpha(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);

	//explosion
	void InitExplosion();
	void calcMatrix(void);
	void buildRot(float theta, float x, float y, float z, float m[16]);
	void exploder(float x, float y, float z, float size, 
		float intensity, float opacity, float delay, float scale);
	void explode(void);
	void DrawExplosion();

	//plane model
	BOOL LoadPlaneModel();
	void DrawPlane();
	void DrawStaticPlaneAndSignal();
	void DrawClippane1();
	void DrawClippane2();
	void DrawClippane3();
	void DrawClippane4();
	void DrawSignal(Point3D start, Point3D end);
	void drawCNString(float x, float y, float z, const char* str);
	void selectFont(int size, int charset, const char* face) ;

protected:
	GLuint    m_nSand;     // sand texture
	GLuint    m_nCactus;   // cactus texture

	// terrain data
	float   m_nArrTerrain[MAP_X * MAP_Z][3];   // heightfield terrain data (0-255); 256x256
	GLuint  m_nArrIndex[MAP_X * MAP_Z * 2];    // vertex array
	float   m_nArrColor[MAP_X * MAP_Z][3];     // color array
	float   m_nArrTexcoord[MAP_X * MAP_Z][2];  // tex coord array

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


	HDC	  m_hDC;
	HGLRC m_hrc;
	HWND  m_hWnd;

	BOOL  m_keys[256];

};