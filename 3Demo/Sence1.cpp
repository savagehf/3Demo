#include "stdafx.h"
#include "Sence1.h"
#include "Utility.h"
#include "3DSREADER.H"
#include "CommonDef.h"
#include <time.h>
#include "MainFrm.h"

//tank particule...
#include "Hdef.h"
#include "Hmissile.h"
#include "Hparticule.h"
#include "Htank.h"
#include "Hscene.h"
#include "typdefi.h"
#include "ComDefinition.h"


//////////////////////////////////////////////////////////////////////////
#define START_X_POS			-8
#define MAX_X_POS			8
#define START_Z_POS			-20
#define MAX_Z_POS			-36
#define START_Y_POS			6.0
#define SCALE_FACTOR		0.0002/*0.05*/


//////////////////////////////////////////////////////////////////////////
//Tank game particule...
// compiled array extensions
PFNGLLOCKARRAYSEXTPROC    glLockArraysEXT = 0;
PFNGLUNLOCKARRAYSEXTPROC  glUnlockArraysEXT = 0;
unsigned long g_SysTime = 0;
static HGLRC g_hRC;
HDC g_hDC;
static LARGE_INTEGER g_TimeFreq, g_TimeStart;
bool g_bResReady = false;
bool g_bFire = false;


//////////////////////////////////////////////////////////////////////////
//for DrawProgress.
const int MAX = 1000; // LENGTH AND WIDTH OF MATRIX(2D array)
const int skyMAX = 50; // LENGTH AND WIDTH OF MATRIX(2D array)

vertex v1,v2,t1,t2,t3;
vertex n1[MAX][MAX];
vertex n2[MAX][MAX];
vertex n3[MAX][MAX];
vertex n4[MAX][MAX];
vertex field[MAX][MAX];
vertex sky[skyMAX][skyMAX];
float alpha[MAX][MAX];

GLfloat xtrans = MAX/2;
GLfloat xptrans = 0;
GLfloat ytrans = 0;
GLfloat yptrans = 0;
GLfloat ztrans = MAX/2;
GLfloat zptrans = 0;

float Progress = 0;
//////////////////////////////////////////////////////////////////////////


GLuint	fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };	// Storage For Three Types Of Fog
GLfloat	fogColor[4] = {0.9019f, 0.8588f, 0.7882f, 1};		// Fog Color


//////////////////////////////////////////////////////////////////////////
bool    multitexture = TRUE;  // Wireframe ON/OFF

CSence1::CSence1(HWND hWnd)
{
	m_hWnd = hWnd;
	m_hDC  = NULL;
	m_hrgc = NULL;
	m_hPalette = NULL;

	//load sence
	m_hDC=NULL;			
	m_pQuadratic = NULL;

	m_bPlaneLoaded = FALSE;
	m_fSignalStep = 0.0;
	m_fFlyStep = 0.0;
	m_eDir = 0;

	m_bDrawPane1 = FALSE;
	m_bDrawPane2 = FALSE;
	m_bDrawPane3 = FALSE;
	m_bDrawPane4 = FALSE;
	m_bStartCollData  = FALSE;
	m_bStartExplosion = FALSE;
	m_bIsGettingData = FALSE;
	

	m_bOnce = true;
	

}
CSence1::~CSence1()
{
	CleanUp();
}

BOOL CSence1::LoadPlaneModel()
{
	//没有纹理的tdrs模型
	C3dsReader Loader;
	if (m_planeModel.getNumObjects() > 0)
	{
		m_planeModel.removeAllObjects();
	}
	m_bPlaneLoaded = Loader.Reader("ModelData\\F16plane.3DS", &m_planeModel);
	m_planeModel.doAfterMath();

	return m_bPlaneLoaded;
}


GLfloat	xrot=0;				// X Rotation
GLfloat	yrot=0;				// Y Rotation
GLfloat	zrot=0;				// Y Rotation
GLfloat Speed;

GLfloat XPOS = -MAX/2;
GLfloat ZPOS = -MAX/2;
GLfloat XP=0;
GLfloat ZP=0;

GLfloat sceneroty;
GLfloat heading;
GLfloat zprot;
GLfloat yptrans2 = 0;

int quality = 4;

GLfloat H = 0;
GLfloat angle;

GLfloat xdist;
GLfloat zdist;
GLfloat Hypot;

float multiplier = 360/(3.14159*2); // multiplier is necessary for conversion from 360 degrees.



void CSence1::CleanUp()
{
	if (NULL != m_hPalette)
	{
		DeleteObject(m_hPalette);
	}
	for (int i=0; i<3; i++)
	{
		if (m_arrTexture[i] > 0)
		{
			glDeleteTextures(1, &m_arrTexture[i]);
		}
	}
	
	KillTimer(m_hWnd, TIMER_DRAW_PLANE);
	KillTimer(m_hWnd, TIMER_EXP_OVER);
	//KillTimer(m_hWnd, TIMER_GET_DATA);


	ReleaseRes();
}

