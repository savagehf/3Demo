#include "stdafx.h"
#include "Sence3.h"
#include <math.h>
#include "CommonDef.h"
#include "MainFrm.h"
#include "3DSREADER.H"

//#define FRAND   (((float)rand()-(float)rand())/RAND_MAX)

//////////////////////////////////////////////////////////////////////////
#define START_X_POS			-10
#define MAX_X_POS			6
#define START_Z_POS			-20
#define MAX_Z_POS			-36
#define START_Y_POS			6.0
#define SCALE_FACTOR		0.0002

#define OFFSET_X_MAXPANE2	7.5
#define OFFSET_X_MAXPANE1	3.0

#define ROTATE_MAX_PANE1	-10.0	//顺时针绕离圆点半径旋转10度
#define ROTATE_MAX_PANE2	10.0	//逆时针绕离圆点半径旋转10度

//////////////////////////////////////////////////////////////////////////
#define TIMER_ROUTE_1_OVER	3000	//绘制闪烁信号线。
#define TIMER_ROUTE_2_OVER	3001	//绘制闪烁信号线。
//#define TIMER_MAX_PLANE2	3002	//绘制第二个Max切面
#define TIMER_DIRTY_BOMB	3003	//绘制脏弹闪烁

#define TIMER_DYNC_PANE1	3010	//动态绘制pane1,y长度值到了就停止。
#define TIMER_DYNC_PANE2	3011	//动态绘制pane2,y长度值到了就停止。

#define TIMER_LINK_BOMBS	3012	//连接三个脏弹点



//////////////////////////////////////////////////////////////////////////
//for DrawProgress.
const int MAX_3 = 1000; // LENGTH AND WIDTH OF MATRIX(2D array)
const int skyMAX_3 = 50; // LENGTH AND WIDTH OF MATRIX(2D array)


vertex V1_3,v2_3,t1_3,t2_3,t3_3;
//vertex n1[MAX_3][MAX_3];
//vertex n2[MAX_3][MAX_3];
//vertex n3[MAX_3][MAX_3];
//vertex n4[MAX_3][MAX_3];
vertex field_3[MAX_3][MAX_3];
vertex sky_3[skyMAX_3][skyMAX_3];
float alpha_3[MAX_3][MAX_3];

GLfloat xtrans_3 = MAX_3/2;
GLfloat ytrans_3 = 0;
GLfloat yptrans_3 = 0;
GLfloat ztrans_3 = MAX_3/2;

//////////////////////////////////////////////////////////////////////////
//bool    wireframe = FALSE;  // Wireframe ON/OFF
bool    water_3 = FALSE;  // Wireframe ON/OFF
bool    multitexture_3 = TRUE;  // Wireframe ON/OFF
bool	lp_3;					// L Pressed? 

bool    wp_3;                 // W pressed?
// bool    mp;                 // M pressed?
// bool    aq;
//bool    sq;
//////////////////////////////////////////////////////////////////////////


//GLfloat	xrot=0;				// X Rotation
GLfloat	yrot_3=0;				// Y Rotation
//GLfloat	3_zrot=0;				// Y Rotation
GLfloat Speed_3;


GLfloat XPOS_3 = -MAX_3/2;
GLfloat ZPOS_3 = -MAX_3/2;
GLfloat XP_3=0;
GLfloat ZP_3=0;

GLfloat sceneroty_3;
GLfloat heading_3;


GLfloat zprot_3;
GLfloat yptrans23 = 0;

int quality_3 = 4;

GLfloat H_3 = 0;
GLfloat angle_3;

GLfloat xdist_3;
GLfloat zdist_3;
GLfloat Hypot_3;

int frames_3 = 0;
float Time1_3;
float Time2_3;
float DiffTime_3;

float multiplier_3 = 360/(3.14159*2); // multiplier_3 is necessary for conversion from 360 degrees.




CSence3::CSence3(HWND hWnd)
{

	//wenjie 2014年5月27日15:39:41
	KillTimer(m_hWnd, TIMER_DRAW_EXP);
	SetTimer(hWnd, TIMER_DRAW_EXP, 20, NULL);

	m_hWnd = hWnd;
	m_hDC  = NULL;
	m_hrgc = NULL;
	m_hPalette = NULL;

	//wenjie 2014年5月27日15:39:41
	//explosion
	m_fRotate = 0.f;
	m_fOpacity = 1.0f;
	m_fIntensity = 1.0f;
	m_fSize = .2f;
	m_fDelta = 0.f;
	m_fScale = 1.;

	//load sence		
	m_pQuadratic = NULL;
	m_bPlaneLoaded = FALSE;
	m_fSignalStep = 0.0;
	m_fFlyStep = 0.0;
	
	m_eDir = 0;
	m_eState = eState_Free;
	m_bColorChange = FALSE;

	//m_bDrawPane1 = FALSE;
	m_bDrawMorePos = FALSE;
	m_uLinkCount = 0;

	m_bDrawPane2 = FALSE;
	m_bDrawPane3 = FALSE;
	m_fOffsetPane1 = 0.0;
	m_fOffsetPane2 = 0.0;
	//m_bDrawPane4 = FALSE;
	//m_bIsGettingData = FALSE;
	//m_bStartSimulateFly = FALSE;
	//m_eTaskType = eTaskNone;

	m_bStartCollData  = FALSE;
	m_bStartExplosion = FALSE;
	m_bExplover = FALSE;
	m_nExplorCount = 0;

	m_vecRoute1Points.clear();
	m_vecRoute2Points.clear();

}

CSence3::~CSence3()
{
	CleanUp();
}



BOOL CSence3::InitOpenGL()
{
	if (!(m_hDC = GetDC(m_hWnd)))
	{
		AfxMessageBox("Unable to create device context");
		return FALSE;
	}

	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),    // pfd结构的大小 
		1,                                // 版本号 
		PFD_DRAW_TO_WINDOW |              // 支持在窗口中绘图 
		PFD_SUPPORT_OPENGL |              // 支持 OpenGL 
		PFD_DOUBLEBUFFER,                 // 双缓存模式 
		PFD_TYPE_RGBA,                    // RGBA 颜色模式 
		24,                               // 24 位颜色深度 
		0, 0, 0, 0, 0, 0,                 // 忽略颜色位 
		0,                                // 没有非透明度缓存 
		0,                                // 忽略移位位 
		0,                                // 无累加缓存 
		0, 0, 0, 0,                       // 忽略累加位 
		32,                               // 32 位深度缓存     
		0,                                // 无模板缓存 
		0,                                // 无辅助缓存 
		PFD_MAIN_PLANE,                   // 主层 
		0,                                // 保留 
		0, 0, 0                           // 忽略层,可见性和损毁掩模 
	}; 	

	int pixelformat;
	pixelformat = ::ChoosePixelFormat(m_hDC, &pfd);//选择像素格式
	::SetPixelFormat(m_hDC, pixelformat, &pfd);	//设置像素格式
	if(pfd.dwFlags & PFD_NEED_PALETTE)
	{
		SetLogicalPalette();	//设置逻辑调色板
	}

	//生成绘制描述表
	m_hrgc = ::wglCreateContext(m_hDC);
	//置当前绘制描述表
	::wglMakeCurrent(m_hDC, m_hrgc);

	return TRUE;
}

