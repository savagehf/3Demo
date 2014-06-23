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
		eState_Free					= 0,	//���ɷ���״̬
		eState_Route_1				= 1,	//����1����˸��·�׶�
		eState_Coll_Data_1			= 2,	//����1�Ĳɼ����ݽ׶�
		eState_Route_2				= 3,	//����2����˸·�߽׶�
		eState_Coll_Data_2			= 4,	//����2�Ĳɼ����ݽ׶�
		eState_DrawMaxPane_1		= 5,	//����1�����ֵ����
		eState_DrawMaxPane_2		= 6,	//����2�����ֵ����
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

	//˵������ʼ����һ�Ļ��ƣ���ֽ⣺
	//1.�켣����˸���ƣ�2.���л��ƣ�3.��ͣ+�ź��߻��ƣ�4.�յ㵽�ˣ���ֹͣ���ƣ�������ѭ����
	//����ڽ�����������ƣ���ô����һ��ֻ����һ��ƽ���ˡ�
	void StartFlyTask1();

	//˵������ʼ������Ļ��ƣ��ֽ����£�
	//1.�켣�ߵ���˵���ƣ�2.���л��ƣ�3.��ͣ+�ź��߻��ƣ�4.�յ㵽�ˣ���ֹͣ���ơ�
	//��������ˣ��ͻ������յľ�ֹ״̬�ķ���ƽ�档
	void StartFlyTask2();

	//ģ�����棬�����൯λ�ã�����ʾ��˸�൯λ��
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
	
	void DrawFlyFree();					//���ɷ���
	void DrawRoute1();					//��һ�׶ε�·����˸
	void DrawCollData1();				//��һ�׶ε����ݲɼ�
	void DrawRoute2();					//�ڶ��ӵ���·����˸
	void DrawCollData2();				//�ڶ��׶ε����ݲɼ�
	void DrawCalcMaxPane1();
	void DrawCalcMaxPane2();
	void DrawRoute1Points();
	void DrawRoute2Points();
	
	//void DrawStaticPlaneAndSignal();	//�ɻ���ֹ״̬�²ɼ����ݵ��ź�ģ�⡣
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
	HPALETTE m_hPalette;		//OpenGL��ɫ��

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
	int   m_eDir;				//�������
	float m_fFlyStep;			//ÿ�η��еĲ���
	BOOL	m_bColorChange;		//����������ɫ
	
	vector<float> m_vecRoute1Points;
	vector<float> m_vecRoute2Points;

	//�����ź���
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