BOOL CSence1::ReleaseRes()
{
	// if we have an RC, release it
	if (m_hrgc)
	{
		// release the RC
		if (!wglMakeCurrent(NULL,NULL))
		{
			// MessageBox(NULL, "Unable to release rendering context", "Error", MB_OK | MB_ICONINFORMATION);
		}

		// delete the RC
		if (!wglDeleteContext(m_hrgc))
		{
			//MessageBox(NULL, "Unable to delete rendering context", "Error", MB_OK | MB_ICONINFORMATION);
		}

		m_hrgc = NULL;
	}

	// release the DC if we have one
	if (m_hDC != NULL && !ReleaseDC(m_hWnd, m_hDC))
	{
		//MessageBox(NULL, "Unable to release device context", "Error", MB_OK | MB_ICONINFORMATION);
		m_hDC = NULL;
	}

	return TRUE;
}


void CSence1::DrawTerrain()									// Here's Where We Do All The Drawing and Animation
{ 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	int i;    
	int i2;    

	if (XPOS < -MAX+80) XPOS = -MAX+80; 
	else if(XPOS > -80) XPOS = -80;
	if (ZPOS < -MAX+80) ZPOS = -MAX+80; 
	else if(ZPOS > -80) ZPOS = -80;
	xtrans = -XPOS;
	ztrans = -ZPOS;

	zprot*=0.975f;
	GLfloat yrot2=sceneroty;
	heading += zprot/2;
	yrot = heading;

	sceneroty = 360.0f - yrot;

	H += (sceneroty-yrot2);   
	if (H > 360) H = 0;
	else if (H < 0) H = 360;

	int xpos = MAX-int(xtrans);
	int zpos = MAX-int(ztrans);

	// CALCULATE ALTITUDE ACCORDING TO LANDSCAPE
	GLfloat GROUNDLEVEL = -((field[xpos-1][zpos-1].y*3)
		+(field[xpos][zpos-1].y*4)
		+(field[xpos+1][zpos-1].y*3)

		+(field[xpos-1][zpos].y*4)               
		+(field[xpos][zpos].y*5)
		+(field[xpos+1][zpos].y*4)

		+(field[xpos-1][zpos+1].y*3)               
		+(field[xpos][zpos+1].y*4)
		+(field[xpos+1][zpos+1].y*3))/(33);

	yptrans +=  GROUNDLEVEL - ytrans;
	yptrans *=0.95f;

	ytrans += yptrans/750+yptrans2/50;      
	ytrans *= 0.999f;

	// CALCULATE YOUR HORIZONTAL SPEED
	Speed = hypot(XP,ZP);   

	glRotatef(-zprot*15,0,0,1);
	glRotatef(Speed*1.5,1,0,0);
	glRotatef(sceneroty,0,1,0);

	glTranslatef(xtrans,ytrans-3-Speed/2,ztrans);    

	GLfloat xtex; 
	GLfloat ytex; 
	GLfloat xtex2; 
	GLfloat ytex2; 
	GLfloat xtexa; 
	GLfloat ytexa; 
	GLfloat xtexa2; 
	GLfloat ytexa2; 

	int xrange1 = int(MAX-xtrans - 70); 
	int xrange2 = int(MAX-xtrans + 70);
	int zrange1 = int(MAX-ztrans - 70);
	int zrange2 = int(MAX-ztrans + 70);   

	if (quality != 1)
	{
		xrange1 /= quality;
		xrange1 *= quality;
		xrange2 /= quality;
		xrange2 *= quality;

		zrange1 /= quality;
		zrange1 *= quality;
		zrange2 /= quality;
		zrange2 *= quality;
	}    

	{   
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[0]);
		glColor4f(1,1,1,1.f);
		//glEnable(GL_CULL_FACE); 会导致飞机绘制为黑色
		// FIRST PASS OF DRAWING THE LANDSCAPE
		for (i = xrange1; i < xrange2; i+=quality)
		{   
			xtexa = (float(i)/MAX)*37;
			xtexa2 = (float(i+quality)/MAX)*37;
			int coord=i-MAX;

			for (i2 = zrange1; i2 < zrange2; i2+=quality)
			{                                     
				xdist = (MAX-xtrans)-i;   
				zdist = (MAX-ztrans)-i2;
				Hypot = hypot(xdist,zdist); 
				if (H > 180)
					angle = (acos(-zdist/Hypot)*multiplier)+180;
				else
					angle = acos(zdist/Hypot)*multiplier;

				if (Hypot < 70)
					if (ABS(angle-H) < 55 || Hypot < quality*4)  //Determines which triangle are in the estimated frustum
					{
						ytexa = (float(i2)/MAX)*37;
						ytexa2 = (float(i2+quality)/MAX)*37;
						int coord2=i2-MAX; 

						glBegin(GL_TRIANGLE_STRIP);
						//glNormal3f(n4[i][i2].x,n4[i][i2].y,n4[i][i2].z);
						glTexCoord2f(xtexa2,ytexa2);  glVertex3f(coord+quality,field[i+quality][i2+quality].y,  coord2+quality);
						//glNormal3f(n1[i][i2].x,n1[i][i2].y,n1[i][i2].z);
						glTexCoord2f(xtexa2,ytexa);   glVertex3f(coord+quality,field[i+quality][i2].y,coord2); 
						//glNormal3f(n3[i][i2].x,n3[i][i2].y,n3[i][i2].z);
						glTexCoord2f(xtexa,ytexa2);   glVertex3f(coord,field[i][i2+quality].y,coord2+quality); 
						//glNormal3f(n2[i][i2].x,n2[i][i2].y,n2[i][i2].z);
						glTexCoord2f(xtexa,ytexa);   glVertex3f(coord,field[i][i2].y,coord2); 
						glEnd();
					}       
			}   
		}
		// SECOND PASS OF DRAWING THE LANDSCAPE(multitexturing)
		if (multitexture)
		{    
			glBindTexture(GL_TEXTURE_2D, m_arrTexture[2]);
			glColor4f(1,.7,.6,.5f);
			for (i = xrange1; i < xrange2; i+=quality)
			{   
				xtexa = (float(i)/MAX)*45;
				xtexa2 = (float(i+quality)/MAX)*45;
				int coord=i-MAX;

				for (i2 = zrange1; i2 < zrange2; i2+=quality)
				{               
					if (alpha[i+quality][i2+quality] > 0 ||alpha[i+quality][i2] > 0 ||alpha[i][i2+quality] > 0 ||alpha[i][i2] > 0)//(alpha1 > 2 || alpha2 > 2 || alpha3 > 2 || alpha4 > 2)
					{       
						xdist = (MAX-xtrans)-i;   
						zdist = (MAX-ztrans)-i2;
						Hypot = hypot(xdist,zdist); 
						if (H > 180)
							angle = (acos(-zdist/Hypot)*multiplier)+180;
						else
							angle = acos(zdist/Hypot)*multiplier;

						if (Hypot < 70)
							if (ABS(angle-H) < 55 || Hypot < quality*4)
							{              
								ytexa = (float(i2)/MAX)*45;
								ytexa2 = (float(i2+quality)/MAX)*45;       
								int coord2=i2-MAX;


								glBegin(GL_TRIANGLE_STRIP);
								glColor4f(1,.7,.6,alpha[i+quality][i2+quality]);
								//glNormal3f(n4[i][i2].x,n4[i][i2].y,n4[i][i2].z);
								glTexCoord2f(xtexa2,ytexa2);  glVertex3f(coord+quality,field[i+quality][i2+quality].y,  coord2+quality);
								glColor4f(1,.7,.6,alpha[i+quality][i2]);
								//glNormal3f(n1[i][i2].x,n1[i][i2].y,n1[i][i2].z);
								glTexCoord2f(xtexa2,ytexa);   glVertex3f(coord+quality,field[i+quality][i2].y,coord2); 
								glColor4f(1,.7,.6,alpha[i][i2+quality]);
								//glNormal3f(n3[i][i2].x,n3[i][i2].y,n3[i][i2].z);
								glTexCoord2f(xtexa,ytexa2);   glVertex3f(coord,field[i][i2+quality].y,coord2+quality); 
								glColor4f(1,.7,.6,alpha[i][i2]);
								//glNormal3f(n2[i][i2].x,n2[i][i2].y,n2[i][i2].z);
								glTexCoord2f(xtexa,ytexa);   glVertex3f(coord,field[i][i2].y,coord2); 
								glEnd();

							}   

					}       
				}
			}   
		}  

		// SKYDOME GENERATED WITH A PRECISELY POSITIONED SPHERE(a shortcut to the real thing)
		glPushMatrix();
		glFogf(GL_FOG_START, MAX*2);							// Fog Start Depth
		glFogf(GL_FOG_END, MAX*15);							// Fog End Depth
		glColor4f(1,1,1,1.f);
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[1]);
		glTranslatef(-xtrans,-MAX*48,-ztrans);
		glRotatef(90,1,0,0);
		gluSphere(m_pQuadratic,MAX*50,32,32);
		glPopMatrix();
	}        


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);								// Keep Going
}