void CSence3::SetLogicalPalette(void)
{
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} logicalPalette = { 0x300, 256 };

	BYTE reds[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE greens[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE blues[] = {0, 85, 170, 255};

	for (int colorNum=0; colorNum<256; ++colorNum)
	{
		logicalPalette.aEntries[colorNum].peRed =
			reds[colorNum & 0x07];
		logicalPalette.aEntries[colorNum].peGreen =
			greens[(colorNum >> 0x03) & 0x07];
		logicalPalette.aEntries[colorNum].peBlue =
			blues[(colorNum >> 0x06) & 0x03];
		logicalPalette.aEntries[colorNum].peFlags = 0;
	}

	m_hPalette = CreatePalette ((LOGPALETTE*)&logicalPalette);
}

void CSence3::InitLight()
{
	//GLfloat LightAmbient[]=		{ 0.35f, 0.35f, 0.35f, 1.0f };//{ 1.0f, 1.0f, 1.0f, 1.0f };
	//GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };
	//GLfloat LightSpecular[]=	{ 1.0f, 1.0f, 1.0f, 1.0f };
	//GLfloat LightPosition[]=	{ 0.0f, 0.0f, 0.0f, 1.0f };

	//GLfloat ambient[] = { 1.0, 0.0, 0.0, 1.0 };
	//GLfloat diffuse[] = { 1.0, 0.0, 0.3, 1.0 };
	//GLfloat specular[] = { 1.0, 0.0, 0.6, 1.0 };
	//GLfloat position[] = { 10.0,1.0,-100.0,0.0 };//light0 位置
	////GLfloat lmodel_ambient[] = { 0.2, 0.7, 0.5, 1.0 };
	//glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	//glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	//glEnable(GL_DEPTH_TEST);
	//glClearColor(0, 0, 0, 1.0);


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


BOOL CSence3::Init(GLvoid)										// All Setup For OpenGL Goes Here
{
	InitOpenGL();

	//必须初始化，否则飞机材质不会绘制。
	InitLight();

	//load plane.
	LoadPlaneModel();

	InitExplosion();

	if (!LoadSenceTex())								// Jump To Texture Loading Routine
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}

	InitTerrain();

	return TRUE;										// Initialization Went OK
}

void CSence3::InitTerrain()
{
	glClearColor(0.9019f, 0.8588f, 0.7882f, 1);				// Black Background

	m_pQuadratic=gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory) (NEW)
	gluQuadricNormals(m_pQuadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	gluQuadricTexture(m_pQuadratic, GL_TRUE);				// Create Texture Coords (NEW)


	int i,i2;     
	float hskyMAX = hypot(skyMAX_3,skyMAX_3);   
	for (i = 0; i < skyMAX_3; i++)
	{
		for (i2 = 0; i2 < skyMAX_3; i2++)
		{
			sky_3[i][i2].y=(hskyMAX/2-hypot(i-skyMAX_3/2,i2-skyMAX_3/2))*400-13200;  
		}  
	}

	//  GENERATE LANDSCAPE     
	for (i = 0; i < MAX_3; i++)
	{     
		for (i2 = 0; i2 < MAX_3; i2++)
		{
			if (i<75 || i2<75 || i>MAX_3-75 || i2>MAX_3-75)
				field_3[i][i2].y=15;   
			else
			{
				field_3[i][i2].y=(float(rand()%100)-50)/3; //Calculate the y coordinate on the same principle. 				
			}
		}
	}        
	//SMOOTH LANDSCAPE
	for (int cnt = 0; cnt < 8; cnt++)
	{       
		for (int t = 1; t < MAX_3-1; t++)
		{
			for (int t2_3 = 1; t2_3 < MAX_3-1; t2_3++)
			{
				field_3[t][t2_3].y = (field_3[t+1][t2_3].y+field_3[t][t2_3-1].y+field_3[t-1][t2_3].y+field_3[t][t2_3+1].y)/4;           
				if (rand()%250==1 && field_3[t][t2_3].y > 1 && cnt == 0) 
				{
					field_3[t][t2_3].y*=5;

					field_3[t-1][t2_3].y=field_3[t][t2_3].y/2;
					field_3[t+1][t2_3].y=field_3[t][t2_3].y/2;
					field_3[t][t2_3-1].y=field_3[t][t2_3].y/2;
					field_3[t][t2_3+1].y=field_3[t][t2_3].y/2;
					field_3[t-1][t2_3-1].y=field_3[t][t2_3].y/3;
					field_3[t+1][t2_3-1].y=field_3[t][t2_3].y/3;
					field_3[t+1][t2_3+1].y=field_3[t][t2_3].y/3;
					field_3[t-1][t2_3+1].y=field_3[t][t2_3].y/3;

				} 
				if (cnt == 5)
				{ 
					if (field_3[t][t2_3].y < -1 && field_3[t][t2_3].y > -1-.5) field_3[t][t2_3].y -= .45, field_3[t][t2_3].y *= 2;
					else if (field_3[t][t2_3].y > -1 && field_3[t][t2_3].y < -1+.5) field_3[t][t2_3].y += .45, field_3[t][t2_3].y *= 2;
				} 
			}
		}

	}

	//  GENERATE NORMALS for good looking lighting     
	for (i = 0; i < MAX_3; i++)
	{     
		for (i2 = 0; i2 < MAX_3; i2++)
		{
			// GENERATE ALPHA INTENSITIES FOR MULTITEXTURE
			if (field_3[i][i2].y > 0)
				alpha_3[i][i2]=field_3[i][i2].y;
			else
				alpha_3[i][i2]=0;              
		}
	}    
	//SMOOTH NORMALS AND ALPHA INTENSITIES 
	for (int cnt = 0; cnt < 3; cnt++)
	{    
		//SwapBuffers(m_hDC);
		//DrawProgress();   

		for (int t = 1; t < MAX_3-1; t++)
		{
			for (int t2_3 = 1; t2_3 < MAX_3-1; t2_3++)
			{
				alpha_3[t][t2_3] = (alpha_3[t+1][t2_3]+alpha_3[t][t2_3-1]+alpha_3[t-1][t2_3]+alpha_3[t][t2_3+1])/4;             
			}
		}
	}         

}

BOOL CSence3::LoadSenceTex()									// Load Bitmap And Convert To A Texture
{
	BOOL Status=FALSE;								// Status Indicator
	AUX_RGBImageRec *TextureImage[1];				// Create Storage Space For The Textures
	memset(TextureImage,0,sizeof(void *)*1);		// Set The Pointer To NULL

	//if (TextureImage[0]=LoadBMP("texture/sand.bmp"))
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

	if (TextureImage[0]=LoadBMP("texture/Loading.bmp"))	// Load Particle Texture
	{
		Status=TRUE;								// Set The Status To TRUE
		glGenTextures(1, &m_arrTexture[3]);				// Create One Texture

		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[3]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);           
	}

	if (TextureImage[0]=LoadBMP("texture/Water.bmp"))	// Load Particle Texture
	{
		Status=TRUE;								// Set The Status To TRUE
		glGenTextures(1, &m_arrTexture[4]);				// Create One Texture

		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[4]);
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

AUX_RGBImageRec* CSence3::LoadBMP(char *Filename)				// Loads A Bitmap Image
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

void CSence3::Draw()
{
	glClearColor(0.1f,0.1f,0.1f,0.0f);  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

	//draw sence
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	DrawTerrain();
	glPopMatrix();

	//draw explosion
	if (m_bStartExplosion)
	{
		glPushMatrix();
		DrawExplosion();
		glPopMatrix();
	}
	glDisable(GL_BLEND);

	//draw plane
	//注意，对于有混合的效果，需要先绘制背景色，然后才绘制混合的切面
	//因为有深度测试，如果下面代码放在前面，就会出现背景不透明的效果
	if (m_bStartCollData)
	{
		glPushMatrix();
			
			switch(m_eState)
			{
			case eState_Free:
				{
					DrawFlyFree();
				}
				break;
			case eState_Route_1:
				{
					glPushMatrix();
						glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
						DrawRoute1();
					glPopMatrix();
				}
				break;
			case eState_Coll_Data_1:
				{
					glPushMatrix();
						glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
						DrawCollData1();
					glPopMatrix();
				}
				break;
			case eState_Route_2:
				{
					DrawRoute2();
				}
				break;
			case eState_Coll_Data_2:
				{
					DrawCollData2();
				}
				break;
			case eState_Draw_Dync_Pane1:
				{
					DrawDyncMaxPane1();
				}
				break;
// 			case eState_DrawMaxPane_1:
// 				{
// 					DrawCalcMaxPane1();
// 				}
// 				break;
			case eState_Draw_Dync_Pane2:
				{
					DrawDyncMaxPane2();
				}
				break;
			//case eState_DrawMaxPane_2:
			//	{
			//		DrawCalcMaxPane2();
			//	}
			//	break;
			case eState_DrawBomb:
				{
					glRotatef(-15.0, 0.0,1.0,0.0);
					glTranslatef(-5.0, 0.0,0.0);
					DrawLastPosition();
				}
				break;
			case eState_DrawConfirmBomb:
				{
					glRotatef(-15.0, 0.0,1.0,0.0);
					glTranslatef(-5.0, 0.0,0.0);
					DrawLastConfirmBomb();
				}
				break;
			default:
				break;
			}
		glPopMatrix();
	}


	::SwapBuffers(m_hDC);		//交互缓冲区
}
void CSence3::DrawTerrain()									// Here's Where We Do All The Drawing and Animation
{ 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	int i;    
	int i2;    

	if (XPOS_3 < -MAX_3+80) XPOS_3 = -MAX_3+80; 
	else if(XPOS_3 > -80) XPOS_3 = -80;
	if (ZPOS_3 < -MAX_3+80) ZPOS_3 = -MAX_3+80; 
	else if(ZPOS_3 > -80) ZPOS_3 = -80;
	xtrans_3 = -XPOS_3;
	ztrans_3 = -ZPOS_3;

	zprot_3*=0.975f;
	GLfloat yrot2=sceneroty_3;
	heading_3 += zprot_3/2;
	yrot_3 = heading_3;

	sceneroty_3 = 360.0f - yrot_3;

	H_3 += (sceneroty_3-yrot2);   
	if (H_3 > 360) H_3 = 0;
	else if (H_3 < 0) H_3 = 360;

	int xpos = MAX_3-int(xtrans_3);
	int zpos = MAX_3-int(ztrans_3);

	// CALCULATE ALTITUDE ACCORDING TO LANDSCAPE
	GLfloat GROUNDLEVEL = -((field_3[xpos-1][zpos-1].y*3)
		+(field_3[xpos][zpos-1].y*4)
		+(field_3[xpos+1][zpos-1].y*3)

		+(field_3[xpos-1][zpos].y*4)               
		+(field_3[xpos][zpos].y*5)
		+(field_3[xpos+1][zpos].y*4)

		+(field_3[xpos-1][zpos+1].y*3)               
		+(field_3[xpos][zpos+1].y*4)
		+(field_3[xpos+1][zpos+1].y*3))/(33);

	yptrans_3 +=  GROUNDLEVEL -ytrans_3;
	yptrans_3 *=0.95f;

	ytrans_3 += yptrans_3/750+yptrans23/50;      
	ytrans_3 *= 0.999f;

	// CALCULATE YOUR HORIZONTAL SPEED
	Speed_3 = hypot(XP_3,ZP_3);   

	glRotatef(-zprot_3*15,0,0,1);
	glRotatef(Speed_3*1.5,1,0,0);
	glRotatef(sceneroty_3,0,1,0);

	glTranslatef(xtrans_3,ytrans_3-3-Speed_3/2,ztrans_3);    

	GLfloat xtex; 
	GLfloat ytex; 
	GLfloat xtex2; 
	GLfloat ytex2; 
	GLfloat xtexa; 
	GLfloat ytexa; 
	GLfloat xtexa2; 
	GLfloat ytexa2; 

	int xrange1 = int(MAX_3-xtrans_3 - 70); 
	int xrange2 = int(MAX_3-xtrans_3 + 70);
	int zrange1 = int(MAX_3-ztrans_3 - 70);
	int zrange2 = int(MAX_3-ztrans_3 + 70);   

	if (quality_3 != 1)
	{
		xrange1 /= quality_3;
		xrange1 *= quality_3;
		xrange2 /= quality_3;
		xrange2 *= quality_3;

		zrange1 /= quality_3;
		zrange1 *= quality_3;
		zrange2 /= quality_3;
		zrange2 *= quality_3;
	}    

	{   
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[0]);
		glColor4f(1,1,1,1.f);
		//glEnable(GL_CULL_FACE); 
		// FIRST PASS OF DRAWING THE LANDSCAPE
		for (i = xrange1; i < xrange2; i+=quality_3)
		{   
			xtexa = (float(i)/MAX_3)*37;
			xtexa2 = (float(i+quality_3)/MAX_3)*37;
			int coord=i-MAX_3;

			for (i2 = zrange1; i2 < zrange2; i2+=quality_3)
			{                                     
				xdist_3 = (MAX_3-xtrans_3)-i;   
				zdist_3 = (MAX_3-ztrans_3)-i2;
				Hypot_3 = hypot(xdist_3,zdist_3); 
				if (H_3 > 180)
					angle_3 = (acos(-zdist_3/Hypot_3)*multiplier_3)+180;
				else
					angle_3 = acos(zdist_3/Hypot_3)*multiplier_3;

				if (Hypot_3 < 70)
					if (ABS(angle_3-H_3) < 55 || Hypot_3 < quality_3*4)  //Determines which triangle are in the estimated frustum
					{
						ytexa = (float(i2)/MAX_3)*37;
						ytexa2 = (float(i2+quality_3)/MAX_3)*37;
						int coord2=i2-MAX_3; 

						glBegin(GL_TRIANGLE_STRIP);
						//glNormal3f(n4[i][i2].x,n4[i][i2].y,n4[i][i2].z);
						glTexCoord2f(xtexa2,ytexa2);  glVertex3f(coord+quality_3,field_3[i+quality_3][i2+quality_3].y,  coord2+quality_3);
						//glNormal3f(n1[i][i2].x,n1[i][i2].y,n1[i][i2].z);
						glTexCoord2f(xtexa2,ytexa);   glVertex3f(coord+quality_3,field_3[i+quality_3][i2].y,coord2); 
						//glNormal3f(n3[i][i2].x,n3[i][i2].y,n3[i][i2].z);
						glTexCoord2f(xtexa,ytexa2);   glVertex3f(coord,field_3[i][i2+quality_3].y,coord2+quality_3); 
						//glNormal3f(n2[i][i2].x,n2[i][i2].y,n2[i][i2].z);
						glTexCoord2f(xtexa,ytexa);   glVertex3f(coord,field_3[i][i2].y,coord2); 
						glEnd();
					}       
			}   
		}
		// SECOND PASS OF DRAWING THE LANDSCAPE(multitexturing)
		if (multitexture_3)
		{    
			glBindTexture(GL_TEXTURE_2D, m_arrTexture[2]);
			glColor4f(1,.7,.6,.5f);
			for (i = xrange1; i < xrange2; i+=quality_3)
			{   
				xtexa = (float(i)/MAX_3)*45;
				xtexa2 = (float(i+quality_3)/MAX_3)*45;
				int coord=i-MAX_3;

				for (i2 = zrange1; i2 < zrange2; i2+=quality_3)
				{               
					if (alpha_3[i+quality_3][i2+quality_3] > 0 ||alpha_3[i+quality_3][i2] > 0 ||alpha_3[i][i2+quality_3] > 0 ||alpha_3[i][i2] > 0)//(alpha1 > 2 || alpha2 > 2 || alpha3 > 2 || alpha4 > 2)
					{       
						xdist_3 = (MAX_3-xtrans_3)-i;   
						zdist_3 = (MAX_3-ztrans_3)-i2;
						Hypot_3 = hypot(xdist_3,zdist_3); 
						if (H_3 > 180)
							angle_3 = (acos(-zdist_3/Hypot_3)*multiplier_3)+180;
						else
							angle_3 = acos(zdist_3/Hypot_3)*multiplier_3;

						if (Hypot_3 < 70)
							if (ABS(angle_3-H_3) < 55 || Hypot_3 < quality_3*4)
							{              
								ytexa = (float(i2)/MAX_3)*45;
								ytexa2 = (float(i2+quality_3)/MAX_3)*45;       
								int coord2=i2-MAX_3;


								glBegin(GL_TRIANGLE_STRIP);
								glColor4f(1,.7,.6,alpha_3[i+quality_3][i2+quality_3]);
								//glNormal3f(n4[i][i2].x,n4[i][i2].y,n4[i][i2].z);
								glTexCoord2f(xtexa2,ytexa2);  glVertex3f(coord+quality_3,field_3[i+quality_3][i2+quality_3].y,  coord2+quality_3);
								glColor4f(1,.7,.6,alpha_3[i+quality_3][i2]);
								//glNormal3f(n1[i][i2].x,n1[i][i2].y,n1[i][i2].z);
								glTexCoord2f(xtexa2,ytexa);   glVertex3f(coord+quality_3,field_3[i+quality_3][i2].y,coord2); 
								glColor4f(1,.7,.6,alpha_3[i][i2+quality_3]);
								//glNormal3f(n3[i][i2].x,n3[i][i2].y,n3[i][i2].z);
								glTexCoord2f(xtexa,ytexa2);   glVertex3f(coord,field_3[i][i2+quality_3].y,coord2+quality_3); 
								glColor4f(1,.7,.6,alpha_3[i][i2]);
								//glNormal3f(n2[i][i2].x,n2[i][i2].y,n2[i][i2].z);
								glTexCoord2f(xtexa,ytexa);   glVertex3f(coord,field_3[i][i2].y,coord2); 
								glEnd();

							}   

					}       
				}
			}   
		}  

		// SKYDOME GENERATED WITH A PRECISELY POSITIONED SPHERE(a shortcut to the real thing)
		glPushMatrix();
		glFogf(GL_FOG_START, MAX_3*2);							// Fog Start Depth
		glFogf(GL_FOG_END, MAX_3*15);							// Fog End Depth
		glColor4f(1,1,1,1.f);
		glBindTexture(GL_TEXTURE_2D, m_arrTexture[1]);
		glTranslatef(-xtrans_3,-MAX_3*48,-ztrans_3);
		glRotatef(90,1,0,0);
		gluSphere(m_pQuadratic,MAX_3*50,32,32);
		glPopMatrix();
		//glFogf(GL_FOG_START, 10.0f);						// Fog Start Depth
		//glFogf(GL_FOG_END, 60.0f);							// Fog End Depth
		//glEnable(GL_BLEND);


		if(water_3)
		{ 
			glPushMatrix();
			//glEnable(GL_CULL_FACE);
			glLoadIdentity();
			glRotatef(-zprot_3*15,0,0,1);
			glRotatef(Speed_3*1.5,1,0,0);
			glRotatef(sceneroty_3,0,1,0);

			glTranslatef(xtrans_3,ytrans_3-3-Speed_3/2,ztrans_3);    

			xtexa = (float(xrange1)/MAX_3)*50;
			xtexa2 = (float(xrange2)/MAX_3)*50;
			ytexa = (float(zrange1)/MAX_3)*50;
			ytexa2 = (float(zrange2)/MAX_3)*50;
			// WATER
			glBindTexture(GL_TEXTURE_2D, m_arrTexture[4]);
			glColor4f(1,1,1,.5f);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(xtexa2,ytexa2); glVertex3f(xrange2-MAX_3,-1,zrange2-MAX_3);
			glTexCoord2f(xtexa2,ytexa);  glVertex3f(xrange2-MAX_3,-1,zrange1-MAX_3); 
			glTexCoord2f(xtexa,ytexa2);  glVertex3f(xrange1-MAX_3,-1,zrange2-MAX_3); 
			glTexCoord2f(xtexa,ytexa);   glVertex3f(xrange1-MAX_3,-1,zrange1-MAX_3); 
			glEnd();
			glPopMatrix();
		}
	}        

	//这行千万不能要，否则飞机绘制不了
	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);								// Keep Going
}


void CSence3::InitExplosion(GLvoid)
{
	static unsigned *explosionimage,*smokeimage;
	static int width,height,components;
	explode();
	glGenTextures(2, texnames);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	explosionimage = m_Texture.read_texture("ModelData\\explosion0.rgba", &width, &height, &components);
	if (explosionimage == NULL) 
	{
		AfxMessageBox("Can't load image file ");
		exit(EXIT_FAILURE);
	} 
	if (components != 2 && components != 4)
		AfxMessageBox("texture should be an RGBA or LA image");
	glBindTexture(GL_TEXTURE_2D, texnames[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, explosionimage);

	smokeimage = m_Texture.read_texture("ModelData\\smoke.la", &width, &height, &components);
	if (explosionimage == NULL) 
	{
		AfxMessageBox("Can't load image file ");
		exit(EXIT_FAILURE);
	} 

	if (components != 2 && components != 4)
		AfxMessageBox("texture should be an RGBA or LA image");

	glBindTexture(GL_TEXTURE_2D, texnames[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, smokeimage);
}

void CSence3::explode(void)
{

	float startcorners[30][3] = {
		{-1.0, -1.0, -1.0},
		{-1.0, 1.0, -1.0},
		{1.0, 1.0, -1.0},
		{1.0, 1.0, -1.0},
		{1.0, -1.0, -1.0},
		{-1.0, -1.0, -1.0},

		{-1.0, -1.0, 1.0},
		{1.0, -1.0, 1.0},
		{1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0},
		{-1.0, 1.0, 1.0},
		{-1.0, -1.0, 1.0},

		{-1.0, 1.0, -1.0},
		{1.0, 1.0, -1.0},
		{1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0},
		{-1.0, 1.0, 1.0},
		{-1.0, 1.0, -1.0},

		{1.0, -1.0, -1.0},
		{1.0, 1.0, -1.0},
		{1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0},
		{1.0, -1.0, 1.0},
		{1.0, -1.0, -1.0},

		{-1.0, -1.0, -1.0},
		{-1.0, 1.0, -1.0},
		{-1.0, 1.0, 1.0},
		{-1.0, 1.0, 1.0},
		{-1.0, -1.0, 1.0},
		{-1.0, -1.0, -1.0}
	};

	float starttraj[10][3] = {
		{0.25, -0.25, -1.0},
		{-0.25, 0.25, -1.0},
		{0.25, -0.25, 1.0},
		{-0.25, 0.25, 1.0},
		{0.25, 1.0, -0.25},
		{-0.25, 1.0, 0.25},
		{0.25, -1.0, -0.25},
		{-0.25, -1.0, 0.25},
		{-1.0, 0.25, -0.25},
		{-1.0, -0.25, 0.25}};
		static int freeze = 0;
		int i,
			j;
		for (i = 0; i < 30; i++)
			for (j = 0; j < 3; j++)
				corners[i][j] = startcorners[i][j];

		for (i = 0; i < 10; i++)
			for (j = 0; j < 3; j++)
				traj[i][j] = starttraj[i][j];

		m_fSize = 0.f;
		m_fRotate = 0.f;
		m_fOpacity = 1.f;
		m_fIntensity = 1.f;
		m_fDelta = 0.f;
		m_fScale = 1.f;
}


void CSence3::exploder(float x, float y, float z, float size, float intensity, float opacity, float delay, float scale)
{
	if (size - delay <= 0.f)
		return;

	glPushMatrix();
	calcMatrix();
	glTranslatef(x, y, z);
	glScalef((size - delay) * scale, (size - delay) * scale, 1.);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texnames[0]);
	glColor4f(intensity, intensity, intensity, opacity);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);glVertex2f(-1.0, -1.0);
	glTexCoord2f(1.0, 0.0);glVertex2f(1.0, -1.0);
	glTexCoord2f(1.0, 1.0);glVertex2f(1.0, 1.0);
	glTexCoord2f(0.0, 1.0);glVertex2f(-1.0, 1.0);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void CSence3::buildRot(float theta, float x, float y, float z, float m[16])
{
	float d = x * x + y * y + z * z;
	float ct = cosf(RAD(theta)),
		st = sinf(RAD(theta));

	if (d > 0) 
	{
		d = 1 / d;
		x *= d;
		y *= d;
		z *= d;
	}
	m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0;
	m[4] = 0; m[5] = 1; m[6] = 0; m[7] = 0;
	m[8] = 0; m[9] = 0; m[10] = 1; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;

	m[0] = x * x + ct * (1 - x * x) + st * 0;
	m[4] = x * y + ct * (0 - x * y) + st * -z;
	m[8] = x * z + ct * (0 - x * z) + st * y;

	m[1] = y * x + ct * (0 - y * x) + st * z;
	m[5] = y * y + ct * (1 - y * y) + st * 0;
	m[9] = y * z + ct * (0 - y * z) + st * -x;

	m[2] = z * x + ct * (0 - z * x) + st * -y;
	m[6] = z * y + ct * (0 - z * y) + st * x;
	m[10] = z * z + ct * (1 - z * z) + st * 0;
}

void CSence3::calcMatrix(void)
{
	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	buildRot(-180 * atan2f(mat[8], mat[10]) / M_PI, 0, 1, 0, mat);
	glMultMatrixf(mat);
}

void CSence3::DrawExplosion()
{
	float normals[10][3] = {
		{0.0, 0.0, -1.0},
		{0.0, 0.0, -1.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, 1.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},
		{-1.0, 0.0, 0.0},
		{-1.0, 0.0, 0.0}};
		float axes[10][3] = {
			{-1.0, 1.0, 0.0},
			{1.0, -1.0, 0.0},
			{-1.0, 1.0, 0.0},
			{1.0, -1.0, 0.0},
			{1.0, 0.0, -1.0},
			{-1.0, 0.0, 1.0},
			{0.0, 1.0, -1.0},
			{0.0, -1.0, 1.0},
			{0.0, 1.0, -1.0},
			{0.0, -1.0, 1.0}};
			float omega[10] = {20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0};
			int i;

			gluLookAt(-sinf(RAD(0)) * 5.5, 0, cosf(RAD(0)) * 5.5, 0., 0., 0., 0., 1., 0.);
			glColor4f(0.f, .2f, 0.f, 1.f);

			glEnable(GL_LIGHTING);
			glPushMatrix();
			glColor3f(.3f, .3f, .3f);
			glPushMatrix();
			glTranslatef(-1.f, -1.f + .2f, -1.5f);
			glScalef(.2f, .2f, .2f);

			for (i = 0; i < 10; i++) 
			{
				float ax = (corners[3 * i][0] + corners[3 * i + 1][0] + corners[3 * i + 2][0]) / 3,
					ay = (corners[3 * i][1] + corners[3 * i + 1][1] + corners[3 * i + 2][1]) / 3,
					az = (corners[3 * i][2] + corners[3 * i + 1][2] + corners[3 * i + 2][2]) / 3;
			}
			glPopMatrix();

			glDisable(GL_LIGHTING);//如果允许灯光效果，amibient就会是InitLight中的淡绿色背景色。

			glTranslatef(-1.f, -1.f, -1.5f);
			//glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, texnames[1]);
			glDepthMask(0);
			//time++;
			//glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			exploder(0.f, 0.f, 0.f, m_fSize, m_fIntensity, m_fOpacity, 0.f, 2.0/*3.f*/);
			glEnable(GL_DEPTH_TEST);
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(1);
			glPopMatrix();
			//glDisable(GL_TEXTURE_2D);
}

void CSence3::DrawRoutePane2()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
	glColor4f(0.5, 0.5, 0.0, 0.4);
	glBegin(GL_POLYGON);
	glVertex3f(START_X_POS, START_Y_POS-10.0, MAX_Z_POS);
	glVertex3f(MAX_X_POS,	START_Y_POS-10.0, MAX_Z_POS);
	glVertex3f(MAX_X_POS,	START_Y_POS,	 MAX_Z_POS);
	glVertex3f(START_X_POS, START_Y_POS,	 MAX_Z_POS);
	glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);
}
void CSence3::DrawRoutePane1()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
	glColor4f(0.5, 0.0, 0.5, 0.3);

	glBegin(GL_POLYGON);
	glVertex3f(MAX_X_POS, START_Y_POS-10.0, START_Z_POS);
	glVertex3f(MAX_X_POS, START_Y_POS-10.0, MAX_Z_POS);
	glVertex3f(MAX_X_POS, START_Y_POS,	   MAX_Z_POS);
	glVertex3f(MAX_X_POS, START_Y_POS,	   START_Z_POS);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
}