void CSence1::DrawClippane1()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
		glColor4f(0.5, 0.0, 0.5, 0.4);
		//glTranslatef();
		glBegin(GL_POLYGON);
			glVertex3f(START_X_POS, START_Y_POS-8.0, START_Z_POS);
			glVertex3f(MAX_X_POS,	START_Y_POS-8.0, START_Z_POS);
			glVertex3f(MAX_X_POS,	START_Y_POS,	 START_Z_POS);
			glVertex3f(START_X_POS, START_Y_POS,	 START_Z_POS);
		glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);
}

void CSence1::DrawClippane3()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
	glColor4f(0.0, 0.5, 0.5, 0.4);
	//glTranslatef();
	glBegin(GL_POLYGON);
		glVertex3f(START_X_POS, START_Y_POS-8.0, MAX_Z_POS);
		glVertex3f(MAX_X_POS,	START_Y_POS-8.0, MAX_Z_POS);
		glVertex3f(MAX_X_POS,	START_Y_POS,	 MAX_Z_POS);
		glVertex3f(START_X_POS, START_Y_POS,	 MAX_Z_POS);
	glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);
}
void CSence1::DrawClippane2()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
	glColor4f(0.5, 0.5, 0.0, 0.3);
		//glRotatef(-10.0f, 0.0f,1.0f,0.0f);
		glBegin(GL_POLYGON);
			glVertex3f(MAX_X_POS, START_Y_POS-8.0, START_Z_POS);
			glVertex3f(MAX_X_POS, START_Y_POS-8.0, MAX_Z_POS);
			glVertex3f(MAX_X_POS, START_Y_POS,	   MAX_Z_POS);
			glVertex3f(MAX_X_POS, START_Y_POS,	   START_Z_POS);
		glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
}
void CSence1::DrawClippane4()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
	glColor4f(0.5, 0.1, 0.1, 0.3);
	glBegin(GL_POLYGON);
		glVertex3f(START_X_POS, START_Y_POS-8.0, START_Z_POS);
		glVertex3f(START_X_POS, START_Y_POS-8.0, MAX_Z_POS);
		glVertex3f(START_X_POS, START_Y_POS,	   MAX_Z_POS);
		glVertex3f(START_X_POS, START_Y_POS,	   START_Z_POS);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
}