void CSence3::DrawFlyFree()
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

		DrawPlane(90*m_eDir);

	glPopMatrix();

}

void CSence3::DrawPlane(float fAngle)
{
	glEnable( GL_COLOR_MATERIAL); 
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

	glRotatef(fAngle/*90*m_eDir*/, 0,1,0);
	glRotated(90.0,0.0,0.0,1.0);//2.绕飞机的箭头原点整个机身逆时针转90度。
	glRotated(90.0,0.0,1.0,0.0);//1.先绕Y转个90度得到侧面。
	glScalef(SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR);
	if (m_bPlaneLoaded)
	{
		m_planeModel.drawGL();
	}
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);	
}

void CSence3::DrawRoute1()
{
	//draw plane static
	glPushMatrix();
		glTranslated(MAX_X_POS, START_Y_POS, START_Z_POS);
		DrawPlane(90.0*1);
	glPopMatrix();

	//draw route.
	glPushMatrix();
		if (m_bColorChange)
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		else
		{
			glColor3f(0.0, 1.0, 0.0);
		}
		glLineWidth(2.0);
		
		
		glBegin(GL_LINE_STRIP);
			glVertex3f(MAX_X_POS, START_Y_POS,	   MAX_Z_POS);
			glVertex3f(MAX_X_POS, START_Y_POS,	  START_Z_POS );
		glEnd();
	glPopMatrix();
}