void CSence1::DrawPlane()
{
	glPushMatrix();

		if (m_eDir == 0)
		{
			//-x---->+x
			glTranslated (START_X_POS+m_fFlyStep, START_Y_POS, START_Z_POS);
		}
		else if (m_eDir == 1)
		{
			//+z---->-z
			glTranslated(MAX_X_POS, START_Y_POS, START_Z_POS-m_fFlyStep);
		}
		else if (m_eDir == 2)
		{
			//+x------>-x
			glTranslated(MAX_X_POS-m_fFlyStep, START_Y_POS, MAX_Z_POS);
		}
		else if (m_eDir == 3)
		{
			//-z----->+z
			glTranslated(START_X_POS, START_Y_POS, MAX_Z_POS + m_fFlyStep);
		}

		glEnable( GL_COLOR_MATERIAL); 
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

 		glRotatef(90*m_eDir, 0,1,0);
		glRotated(90.0,0.0,0.0,1.0);//2.绕飞机的箭头原点整个机身逆时针转90度。
		glRotated(90.0,0.0,1.0,0.0);//1.先绕Y转个90度得到侧面。
		glScalef(SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR);
 		if (m_bPlaneLoaded)
 		{
 			m_planeModel.drawGL();
 		}
		glDisable(GL_LIGHTING);
		
	glPopMatrix();

}

void CSence1::DrawStaticPlaneAndSignal()
{
	float xPos = 0.0f;
	float zPos = 0.0f;
	float yPos = START_Y_POS;

	//1.draw static plane.
	glPushMatrix();
		if (m_eDir == 0)
		{
			xPos = START_X_POS;
			zPos = START_Z_POS;
			glTranslated (START_X_POS, START_Y_POS, START_Z_POS);
			
		}
		else if (m_eDir == 1)
		{
			xPos = MAX_X_POS;
			zPos = START_Z_POS;
			glTranslated(MAX_X_POS, START_Y_POS, START_Z_POS);
		}
		else if (m_eDir == 2)
		{
			xPos = MAX_X_POS;
			zPos = MAX_Z_POS;
			glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
		}
		else if (m_eDir == 3)
		{
			xPos = START_X_POS;
			zPos = MAX_Z_POS;
			glTranslated(START_X_POS, START_Y_POS, MAX_Z_POS);
		}

		glEnable( GL_COLOR_MATERIAL); 
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

		glRotatef(90*m_eDir, 0,1,0);
		glRotated(90.0,0.0,0.0,1.0);//2.绕飞机的箭头原点整个机身逆时针转90度。
		glRotated(90.0,0.0,1.0,0.0);//1.先绕Y转个90度得到侧面。
		glScalef(0.0002,0.0002,0.0002);
		if (m_bPlaneLoaded)
		{
			m_planeModel.drawGL();
		}
		glDisable(GL_LIGHTING);

	glPopMatrix();

	//2.draw signal
	Point3D ptstart(xPos,yPos, zPos);
	Point3D ptend(xPos,yPos-6.0, zPos);
	glPushMatrix();
	DrawSignal(ptstart, ptend); 
	glPopMatrix();

	glPushMatrix();
	drawCNString(xPos, yPos, zPos, " 获取采样数据");
	glPopMatrix();
}
void CSence1::DrawSignal( Point3D start, Point3D end) 
{
	float signalLength = 0.1f;

	float deltX = end.x - start.x;
	float deltY = end.y - start.y;
	float deltZ = end.z - start.z;

	float x1 = start.x + deltX * m_fSignalStep;
	float y1 = start.y + deltY * m_fSignalStep;
	float z1 = start.z + deltZ * m_fSignalStep;

	float x2 = x1 + deltX * signalLength ;
	float y2 = y1 + deltY * signalLength ;
	float z2 = z1 + deltZ * signalLength ;


	glLineWidth(2);
	glEnable(GL_LINES) ;
	glLineStipple (1, 0x0F0F); 
	glColor3f(0.0, 0.0, 1.0);

	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f(x1,y1,z1);
	glVertex3f(x2,y2,z2);
	glEnd();
	glPopMatrix();

	glDisable(GL_LINES) ;
	glLineWidth(1);
}

void CSence1::drawCNString(float x, float y, float z, const char* str)
{
	selectFont(16, GB2312_CHARSET, "楷体_GB2312");

	glRasterPos3f(x, y, z);	

	int len, i;
	wchar_t* wstring;
	HDC hDC = m_hDC;//wglGetCurrentDC(); //获取显示设备
	GLuint list = glGenLists(1); //申请1个显示列表

	//计算字符的个数
	//如果是双字节字符的（比如中文字符），两个字节才算一个字符
	//否则一个字节算一个字符
	len = 0;
	for(i=0; str[i]!='\0'; ++i)
	{
		if( IsDBCSLeadByte(str[i]) )
			++i;
		++len;
	}
	// 将混合字符转化为宽字符
	int nLen = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, NULL, 0);
	wstring = (wchar_t*)malloc((len+1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';// 只是转义符,它本身的类型是wchar_t

	// 逐个输出字符
	for(i=0; i<len; ++i)
	{
		wglUseFontBitmapsW(m_hDC, wstring[i], 1, list);
		glCallList(list);
	}
	// 回收所有临时资源
	free(wstring);
	glDeleteLists(list, 1);
} 

void CSence1::selectFont(int size, int charset, const char* face) 
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_BOLD, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(/*wglGetCurrentDC()*/m_hDC, hFont);
	DeleteObject(hOldFont);
}

void CSence1::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == TIMER_DRAW_PLANE)
	{
		if (!m_bIsGettingData)
		{
			if (m_eDir == 0)
			{
				//-x---->+x
				if (m_fFlyStep + START_X_POS >= MAX_X_POS)
				{
					m_bIsGettingData = TRUE;
					//绘制4s的signal
					SetTimer(m_hWnd, TIMER_SIGNAL_EXPIRE, 6000, NULL);
					SetTimer(m_hWnd, TIMER_DRAW_SIGNAL, 100, NULL);
					m_eDir = 1;
					m_fFlyStep = 0.0;
					m_bDrawPane1 = TRUE;
				}
			}
			else if (m_eDir == 1)
			{
				//+z---->-z
				if (START_Z_POS +(-m_fFlyStep) <= MAX_Z_POS)
				{
					m_bIsGettingData = TRUE;
					//绘制4s的signal
					SetTimer(m_hWnd, TIMER_SIGNAL_EXPIRE, 6000, NULL);
					SetTimer(m_hWnd, TIMER_DRAW_SIGNAL, 100, NULL);

					m_eDir = 2;
					m_fFlyStep = 0.0f;
					m_bDrawPane2 = TRUE;
				}
			}
			else if (m_eDir == 2)
			{
				//+x---->-x
				if (MAX_X_POS-m_fFlyStep <= START_X_POS)
				{
					m_bIsGettingData = TRUE;
					//绘制4s的signal
					SetTimer(m_hWnd, TIMER_SIGNAL_EXPIRE, 6000, NULL);
					SetTimer(m_hWnd, TIMER_DRAW_SIGNAL, 100, NULL);

					m_eDir = 3;
					m_fFlyStep = 0.0f;
					m_bDrawPane3 = TRUE;
				}
			}
			else if (m_eDir == 3)
			{
				//-z---->+z
				if (MAX_Z_POS+m_fFlyStep >= START_Z_POS)
				{
					m_bIsGettingData = TRUE;
					//绘制4s的signal
					SetTimer(m_hWnd, TIMER_SIGNAL_EXPIRE, 6000, NULL);
					SetTimer(m_hWnd, TIMER_DRAW_SIGNAL, 100, NULL);

					m_eDir = 0;
					m_fFlyStep = 0.0f;
					m_bDrawPane4 = TRUE;
				}
			}	

			m_fFlyStep += 0.5f;
		}
	}
	else if (nIDEvent == TIMER_SIGNAL_EXPIRE)
	{
		//停止绘制signal,开始移动飞机
		m_bIsGettingData = FALSE;
		m_fSignalStep = 0.0f;
		KillTimer(m_hWnd, TIMER_SIGNAL_EXPIRE);
		KillTimer(m_hWnd, TIMER_DRAW_SIGNAL);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (NULL != pFrame)
		{
			pFrame->PostMessage(WM_ADD_ONE_DATA);
		}
	}
	else if (nIDEvent == TIMER_DRAW_SIGNAL)
	{
		if (m_fSignalStep >= 4.0)
		{
			m_fSignalStep = 0.0;
		}
		else
		{
			m_fSignalStep += 0.5f;
		}
	}
}
void CSence1::ResetExp()
{
	KillTimer(m_hWnd, TIMER_EXP_OVER);

	m_bStartExplosion = FALSE;
}