void CSence3::DrawCollData1()
{
	//draw flash plane.
	glPushMatrix();
		glTranslated(MAX_X_POS, START_Y_POS, START_Z_POS-m_fFlyStep);
		DrawPlane(90.0);
	glPopMatrix();

	//2.draw signal
	float xPos = MAX_X_POS;
	float yPos = START_Y_POS;
	float zPos = START_Z_POS-m_fFlyStep;

	Point3D ptstart(xPos,yPos, zPos);
	Point3D ptend(xPos,yPos-6.0, zPos);
	glPushMatrix();
		DrawSignal(ptstart, ptend); 
	glPopMatrix();

	glPushMatrix();
		drawCNString(xPos+2.0, yPos, zPos, " 获取采样数据");
	glPopMatrix();

	if (m_bDrawPane2)
	{
		DrawRoutePane1();
	}

	DrawRoute1Points();
}

void CSence3::DrawRoute1Points()
{
	//draw points
	int nCount = m_vecRoute1Points.size();
	if (nCount > 0)
	{
		glEnable ( GL_DEPTH_TEST );
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor3f(1.0, 0.0, 0.0);
		for (int nIndex = 0; nIndex<nCount; nIndex++)
		{
			float fZPos = m_vecRoute1Points[nIndex];

			glPointSize(4.0);
			glPushMatrix();
			glBegin(GL_POINTS);
			glVertex3f(MAX_X_POS, START_Y_POS, START_Z_POS-fZPos);
			glEnd();
			glPopMatrix();
		}
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
}

void CSence3::DrawRoute2()
{
	glPushMatrix();
	glRotated(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
		//draw plane static
		glPushMatrix();
			glTranslated (MAX_X_POS, START_Y_POS, MAX_Z_POS);
			DrawPlane(90.0*2);
		glPopMatrix();

		//draw route 2.
		glPushMatrix();
			if (m_bColorChange)
			{
				glColor3f(1.0, 0.0, 0.0);
			}
			else
			{
				glColor3f(0.0, 1.0, 0.0);
			}

			glLineWidth(2.0);
			glBegin(GL_LINE_STRIP);
			glVertex3f(MAX_X_POS,	START_Y_POS, MAX_Z_POS);
			glVertex3f(START_X_POS, START_Y_POS, MAX_Z_POS);
			glEnd();
		glPopMatrix();
	glPopMatrix();

	if (m_bDrawPane2)
	{
		glPushMatrix();
			glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
			DrawRoutePane1();
			DrawRoute1Points();
		glPopMatrix();
	}
}

void CSence3::DrawCollData2()
{
	glPushMatrix();
	glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);

		glPushMatrix();
			//+x------>-x
			glTranslated(MAX_X_POS-m_fFlyStep, START_Y_POS, MAX_Z_POS);
			DrawPlane(90.0*2);
		glPopMatrix();

		//2.draw signal
		float xPos = MAX_X_POS-m_fFlyStep;
		float yPos = START_Y_POS;
		float zPos = MAX_Z_POS;

		Point3D ptstart(xPos,yPos, zPos);
		Point3D ptend(xPos,yPos-6.0, zPos);
		glPushMatrix();
			DrawSignal(ptstart, ptend); 
		glPopMatrix();

		glPushMatrix();
			drawCNString(xPos, yPos+1.5, zPos, " 获取采样数据");
		glPopMatrix();

		if (m_bDrawPane3)
		{
			DrawRoutePane2();
		}
		DrawRoute2Points();
	glPopMatrix();
	
	if (m_bDrawPane2)
	{
		glPushMatrix();
			glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
			DrawRoutePane1();
			DrawRoute1Points();
		glPopMatrix();
	}
	
}

void CSence3::DrawRoute2Points()
{
	//draw route 2 points
	int nCount = m_vecRoute2Points.size();
	if (nCount > 0)
	{
		glEnable ( GL_DEPTH_TEST );
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor3f(0.0, 1.0, 0.0);
		for (int nIndex = 0; nIndex<nCount; nIndex++)
		{
			float fXPos = m_vecRoute2Points[nIndex];

			glPointSize(4.0);
			glPushMatrix();
				glBegin(GL_POINTS);
				glVertex3f(MAX_X_POS-fXPos, START_Y_POS, MAX_Z_POS);
				glEnd();
			glPopMatrix();
		}
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
}

void CSence3::DrawDyncMaxPane1()
{
	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);

		glPushMatrix();
			glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
			DrawPlane(90.0*2);
		glPopMatrix();
			if (m_bDrawPane3)
			{
				DrawRoutePane2();
				DrawRoute2Points();
			}
	glPopMatrix();

	if (m_bDrawPane2)
	{
		glPushMatrix();
			glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
			DrawRoutePane1();
			DrawRoute1Points();
		glPopMatrix();
	}


	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
		DrawDyncPane1();
		DrawMaxBlingPoint1();
	glPopMatrix();
}
//void CSence3::DrawCalcMaxPane1()
//{
//	glPushMatrix();
//	glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
//
//		glPushMatrix();
//			glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
//			DrawPlane(90.0*2);
//		glPopMatrix();
//		if (m_bDrawPane3)
//		{
//			DrawRoutePane2();
//			DrawRoute2Points();
//		}
//	glPopMatrix();
//
//	if (m_bDrawPane2)
//	{
//		glPushMatrix();
//			glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
//			DrawRoutePane1();
//			DrawRoute1Points();
//		glPopMatrix();
//	}
//	
//
//	glPushMatrix();
//		glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
//		DrawMaxPane1();
//		DrawMaxBlingPoint1();
//	glPopMatrix();
//	
//}

void CSence3::DrawDyncMaxPane2()
{
	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
		glPushMatrix();
			glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
			DrawPlane(90.0*2);
		glPopMatrix();

		if (m_bDrawPane3)
		{
			DrawRoutePane2();
			DrawRoute2Points();
		}
		DrawDyncPane2();
		DrawMaxBlingPoint2();
	glPopMatrix();

	if (m_bDrawPane2)
	{
		glPushMatrix();
			glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
			DrawRoutePane1();
			DrawRoute1Points();
		glPopMatrix();
	}

	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
		DrawMaxPane1();
		DrawMaxBlingPoint1();
	glPopMatrix();
}
//void CSence3::DrawCalcMaxPane2()
//{
//	
//	glPushMatrix();
//		glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
//		glPushMatrix();
//			glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
//			DrawPlane(90.0*2);
//		glPopMatrix();
//		
//		if (m_bDrawPane3)
//		{
//			DrawRoutePane2();
//			DrawRoute2Points();
//		}
//		DrawMaxPane2();
//		DrawMaxBlingPoint2();
//	glPopMatrix();
//
//	if (m_bDrawPane2)
//	{
//		glPushMatrix();
//		glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
//		DrawRoutePane1();
//		DrawRoute1Points();
//		glPopMatrix();
//	}
//	glPushMatrix();
//		glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
//		DrawMaxPane1();
//		DrawMaxBlingPoint1();
//	glPopMatrix();
//
//	
//}
void CSence3::DrawLastPosition()
{
	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
		glPushMatrix();
			glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
			DrawPlane(90.0*2);
		glPopMatrix();

		if (m_bDrawPane3)
		{
			DrawRoutePane2();
			DrawRoute2Points();
		}
		DrawMaxPane2();
		DrawBlingLine();
		DrawBlingBomb();
	glPopMatrix();


	if (m_bDrawPane2)
	{
		glPushMatrix();
			glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
			DrawRoutePane1();
			DrawRoute1Points();
		glPopMatrix();
	}
	

	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
		DrawMaxPane1();
	glPopMatrix();

	//绘制另外2个采样的脏弹位置.
	if (m_bDrawMorePos)
	{
		glPushMatrix();
			glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
			Draw2MoreBlingBombs();	
		glPopMatrix();
	}

}

void CSence3::DrawLastConfirmBomb()
{
	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
		glPushMatrix();
			glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
			DrawPlane(90.0*2);
		glPopMatrix();

		if (m_bDrawPane3)
		{
			DrawRoutePane2();
			DrawRoute2Points();
		}
		DrawMaxPane2();
		DrawBlingLine();
		DrawBlingBomb();
	glPopMatrix();


	if (m_bDrawPane2)
	{
		glPushMatrix();
			glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
			DrawRoutePane1();
			DrawRoute1Points();
		glPopMatrix();
	}


	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE1, 0.0,1.0, 0.0);
		DrawMaxPane1();
	glPopMatrix();

	//绘制另外2个采样的脏弹位置.
	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
		Draw2MoreBlingBombs();	
	glPopMatrix();


	DrawBombLinks();
}

void CSence3::DrawBombLinks()
{
	glPushMatrix();
		glRotatef(ROTATE_MAX_PANE2, 0.0,1.0,0.0);
		glLineWidth(2.0);
		glColor3f(1.0, 0.0, 0.0);
		if (m_uLinkCount == 1)
		{
			glBegin(GL_LINE_STRIP);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2+2.0, START_Y_POS-10.0, START_Z_POS+2.0);
			glEnd();
		}
		else if (m_uLinkCount == 2)
		{
			glBegin(GL_LINE_STRIP);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2+2.0, START_Y_POS-10.0, START_Z_POS+2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2/*+1.0*/, START_Y_POS-10.0, START_Z_POS+2.0-2.0);
			glEnd();
		}
		else if (m_uLinkCount == 3)
		{
			glBegin(GL_LINE_STRIP);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2+2.0, START_Y_POS-10.0, START_Z_POS+2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2/*+1.0*/, START_Y_POS-10.0, START_Z_POS+2.0-2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0);
			glEnd();
		}
		else if (m_uLinkCount == 4)
		{
			glBegin(GL_LINE_STRIP);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2+2.0, START_Y_POS-10.0, START_Z_POS+2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2/*+1.0*/, START_Y_POS-10.0, START_Z_POS+2.0-2.0);
				glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0);
			glEnd();

			//绘制最后确定的Bomb
			glPushMatrix();
				if (m_bColorChange)
				{
					glColor3f(1.0, 1.0, 0.0);
				}
				else
				{
					glColor3f( 0.0, 1.0, 0.0);
				}
				glTranslatef(START_X_POS+OFFSET_X_MAXPANE2+1.0, START_Y_POS-10.0, START_Z_POS+2.0-1.0/*-8.0*/);
				auxSolidSphere(0.3);
			glPopMatrix();
		}
	glPopMatrix();
}

void CSence3::DrawDyncPane1()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
	glColor4f(0.8, 0.0, 0.0, 0.4);
		glBegin(GL_POLYGON);
			glVertex3f(START_X_POS-9,	START_Y_POS-m_fOffsetPane1,	START_Z_POS-8.0);
			glVertex3f(MAX_X_POS+1.0,	START_Y_POS-m_fOffsetPane1, START_Z_POS-8.0);
			glVertex3f(MAX_X_POS+1.0,	START_Y_POS,				START_Z_POS-8.0);
			glVertex3f(START_X_POS-9,	START_Y_POS,				START_Z_POS-8.0);
		glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);

	DrawMaxBlingPoint1();
}
void CSence3::DrawMaxPane1()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
		glColor4f(0.8, 0.0, 0.0, 0.4);
		glBegin(GL_POLYGON);
		glVertex3f(START_X_POS-9, START_Y_POS-10.0, START_Z_POS-8.0);
		glVertex3f(MAX_X_POS+1.0,	START_Y_POS-10.0, START_Z_POS-8.0);
		glVertex3f(MAX_X_POS+1.0,	START_Y_POS/*-3.0*/,	 START_Z_POS-8.0);
		glVertex3f(START_X_POS-9, START_Y_POS/*-3.0*/,	 START_Z_POS-8.0);
		glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);

	DrawMaxBlingPoint1();
}

void CSence3::DrawMaxBlingPoint1()
{
	//draw points
	int nCount = m_vecRoute1Points.size();
	if (nCount > 0)
	{
		glEnable ( GL_DEPTH_TEST );
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (m_bColorChange)
		{
			glColor3f(1.0, 0.0, 1.0);
		}
		else
		{
			glColor3f(1.0, 1.0, 0.0);
		}

		float fZPos = m_vecRoute1Points[16];

		glPointSize(6.0);
		glPushMatrix();
		glBegin(GL_POINTS);
		glVertex3f(MAX_X_POS, START_Y_POS, START_Z_POS-fZPos);
		glEnd();
		glPopMatrix();
		
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
}

void CSence3::DrawMaxBlingPoint2()
{
	//draw points
	int nCount = m_vecRoute2Points.size();
	if (nCount > 0)
	{
		glEnable ( GL_DEPTH_TEST );
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (m_bColorChange)
		{
			glColor3f(1.0, 0.0, 1.0);
		}
		else
		{
			glColor3f(1.0, 1.0, 0.0);
		}

		float fZPos = m_vecRoute2Points[17];

		glPointSize(6.0);
		glPushMatrix();
		glBegin(GL_POINTS);
		glVertex3f(MAX_X_POS-fZPos, START_Y_POS, MAX_Z_POS);
		glEnd();
		glPopMatrix();

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
}

void CSence3::DrawDyncPane2()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
	glColor4f(0.0, 0.9, 0.0, 0.6);
		glBegin(GL_POLYGON);
			glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-m_fOffsetPane2,	START_Z_POS+9.0);
			glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-m_fOffsetPane2,	MAX_Z_POS-6.0);
			glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS,					MAX_Z_POS-6.0);
			glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS,					START_Z_POS+9.0);
		glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);

	DrawMaxBlingPoint2();
}
void CSence3::DrawMaxPane2()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();
		glColor4f(0.0, 0.9, 0.0, 0.6);
		glBegin(GL_POLYGON);
		glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+9.0/*-6.0*/);
		glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, MAX_Z_POS-6.0);
		glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS/*+3.0*/,	   MAX_Z_POS-6.0);
		glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS/*+3.0*/,	   START_Z_POS+9.0/*-6.0*/);
		glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);

	DrawMaxBlingPoint2();
}
void CSence3::DrawBlingLine()
{
	glPushMatrix();
		if (m_bColorChange)
		{
			glColor3f(0.0, 1.0, 0.0);
		}
		else
		{
			glColor3f(0.0, 0.0, 1.0);
		}
		
		//glLineWidth(3.0);
		glBegin(GL_LINE_STRIP);
			glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS/*-3.0*/, START_Z_POS+2.0/*-8.0*/);
			glVertex3f(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0/*-8.0*/);
		glEnd();
	glPopMatrix();
}