void CSence1::SetStartExp(BOOL bStart)
{
	m_bStartExplosion = bStart;
}
void CSence1::SetStartPlane(BOOL bStart)
{
	m_bStartCollData = bStart;
}

void CSence1::Setfire()
{
	g_bFire = true;
	SetTimer(m_hWnd,TIMER_FIRE_PARTICULE, 30, NULL);
}

//////////////////////////////////////////////////////////////////////////
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // size of structure
		1,                              // default version
		PFD_DRAW_TO_WINDOW |            // window drawing support
		PFD_SUPPORT_OPENGL |            // OpenGL support
		PFD_DOUBLEBUFFER,               // double buffering support
		PFD_TYPE_RGBA,                  // RGBA color mode
		24,                           // 32 bit color mode
		0, 0, 0, 0, 0, 0,               // ignore color bits, non-palettized mode
		0,                              // no alpha buffer
		0,                              // ignore shift bit
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // ignore accumulation bits
		16,                             // 16 bit z-buffer size
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main drawing plane
		0,                              // reserved
		0, 0, 0 };                      // layer masks ignored

		// Choose a pixel format that best matches that described in pfd
		nPixelFormat = ChoosePixelFormat(g_hDC, &pfd);
		//DebugOut( "PixelFormat Selected: %d\nBPP: %d\n", nPixelFormat, GetDeviceCaps(hDC, BITSPIXEL) );

		// Set the pixel format for the device context
		SetPixelFormat(g_hDC, nPixelFormat, &pfd);
}


void CSence1::ChangeViewSize( int width, int height )
{	
	// avoid divide by zero
	if (height==0)  
	{
		height=1;
	}
	// set the viewport to the new dimensions
	glViewport(0, 0, width, height);
	// select the projection matrix and clear it out
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// set the perspective with the appropriate aspect ratio
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 1000.0f);
	// select modelview matrix
	glMatrixMode(GL_MODELVIEW);
}


// May be useful to some people.
bool CSence1::appPreInit() // Called before OpenGL has been initialized.
{
	Test(HTank::Load());
	Test(HMissile::Load());
	Test(HParticule::Load());
	PartSyst.Alloc(5000);

	Test(oArena.LoadLWO("data/sol1.lwo"));
	Test(oSky.LoadLWO("data/sky.lwo"));

	oArena.CalcBBox(&bbArena);

	bbArena.b.y+=50.0f;

	HTank::SetArena(&bbArena);
	HMissile::SetArena(&bbArena);
	HMissile::SetPartSyst(&PartSyst);

	PlayerTank=&(TTank[0]);

	for (int i=0;i<NTANK;i++)
	{
		TTank[i].Pos.x=bbArena.a.x+(bbArena.b.x-bbArena.a.x)*rnd();
		TTank[i].Pos.z=bbArena.a.z+(bbArena.b.z-bbArena.a.z)*rnd();
		TTank[i].Angle=(float)RAD2DEG(atan2(-TTank[i].Pos.x+(bbArena.b.x+bbArena.a.x)*0.5f
			,-TTank[i].Pos.z+(bbArena.b.z+bbArena.a.z)*0.5f));

		if (i)
		{
			//lee
			//TTankAI[i]=new HTankAI_Random(&(TTank[i]));
		}
		else
		{
			TTankAI[i]=NULL;
		}
	}

	return true;
}
// Startup Stuff.
bool CSence1::appInit()	// Called right after the window is created, and OpenGL is initialized.
{
	// Initialize our system time.
	QueryPerformanceFrequency(&g_TimeFreq);
	QueryPerformanceCounter(&g_TimeStart);

	//	glGetExtension(&glExt_compiled_vertex_array);
	//  check for the compiled array extensions
	char *extList = (char *) glGetString(GL_EXTENSIONS);

	if (extList && strstr(extList, "GL_EXT_compiled_vertex_array"))
	{
		glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC) wglGetProcAddress("glLockArraysEXT");
		glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) wglGetProcAddress("glUnlockArraysEXT");
	}

	oArena.LoadTexture();
	oSky.LoadTexture();
	HTank::LoadTexture();
	HMissile::LoadTexture();
	HParticule::LoadTexture();

	// Create a simple camera/projection matrix.
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(90, (GLdouble)400/(GLdouble)300, 1.0, 1000.0);

	/*HColor4f ambient(0.0f,0.0f,0.3f);
	HColor4f clight(0.0f,0.0f,0.3f);
	float light0[4]={1.0f,8.0f,-36.0f,0.0f};
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,clight);
	glLightfv(GL_LIGHT0,GL_POSITION,light0);
	glEnable(GL_LIGHT0);*/