void CSence3::DrawBlingBomb()
{
	glPushMatrix();
		if (m_bColorChange)
		{
			glColor3f(0.0, 1.0, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		glTranslatef(START_X_POS+OFFSET_X_MAXPANE2, START_Y_POS-10.0, START_Z_POS+2.0/*-8.0*/);
		auxSolidSphere(0.2);
	glPopMatrix();
}

void CSence3::Draw2MoreBlingBombs()
{
	glPushMatrix();
		if (m_bColorChange)
		{
			glColor3f(0.0, 1.0, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		glTranslatef(START_X_POS+OFFSET_X_MAXPANE2+2.0, START_Y_POS-10.0, START_Z_POS+2.0);
		auxSolidSphere(0.2);
	glPopMatrix();

	glPushMatrix();
		if (m_bColorChange)
		{
			glColor3f(0.0, 1.0, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		glTranslatef(START_X_POS+OFFSET_X_MAXPANE2/*+1.0*/, START_Y_POS-10.0, START_Z_POS+2.0-2.0);
		auxSolidSphere(0.2);
	glPopMatrix();

}

void CSence3::DrawSignal( Point3D start, Point3D end) 
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

void CSence3::drawCNString(float x, float y, float z, const char* str)
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

void CSence3::selectFont(int size, int charset, const char* face) 
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_BOLD, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(/*wglGetCurrentDC()*/m_hDC, hFont);
	DeleteObject(hOldFont);
}
void CSence3::OnKeyDown(WPARAM nChar)
{
	switch(nChar)
	{
	case VK_UP:
		{
			XP_3 -= (float)sin(heading_3*piover180) * 0.1f;	
			ZP_3 -= (float)cos(heading_3*piover180) * 0.1f;
		}
		break;
	case VK_DOWN:
		{
			XP_3 += (float)sin(heading_3*piover180) * 0.1f;	
			ZP_3 += (float)cos(heading_3*piover180) * 0.1f;
		}
		break;
	case VK_LEFT:
		{
			zprot_3 += 0.045f;
		}
		break;
	case VK_RIGHT:
		{
			zprot_3 -= 0.045f;
		}
		break;
	case VK_NEXT:
		{
			yptrans23+=0.05f;
			if (yptrans23 > 0) yptrans23*=0.9f;	
		}
		break;
	case VK_PRIOR:
		{
			yptrans23-=0.05f;
			if (yptrans23 < -8) yptrans23*=0.99f;
		}
		break;
	case 'M':
		{
	/*		if (!mp)
			{
				mp=TRUE;
				multitexture_3=!multitexture_3; 
			}*/
		}
		break;
	case 'L':
		{
			if (!lp_3)
			{
				lp_3=TRUE;
				water_3=!water_3;
			}
		}
		break;
	case 'W':
		{
			if (!wp_3)
			{
				wp_3=TRUE;
				//wireframe=!wireframe;
			}
		}
		break;
	default:
		break;
	}

	XP_3 *= 0.995f; 
	ZP_3 *= 0.995f; 
	XPOS_3 += XP_3/30;//(float)sin(heading_3*piover180) * 0.25f;
	ZPOS_3 += ZP_3/30;
}
void CSence3::OnKeyUp(WPARAM nChar)
{
	switch(nChar)
	{
	case 'L':
		{
			lp_3 = FALSE;
		}
		break;
	case 'W':
		{
			wp_3 = FALSE;
		}
		break;
	default:
		break;
	}

}

void CSence3::Resize(int cx, int cy)
{
	glViewport(0,0,cx,cy);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)cx/(GLfloat)cy, 0.1f,50000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	/////////////////////////////////////////////////////////////////
}
void CSence3::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TIMER_DRAW_EXP)
	{
		if (m_bStartExplosion)
		{
			int i;
			//更新爆炸纹理
			m_fSize += .005f;//更新爆炸范围
			m_fDelta += .04f;
			m_fRotate += .4f;
			m_fOpacity -= .006f;//m_fOpacity为0时，爆炸云就消失了

			for (i = 0; i < 30; i++) 
			{
				corners[i][0] += 0.7 * traj[i / 3][0];
				corners[i][1] += 0.7 * traj[i / 3][1];
				corners[i][2] += 0.7 * traj[i / 3][2];
			}

			for (i = 0; i < 10; i++)
				traj[i][1] -= 0.025f;
			//time++;	
			m_nExplorCount++;
		}
	}
	else if(nIDEvent == TIMER_DRAW_PLANE)
	{
		switch(m_eState)
		{
		case eState_Free:
			{
				if (m_eDir == 0)
				{
					//-x---->+x
					if (m_fFlyStep + START_X_POS >= MAX_X_POS)
					{
						m_eDir = 1;
						m_fFlyStep = 0.0;
					}
				}
				else if (m_eDir == 1)
				{
					//+z---->-z
					if (START_Z_POS +(-m_fFlyStep) <= MAX_Z_POS)
					{
						m_eDir = 2;
						m_fFlyStep = 0.0f;
					}
				}
				else if (m_eDir == 2)
				{
					//+x---->-x
					if (MAX_X_POS-m_fFlyStep <= START_X_POS)
					{
						m_eDir = 3;
						m_fFlyStep = 0.0f;
					}
				}
				else if (m_eDir == 3)
				{
					//-z---->+z
					if (MAX_Z_POS+m_fFlyStep >= START_Z_POS)
					{
						m_eDir = 0;
						m_fFlyStep = 0.0f;

					}
				}	

				m_fFlyStep += 0.5f;
			}
			break;
		case eState_Route_1: 
			{
				//更改线条颜色。
				m_bColorChange = !m_bColorChange;	
			}
			break;
		case eState_Coll_Data_1:
			{
				//-z--->+z
				if (START_Z_POS +(-m_fFlyStep) <= MAX_Z_POS)
				{
					//绘制平面
					m_bDrawPane2 = TRUE;

					//走到底了，再从头走一遍
					m_fFlyStep    = 0.0f;
					m_fSignalStep = 0.0f;
					m_vecRoute1Points.clear();
				}

				m_vecRoute1Points.push_back(m_fFlyStep);
				m_fFlyStep += 0.5f;

				SendDataToChart1(m_fFlyStep);
			}
			break;
		case eState_Route_2:
			{
				//更改线条颜色。
				m_bColorChange = !m_bColorChange;	
			}
			break;
		case eState_Coll_Data_2:
			{
				//+x---->-x
				if (MAX_X_POS-m_fFlyStep <= START_X_POS)
				{
					//绘制平面
					m_bDrawPane3 = TRUE;

					//走到底了，再从头走一遍
					m_fFlyStep    = 0.0f;
					m_fSignalStep = 0.0f;
					m_vecRoute2Points.clear();
				}

				m_vecRoute2Points.push_back(m_fFlyStep);
				m_fFlyStep += 0.5f;
				
				//同步数据到Chartctrl
				SendDataToChart2(m_fFlyStep);
			}
			break;
		//case eState_DrawMaxPane_1:
		//	{
		//		//更改线条颜色。
		//		m_bColorChange = !m_bColorChange;
		//	}
		//	break;
		//case eState_DrawMaxPane_2:
		//	{
		//		m_bColorChange = !m_bColorChange;
		//	}
		//	break;
		case eState_DrawBomb:
			{
				m_bColorChange = !m_bColorChange;
			}
			break;
		case eState_DrawConfirmBomb:
			{
				m_bColorChange = !m_bColorChange;
			}
			break;
		default:
			break;
		}
	}
	//route 1的闪烁绘制该结束了。进入1的采集动画阶段
	else if (nIDEvent == TIMER_ROUTE_1_OVER)
	{
		m_eState = eState_Coll_Data_1;
		
		KillTimer(m_hWnd,TIMER_ROUTE_1_OVER);
		SetTimer(m_hWnd, TIMER_DRAW_SIGNAL, 30, NULL);//整个飞行阶段一直绘制信号线。以示采集。	
	}
	else if (nIDEvent == TIMER_ROUTE_2_OVER)
	{
		m_eState = eState_Coll_Data_2;

		KillTimer(m_hWnd, TIMER_ROUTE_2_OVER);
		SetTimer(m_hWnd, TIMER_DRAW_SIGNAL, 30, NULL);
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
// 	else if (nIDEvent == TIMER_MAX_PLANE2)
// 	{		
// 		KillTimer(m_hWnd, TIMER_MAX_PLANE2);
// 		SetTimer(m_hWnd, TIMER_DIRTY_BOMB, 4000, NULL);
// 		m_eState = eState_DrawMaxPane_2;
// 	}
	else if (nIDEvent == TIMER_DIRTY_BOMB)
	{
		KillTimer(m_hWnd, TIMER_DIRTY_BOMB);
		m_eState = eState_DrawBomb;
	}
	else if (nIDEvent == TIMER_DYNC_PANE1)
	{
		if (m_fOffsetPane1> 10.0)
		{
			KillTimer(m_hWnd, TIMER_DYNC_PANE1);
			m_fOffsetPane1 = 0.0;

			m_eState = eState_Draw_Dync_Pane2;
			SetTimer(m_hWnd, TIMER_DYNC_PANE2, 200, NULL);
		}
		m_fOffsetPane1 += 0.5;

		//更改线条颜色。
		m_bColorChange = !m_bColorChange;
	}
	else if (nIDEvent == TIMER_DYNC_PANE2)
	{
		if (m_fOffsetPane2 > 10.0)
		{
			KillTimer(m_hWnd, TIMER_DYNC_PANE2);
			m_fOffsetPane2 = 0.0;

			m_eState = eState_DrawBomb;
		}
		m_fOffsetPane2 += 0.5;

		m_bColorChange = !m_bColorChange;
	}
	else if (nIDEvent == TIMER_LINK_BOMBS)
	{
		m_uLinkCount++;
		if (m_uLinkCount == 4)
		{
			KillTimer(m_hWnd, TIMER_LINK_BOMBS);
		}
	}

}
void CSence3::ResetExp()
{
	KillTimer(m_hWnd, TIMER_EXP_OVER);

	m_bExplover = TRUE;
	m_bStartExplosion = FALSE;

	m_nExplorCount = 0;
	m_fRotate = 0.f;
	m_fOpacity = 1.0f;
	m_fIntensity = 1.0f;
	m_fSize = .2f;
	m_fDelta = 0.f;
	m_fScale = 1.;
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 3; j++)
			corners[i][j] = 0;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 3; j++)
			traj[i][j] = 0.0f;
}

void CSence3::SetStartExp(BOOL bStart)
{
	m_bStartExplosion = bStart;
}
void CSence3::SetStartPlane(BOOL bStart)
{
	m_bStartCollData = bStart;
}
BOOL CSence3::LoadPlaneModel()
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

void CSence3::CleanUp()
{
	if (NULL != m_hPalette)
	{
		DeleteObject(m_hPalette);
	}
	for (int i=0; i<5; i++)
	{
		if (m_arrTexture[i] > 0)
		{
			glDeleteTextures(1, &m_arrTexture[i]);
		}
	}
	for (int i=0; i<2; i++)
	{
		if (texnames[i] > 0)
		{
			glDeleteTextures(1, &texnames[i]);
		}
	}

	KillTimer(m_hWnd, TIMER_DRAW_EXP);
	KillTimer(m_hWnd, TIMER_DRAW_PLANE);
	KillTimer(m_hWnd, TIMER_EXP_OVER);

	ReleaseRes();
}

BOOL CSence3::ReleaseRes()
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

void CSence3::StartFlyTask1()
{
	m_eState = eState_Route_1;
	SetTimer(m_hWnd, TIMER_ROUTE_1_OVER, 3000, NULL);

	//reset 
	KillTimer(m_hWnd, TIMER_DRAW_SIGNAL);
	m_fFlyStep = 0.0f;
	m_fSignalStep = 0.0f;
}
void CSence3::StartFlyTask2()
{
	m_eState = eState_Route_2;
	SetTimer(m_hWnd, TIMER_ROUTE_2_OVER, 3000, NULL);

	//reset.
	KillTimer(m_hWnd, TIMER_DRAW_SIGNAL);
	m_fFlyStep = 0.0f;
	m_fSignalStep = 0.0f;

	//Init route 1 points array.hard code.
	m_vecRoute1Points.clear();
	for(float f = 0.0; f<=16.0;)
	{
		m_vecRoute1Points.push_back(f);
		f += 0.5;
	}
}

void CSence3::CalcNuclearPos()
{
	//绘制动态的Pane1
	m_eState = eState_Draw_Dync_Pane1;
	SetTimer(m_hWnd, TIMER_DYNC_PANE1, 200, NULL);
	
	//绘制第一个max pane
	//m_eState = eState_DrawMaxPane_1;
	//3s后绘制第二个max pane
	//SetTimer(m_hWnd, TIMER_MAX_PLANE2, 3000,NULL);

	//Init route 2 points array.hard code.
	m_vecRoute2Points.clear();
	for (float f=0.0; f<=16.0;)
	{
		m_vecRoute2Points.push_back(f);
		f += 0.5;
	}
}

void CSence3::Draw2MorePos()
{
	m_bDrawMorePos = TRUE;
}

void CSence3::DrawConfirmedPos()
{
	m_eState = eState_DrawConfirmBomb;
	SetTimer(m_hWnd, TIMER_LINK_BOMBS, 2000, NULL);
}

void CSence3::SendDataToChart1(float fCurPos, float fDensity /*= 0*/)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		pFrame->SendDataChart1(fCurPos, fDensity);
	}
}
void CSence3::SendDataToChart2(float fCurPos, float fDensity /*= 0*/)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		pFrame->SendDataChart2(fCurPos, fDensity);
	}
}