// 	float light1[4]={1.0f,-0.1f,0.5f,0.0f};
// 	glLightfv(GL_LIGHT1,GL_DIFFUSE,clight);
// 	glLightfv(GL_LIGHT1,GL_POSITION,light1);
// 	glEnable(GL_LIGHT1);

 	// And simple model matrix.
 	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();
 
 	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 
 	glShadeModel(GL_SMOOTH);
 	glEnable(GL_DEPTH_TEST);

	return true;
}

// Draw all the scene related stuff.
bool CSence1::RenderParticule()
{
	if (!g_bResReady)
	{
		return false;
	}

	// Get the current time, and update the time controller.
	LARGE_INTEGER TimeNow;
	QueryPerformanceCounter(&TimeNow);
	g_SysTime = (UInt32)((TimeNow.QuadPart- g_TimeStart.QuadPart)*1000/g_TimeFreq.QuadPart);

	static bool first=true;
	static double lastTime;
	double fTime = g_SysTime/1000.f;
	if (first)
	{
		lastTime=fTime;
	}
	int i;

	float t=(float)fTime;
	float dt=(float)(fTime-lastTime);

	//设置Y方向的发射角度。
	if (m_bOnce)
	{
		PlayerTank->RotateGun(-8.0);
		m_bOnce = false;
	}

	if(g_bFire)
	{
		NewMissile();
		g_bFire = false;
	}

	for(i=0;i<NTANK;i++)
	{
		if (TTankAI[i]!=NULL)
			TTankAI[i]->Update(t,dt);
		TTank[i].Update(t,dt);
	}
	UpdateMissile(t,dt);
	PartSyst.Update(t,dt);

	//glClearColor(0.1f,0.1f,0.1f,0.0f);  //lee add
	glClear( GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//draw plane
	if (m_bStartCollData)
	{
		if (m_bIsGettingData)
		{
			DrawStaticPlaneAndSignal();
		}
		else
		{
			DrawPlane();
		}

		if (m_bDrawPane1)
		{
			DrawClippane1();
		}
		if (m_bDrawPane2)
		{
			DrawClippane2();
		}
		if (m_bDrawPane3)
		{
			DrawClippane3();
		}
		if (m_bDrawPane4)
		{
			DrawClippane4();
		}
	}

	//draw sence
	//glDepthFunc(GL_LEQUAL);		
	//glDisable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glPushMatrix();
	DrawTerrain();
	glPopMatrix();
	//glEnable(GL_BLEND);


	//加了push就没有炮弹效果了。
	//glPushMatrix();
	//设置炮弹的发射位置。
	HPoint3D pCam(-16,8,-16);
	PlayerTank->SetCam(pCam);
	//glPopMatrix();

	glPushMatrix();
		if (m_bStartExplosion)
		{
			glPushMatrix();
				glEnable(GL_BLEND);
				AffMissile();
				float cam_angle=DEG2RAD(PlayerTank->GetCamAngle());
				HPoint3D dx((float)cos(cam_angle),0,-(float)sin(cam_angle));
				HPoint3D dy(0,1,0);
				PartSyst.Aff(dx,dy);
				glDisable(GL_BLEND);
			glPopMatrix();		
		}
	glPopMatrix();


	SwapBuffers(g_hDC);

	lastTime=fTime; 
	first=false;
	return true;
}

void CSence1::Init()
{
	appPreInit();

	g_hDC = GetDC(m_hWnd);
	// Select our precious pixel format.
	SetDCPixelFormat( g_hDC );
	// Yeppers, make something that OpenGL understands.
	g_hRC = wglCreateContext( g_hDC );
	wglMakeCurrent( g_hDC, g_hRC );


	InitLight();
	appInit();

	//load plane
	LoadPlaneModel();
	LoadSenceTex();
	InitTerrain();

	g_bResReady = true;
}

void CSence1::InitLight()
{
	//下面一行必须要加上，否则飞机的材质不能表现。
	glEnable( GL_COLOR_MATERIAL); 
	glShadeModel(GL_SMOOTH);

	//设置light0
	GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat diffuse[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat specular[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat position[] = {1.0f,8.0f,-36.0f,0.0f};//{ 1.0f,-8.0f,-100.0f,0.0 };//light0 位置
	GLfloat lmodel_ambient[] = { 0.2, 0.7, 0.5, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

}

void CSence1::InitTerrain()
{
	glClearColor(0.9019f, 0.8588f, 0.7882f, 1);				// Black Background

	m_pQuadratic=gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory) (NEW)
	gluQuadricNormals(m_pQuadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	gluQuadricTexture(m_pQuadratic, GL_TRUE);				// Create Texture Coords (NEW)


	int i,i2;     
	float hskyMAX = hypot(skyMAX,skyMAX);   
	for (i = 0; i < skyMAX; i++)
	{
		for (i2 = 0; i2 < skyMAX; i2++)
		{
			sky[i][i2].y=(hskyMAX/2-hypot(i-skyMAX/2,i2-skyMAX/2))*400-13200;  
		}  
	}

	//  GENERATE LANDSCAPE     
	for (i = 0; i < MAX; i++)
	{     
		for (i2 = 0; i2 < MAX; i2++)
		{
			if (i<75 || i2<75 || i>MAX-75 || i2>MAX-75)
				field[i][i2].y=15;   
			else
			{
				field[i][i2].y=(float(rand()%100)-50)/3; //Calculate the y coordinate on the same principle. 				
			}
		}
	}        
	//SMOOTH LANDSCAPE
	for (int cnt = 0; cnt < 8; cnt++)
	{       
		for (int t = 1; t < MAX-1; t++)
		{
			for (int t2_3 = 1; t2_3 < MAX-1; t2_3++)
			{
				field[t][t2_3].y = (field[t+1][t2_3].y+field[t][t2_3-1].y+field[t-1][t2_3].y+field[t][t2_3+1].y)/4;           
				if (rand()%250==1 && field[t][t2_3].y > 1 && cnt == 0) 
				{
					field[t][t2_3].y*=5;

					field[t-1][t2_3].y=field[t][t2_3].y/2;
					field[t+1][t2_3].y=field[t][t2_3].y/2;
					field[t][t2_3-1].y=field[t][t2_3].y/2;
					field[t][t2_3+1].y=field[t][t2_3].y/2;
					field[t-1][t2_3-1].y=field[t][t2_3].y/3;
					field[t+1][t2_3-1].y=field[t][t2_3].y/3;
					field[t+1][t2_3+1].y=field[t][t2_3].y/3;
					field[t-1][t2_3+1].y=field[t][t2_3].y/3;

				} 
				if (cnt == 5)
				{ 
					if (field[t][t2_3].y < -1 && field[t][t2_3].y > -1-.5) field[t][t2_3].y -= .45, field[t][t2_3].y *= 2;
					else if (field[t][t2_3].y > -1 && field[t][t2_3].y < -1+.5) field[t][t2_3].y += .45, field[t][t2_3].y *= 2;
				} 
			}
		}

	}

	//  GENERATE NORMALS for good looking lighting     
	for (i = 0; i < MAX; i++)
	{     
		for (i2 = 0; i2 < MAX; i2++)
		{
			// GENERATE ALPHA INTENSITIES FOR MULTITEXTURE
			if (field[i][i2].y > 0)
				alpha[i][i2]=field[i][i2].y;
			else
				alpha[i][i2]=0;              
		}
	}    
	//SMOOTH NORMALS AND ALPHA INTENSITIES 
	for (int cnt = 0; cnt < 3; cnt++)
	{    
		//SwapBuffers(m_hDC);
		//DrawProgress();   

		for (int t = 1; t < MAX-1; t++)
		{
			for (int t2_3 = 1; t2_3 < MAX-1; t2_3++)
			{
				alpha[t][t2_3] = (alpha[t+1][t2_3]+alpha[t][t2_3-1]+alpha[t-1][t2_3]+alpha[t][t2_3+1])/4;             
			}
		}
	}         

}
AUX_RGBImageRec* CSence1::LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}


BOOL CSence1::LoadSenceTex()									// Load Bitmap And Convert To A Texture
{
	BOOL Status=FALSE;								// Status Indicator
	AUX_RGBImageRec *TextureImage[1];				// Create Storage Space For The Textures
	memset(TextureImage,0,sizeof(void *)*1);		// Set The Pointer To NULL

	if (TextureImage[0]=LoadBMP("texture/asphalt.bmp"))	// Load Particle Texture
	{
		Status=TRUE;								// Set The Status To TRUE
		glGenTextures(1, &m_arrTexture[0]);				// Create One Texture

		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);           
	}

	if (TextureImage[0]=LoadBMP("texture/sky.bmp"))	// Load Particle Texture
	{
		Status=TRUE;								// Set The Status To TRUE
		glGenTextures(1, &m_arrTexture[1]);				// Create One Texture

		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);           
	}

	if (TextureImage[0]=LoadBMP("texture/stone.bmp"))	// Load Particle Texture
	{
		Status=TRUE;								// Set The Status To TRUE
		glGenTextures(1, &m_arrTexture[2]);				// Create One Texture

		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[2]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);           
	}

	if (TextureImage[0])							// If Texture Exists
	{
		if (TextureImage[0]->data)					// If Texture Image Exists
		{
			free(TextureImage[0]->data);			// Free The Texture Image Memory
		}
		free(TextureImage[0]);						// Free The Image Structure
	}

	return Status;									// Return The Status
}