//注意：此函数需要在GL-depth_test Enable状态下才会显示信号线正常。
//否则信号线看起来总是在2D平面绘制的效果。
//void CSence3::DrawStaticPlaneAndSignal()
//{
//	float xPos = 0.0f;
//	float zPos = 0.0f;
//	float yPos = START_Y_POS;
//
//	//1.draw static plane.
//	glPushMatrix();
//	if (m_eDir == 0)
//	{
//		xPos = START_X_POS;
//		zPos = START_Z_POS;
//		glTranslated (START_X_POS, START_Y_POS, START_Z_POS);
//
//	}
//	else if (m_eDir == 1)
//	{
//		xPos = MAX_X_POS;
//		zPos = START_Z_POS;
//		glTranslated(MAX_X_POS, START_Y_POS, START_Z_POS);
//	}
//	else if (m_eDir == 2)
//	{
//		xPos = MAX_X_POS;
//		zPos = MAX_Z_POS;
//		glTranslated(MAX_X_POS, START_Y_POS, MAX_Z_POS);
//	}
//	else if (m_eDir == 3)
//	{
//		xPos = START_X_POS;
//		zPos = MAX_Z_POS;
//		glTranslated(START_X_POS, START_Y_POS, MAX_Z_POS);
//	}
//
//	glEnable( GL_COLOR_MATERIAL); 
//	glShadeModel(GL_SMOOTH);
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
//	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
//
//	glRotatef(90*m_eDir, 0,1,0);
//	glRotated(90.0,0.0,0.0,1.0);//2.绕飞机的箭头原点整个机身逆时针转90度。
//	glRotated(90.0,0.0,1.0,0.0);//1.先绕Y转个90度得到侧面。
//	glScalef(0.0002,0.0002,0.0002);
//	if (m_bPlaneLoaded)
//	{
//		m_planeModel.drawGL();
//	}
//	glDisable(GL_LIGHTING);
//
//	glPopMatrix();
//
//	//2.draw signal
//	Point3D ptstart(xPos,yPos, zPos);
//	Point3D ptend(xPos,yPos-6.0, zPos);
//	glPushMatrix();
//	DrawSignal(ptstart, ptend); 
//	glPopMatrix();
//
//	glPushMatrix();
//	drawCNString(xPos, yPos, zPos, " 获取采样数据");
//	glPopMatrix();
//}