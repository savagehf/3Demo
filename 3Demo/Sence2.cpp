#include "stdafx.h"
#include "MainFrm.h"
#include "Sence2.h"
#include "vectorlib.h"

#include "3DSREADER.H"
#include "CommonDef.h"
#include "MSModel.h"
#include <time.h>
#include "ComDefinition.h"

#define START_X_POS			-8
#define MAX_X_POS			8
#define START_Z_POS			-20
#define MAX_Z_POS			-36
#define START_Y_POS			6.0
#define SCALE_FACTOR		0.0002/*0.05*/

CSence2::CSence2(HWND hWnd)
{
	m_hrc = NULL;
	m_hDC = NULL;
	m_hWnd = hWnd;

	m_nCactus = 0;
	m_nSand  = 0;

	//explosion
	m_fRotate = 0.f;
	m_fOpacity = 1.0f;
	m_fIntensity = 1.0f;
	m_fSize = .2f;
	m_fDelta = 0.f;
	m_fScale = 1.;

	m_bPlaneLoaded = FALSE;
	m_fSignalStep = 0.0;
	m_eDir = 0;
	m_bDrawPane1 = FALSE;
	m_bDrawPane2 = FALSE;
	m_bDrawPane3 = FALSE;
	m_bDrawPane4 = FALSE;

	m_bStartCollData  = FALSE;
	m_bStartExplosion = FALSE;
	m_bExplover = FALSE;
	m_nExplorCount = 0;

}

CSence2::~CSence2()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////
//#define KEY_DOWN(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
//#define KEY_UP(vk_code)    ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define RAND_COORD(x)   ((float)rand()/RAND_MAX * (x))
#define KEY_DOWN(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

/*#include "funcdef.h"*/
typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);
// compiled array extensions

extern PFNGLLOCKARRAYSEXTPROC  glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;


#define BITMAP_ID   0x4D42    // the universal bitmap ID


/*****************************************************************************
Performs one-time application-specific setup. Returns FALSE on any failure.
*****************************************************************************/
BOOL CSence2::Init()
{
	InitOpenGL();

	InitLight();

	//load plane.
	LoadPlaneModel();
	
	InitExplosion();

	// load res.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CCW);

	LoadTexture("Texture\\cactus.bmp", m_nCactus);
	LoadTexture("Texture\\sand.bmp", m_nSand);

	// make the background look like the sky
	float blue[4] = { 0.5, 0.5, 1.0, 0.0 };
	glClearColor(0.5, 0.5, 1.0, 0.0);

	// use a distant fog for a haze effect
// 	glEnable(GL_FOG);
// 	glFogfv(GL_FOG_COLOR, blue);
// 	glFogf(GL_FOG_MODE, GL_EXP2);
// 	glFogf(GL_FOG_START, 200);
// 	glFogf(GL_FOG_END, 1000);
// 	glFogf(GL_FOG_DENSITY, 0.002f);

	// load terrain arrays
	InitializeTerrain();

	// check for the compiled array extensions
	char *extList = (char *) glGetString(GL_EXTENSIONS);

	if (extList && strstr(extList, "GL_EXT_compiled_vertex_array"))
	{
		// get the address of the compiled array extensions
		glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC) wglGetProcAddress("glLockArraysEXT");
		glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) wglGetProcAddress("glUnlockArraysEXT");
	}

	return TRUE;
} 

void CSence2::InitLight()
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


BOOL CSence2::LoadPlaneModel()
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
BOOL CSence2::InitOpenGL()
{
	if (!(m_hDC = GetDC(m_hWnd)))
	{
		AfxMessageBox("Unable to create device context");
		return FALSE;
	}

	// set the pixel format we want
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // size of structure
		1,                              // default version
		PFD_DRAW_TO_WINDOW |            // window drawing support
		PFD_SUPPORT_OPENGL |            // OpenGL support
		PFD_DOUBLEBUFFER,               // double buffering support
		PFD_TYPE_RGBA,                  // RGBA color mode
		32,                           // 32 bit color mode
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

		GLuint  pixelFormat;

		// choose best matching pixel format
		if (!(pixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
		{
			//MessageBox(NULL, "Can't find an appropriate pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		// set pixel format to device context
		if(!SetPixelFormat(m_hDC, pixelFormat,&pfd))
		{
			//MessageBox(NULL, "Unable to set pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		// create the OpenGL rendering context
		if (!(m_hrc = wglCreateContext(m_hDC)))
		{
			//MessageBox(NULL, "Unable to create OpenGL rendering context", "Error",MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		// now make the rendering context the active one
		if(!wglMakeCurrent(m_hDC, m_hrc))
		{
			//MessageBox(NULL,"Unable to activate OpenGL rendering context", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
}

/*****************************************************************************
 The work of the application is done here. This is called every frame, and
 handles the actual rendering of the scene.
*****************************************************************************/
BOOL CSence2::Draw()
{
  // used to track the orientation of the viewer
  static GLfloat s_eye[] = { MAP_X * MAP_SCALE * 0.5, 8.0, -MAP_Z * MAP_SCALE * 0.5};
  static GLfloat s_at[]  = { 0.0, 0.0, 0.0 };
  static GLfloat s_angle = -90.0;
  float speed = 0.3f;

  // check for rotation
  if (m_keys[VK_LEFT])
  {
    s_angle -= 2.0;
  }
  if (m_keys[VK_RIGHT])
  {
    s_angle += 2.0;
  }

  // run if the shift key is pressed
  if (KEY_DOWN(VK_SHIFT))
    speed = speed * 2;

  float rad =  float(PI*s_angle/180.0f);

  // check for forward and backward motion
  if (m_keys[VK_UP])
  {
    s_eye[2] += (float)sin(rad) * speed;
    s_eye[0] += (float)cos(rad) * speed;
  }
  if (m_keys[VK_DOWN])
  {
    s_eye[2] -= (float)sin(rad) * speed;
    s_eye[0] -= (float)cos(rad) * speed;
  }

  // do bound's checking to make sure they don't leave the map
  if (s_eye[0] < MAP_SCALE)
    s_eye[0] = MAP_SCALE;
  if (s_eye[0] > (MAP_X - 2) * MAP_SCALE)
    s_eye[0] = (MAP_X - 2) * MAP_SCALE;
  if (s_eye[2] < -(MAP_Z - 2) * MAP_SCALE)
    s_eye[2] = -(MAP_Z - 2) * MAP_SCALE;
  if (s_eye[2] > - MAP_SCALE)
    s_eye[2] = -MAP_SCALE;

  // set the eye position in relation to the ground
  s_eye[1] = GetHeight(s_eye[0], s_eye[2]) + 2.0f;

  //set the look at point to be at eye level in the direction the viewer is headed
  s_at[0] = float(s_eye[0] + 100*cos(rad));
  s_at[2] = float(s_eye[2] + 100*sin(rad));
  s_at[1] = s_eye[1];

  float x1 = s_eye[0];
  float y1 = s_eye[1]+20;
  float z1 = s_eye[2];
  float s1 = s_at[0];
  float s2 = s_at[1];
  float s3 = s_at[2];

  // set up the modelview matrix according to this viewer orientation
//    glLoadIdentity();
//    gluLookAt(s_eye[0], s_eye[1]+20, s_eye[2],
//      s_at[0], s_at[1], s_at[2],
//      0.0, 1.0, 0.0
//      );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
// 		if (m_bExplover)
// 		{
// 			glEnable(GL_TEXTURE_2D);
// 		}
		glTranslatef(-400, -29, 400);

		DrawSand();
		DrawCacti();
	glPopMatrix();

		//draw plane
	//if (m_bStartCollData)
	{
		glPushMatrix();
		//glTranslatef(-400, -30, 400);
 		if (m_bIsGettingData)
 		{
 			DrawStaticPlaneAndSignal();
 		}
 		else
		{
			DrawPlane();
		}
		

		//if (m_bDrawPane1)
		{
			DrawClippane1();
		}
		//if (m_bDrawPane2)
		{
			DrawClippane2();
		}
		//if (m_bDrawPane3)
		{
			DrawClippane3();
		}
		//if (m_bDrawPane4)
		{
			DrawClippane4();
		}
		glPopMatrix();
	}

	////draw explosion
	//if (m_bStartExplosion)
	//{
	//	glPushMatrix();
	//	glTranslatef(400.0f, 20.0f, -410.0f);
	//	DrawExplosion();
	//	glPopMatrix();
	//}

	SwapBuffers(m_hDC);

	return TRUE;
} // end Draw()


/*****************************************************************************
LoadBitmapFile()

Returns a pointer to the bitmap image of the bitmap specified by filename.
Also returns the bitmap header information. No support for 8-bit bitmaps.
*****************************************************************************/
unsigned char *  CSence2::LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;                        // the file pointer
	BITMAPFILEHEADER  bitmapFileHeader;   // bitmap file header
	unsigned char    *bitmapImage;        // bitmap image data
	unsigned int      imageIdx = 0;       // image index counter
	unsigned char     tempRGB;            // swap variable

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// swap the R and B values to get RGB since the bitmap color format is in BGR
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	return bitmapImage;
} // end LoadBitmapFile()


/*****************************************************************************
LoadBitmapFileWithAlpha

Loads a bitmap file normally, and then adds an alpha component to use for
blending
*****************************************************************************/
unsigned char* CSence2::LoadBitmapFileWithAlpha(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	unsigned char *bitmapImage = LoadBitmapFile(filename, bitmapInfoHeader);
	unsigned char *bitmapWithAlpha = (unsigned char *)malloc(bitmapInfoHeader->biSizeImage * 4 / 3);

	if (bitmapImage == NULL || bitmapWithAlpha == NULL)
		return NULL;

	// loop through the bitmap data
	for (unsigned int src = 0, dst = 0; src < bitmapInfoHeader->biSizeImage; src +=3, dst +=4)
	{
		// if the pixel is black, set the alpha to 0. Otherwise, set it to 255.
		if (bitmapImage[src] == 0 && bitmapImage[src+1] == 0 && bitmapImage[src+2] == 0)
			bitmapWithAlpha[dst+3] = 0;
		else
			bitmapWithAlpha[dst+3] = 0xFF;

		// copy pixel data over
		bitmapWithAlpha[dst] = bitmapImage[src];
		bitmapWithAlpha[dst+1] = bitmapImage[src+1];
		bitmapWithAlpha[dst+2] = bitmapImage[src+2];
	}

	free(bitmapImage);

	return bitmapWithAlpha;
} // end LoadBitmapFileWithAlpha()


/****************************************************************************
LoadTexture()

Loads the texture from the specified file and stores it in iTexture.
*****************************************************************************/
void CSence2::LoadTexture(char *filename, GLuint &texture)
{
	// get a texture object
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// load the bitmap
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char *buffer = LoadBitmapFileWithAlpha(filename, &bitmapInfoHeader);

	// set up the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	// we're done with the bitmap data
	free(buffer);
} // end LoadTexture()

/*****************************************************************************
InitializeTerrain()

Create vertex arrays containing terrain data
*****************************************************************************/
void CSence2::InitializeTerrain()
{
	// used to track current entry in the index array
	int index = 0;
	int currentVertex;

	// loop through all of the heightfield points, randomly generating height values
	for (int z = 0; z < MAP_Z; z++)
	{
		for (int x = 0; x < MAP_X; x++)
		{
			// set the world coordinates of this point
			m_nArrTerrain[x + z * MAP_X][0] = float(x)*MAP_SCALE;
			m_nArrTerrain[x + z * MAP_X][1] = 5.0f + FRAND * 5.0f;
			m_nArrTerrain[x + z * MAP_X][2] = -float(z)*MAP_SCALE;

			// vertices are numbered left to right, top to bottom
			currentVertex = z * MAP_X + x;

			// set the values in the color array
			m_nArrColor[currentVertex][0] = m_nArrColor[currentVertex][1] =
				m_nArrColor[currentVertex][2] = m_nArrTerrain[x + z * MAP_X][1] / 20.0f + 0.5f;

			// set the values in the texture coordinate array. since the texture
			// is tiled over each "square", we can use texture wrapping
			m_nArrTexcoord[currentVertex][0] = (float) x;
			m_nArrTexcoord[currentVertex][1] = (float) z;

		}
	}

	// loop over all vertices in the terrain map
	for (int z = 0; z < MAP_Z-1; z++)
	{
		for (int x = 0; x < MAP_X; x++)
		{
			// add next two elements to the triangle strip
			currentVertex = z * MAP_X + x;
			m_nArrIndex[index++] = currentVertex + MAP_X;
			m_nArrIndex[index++] = currentVertex;
		}
	}

	// enable the vertex arrays being used
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// pass the pointers to OpenGL
	glVertexPointer(3, GL_FLOAT, 0, m_nArrTerrain);
	glColorPointer(3, GL_FLOAT, 0, m_nArrColor);
	glTexCoordPointer(2, GL_FLOAT, 0, m_nArrTexcoord);
} // end InitializeArrays()




/*****************************************************************************
GetHeight()

Returns the height of the terrain at (x, z)
*****************************************************************************/
float CSence2::GetHeight(float x, float z)
{
	// divide by the grid-spacing if it is not 1
	float projCameraX = x / MAP_SCALE;
	float projCameraZ = -z / MAP_SCALE;

	// compute the height field coordinates (hflCol0, hflRow0) and
	// (hflCol1, hflRow1) that identify the height field cell directly below the camera.
	int hflCol0 = int(projCameraX);
	int hflRow0 = int(projCameraZ);
	int hflCol1 = hflCol0 + 1;
	int hflRow1 = hflRow0 + 1;

	// get the four corner heights of the cell from the height field
	float h00 = m_nArrTerrain[hflCol0 + hflRow0*MAP_X][1];
	float h01 = m_nArrTerrain[hflCol1 + hflRow0*MAP_X][1];
	float h11 = m_nArrTerrain[hflCol1 + hflRow1*MAP_X][1];
	float h10 = m_nArrTerrain[hflCol0 + hflRow1*MAP_X][1];

	// calculate the position of the camera relative to the cell.
	// note, that 0 <= tx, ty <= 1.
	float tx = projCameraX - float(hflCol0);
	float ty = projCameraZ - float(hflRow0);

	// the next step is to perform a bilinear interpolation to compute the height
	// of the terrain directly below the object.
	float txty = tx * ty;

	return h00 * (1.0f - ty - tx + txty)
		+ h01 * (tx - txty)
		+ h11 * txty
		+ h10 * (ty - txty);
} // end GetHeight()


/*****************************************************************************
DrawCacti()

Draw cacti as billboarded quads.
*****************************************************************************/
void CSence2::DrawCacti()
{
	// make sure the random numbers we generate are the same every time
	srand(100);

	// make sure the transparent part of the texture isn't drawn
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	// get the modelview matrix
	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);

	// get the right and up vectors
	vector3_t right(mat[0], mat[4], mat[8]);
	vector3_t up(mat[1], mat[5], mat[9]);

	// select the cactus texture
	glBindTexture(GL_TEXTURE_2D, m_nCactus);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// draw all cacti
	glBegin(GL_QUADS);
	for (int n = 0; n < NUM_CACTI; n++)
	{
		// randomly size the cactus
		float size = 5.0f + FRAND + 3.0f;

		// pick a random position on the map 
		vector3_t pos(RAND_COORD((MAP_X - 1) * MAP_SCALE), 0.0, -RAND_COORD((MAP_Z - 1) * MAP_SCALE));
		pos.y = GetHeight(pos.x, pos.z) + size - 0.5f;

		// bottom left corner
		glTexCoord2f(0.0, 0.0); glVertex3fv((pos + (right + up) * -size).v);
		// bottom right corner
		glTexCoord2f(1.0, 0.0); glVertex3fv((pos + (right - up) * size).v);
		// top right corner
		glTexCoord2f(1.0, 1.0); glVertex3fv((pos + (right + up) * size).v);
		// top left corner
		glTexCoord2f(0.0, 1.0); glVertex3fv((pos + (up - right) * size).v);
	}
	glEnd();
	glDisable(GL_ALPHA);
	glDisable(GL_BLEND);
} // end DrawCacti()


/****************************************************************************
DrawSand()

Draw the terrain
*****************************************************************************/
void CSence2::DrawSand()
{
	// select the sand texture
	glBindTexture(GL_TEXTURE_2D, m_nSand);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if (glLockArraysEXT)
		glLockArraysEXT(0, MAP_X * MAP_Z * 6);

	// loop through all the triangle strips
	for (int z = 0; z < MAP_Z-1; z++)
	{
		// draw the triangles in this strip
		glDrawElements(GL_TRIANGLE_STRIP, MAP_X * 2, GL_UNSIGNED_INT, &m_nArrIndex[z * MAP_X * 2]);
	}

	// if the compiled arrays extension is available, unlock the arrays
	if (glUnlockArraysEXT)
		glUnlockArraysEXT();
} // end DrawSand()

/*****************************************************************************
 Cleanup()

 Called at the end of successful program execution.
*****************************************************************************/
BOOL CSence2::Cleanup()
{
	if (m_nSand > 0 )
	glDeleteTextures(1, &m_nSand);
	if (m_nCactus > 0)
	glDeleteTextures(1, &m_nCactus);

	for (int i=0; i<2; i++)
	{
		if (texnames[i] > 0)
		{
			glDeleteTextures(1, &texnames[i]);
		}
	}


	ReleaseRes();

	return TRUE;
} // end Cleanup()

BOOL CSence2::ReleaseRes()
{
	// if we have an RC, release it
	if (m_hrc)
	{
		// release the RC
		if (!wglMakeCurrent(NULL,NULL))
		{
			// MessageBox(NULL, "Unable to release rendering context", "Error", MB_OK | MB_ICONINFORMATION);
		}

		// delete the RC
		if (!wglDeleteContext(m_hrc))
		{
			//MessageBox(NULL, "Unable to delete rendering context", "Error", MB_OK | MB_ICONINFORMATION);
		}

		m_hrc = NULL;
	}

	// release the DC if we have one
	if (m_hDC != NULL && !ReleaseDC(m_hWnd, m_hDC))
	{
		//MessageBox(NULL, "Unable to release device context", "Error", MB_OK | MB_ICONINFORMATION);
		m_hDC = NULL;
	}

	//// destroy the window if we have a valid handle
	//if (g_hwnd && !DestroyWindow(g_hwnd))
	//{
	// MessageBox(NULL, "Unable to destroy window", "Error", MB_OK | MB_ICONINFORMATION);
	// g_hwnd = NULL;
	//}

	//// unregister our class so we can create a new one if we need to
	//if (!UnregisterClass(WND_CLASS_NAME, g_hInstance))
	//{
	// MessageBox(NULL, "Unable to unregister window class", "Error", MB_OK | MB_ICONINFORMATION);
	// g_hInstance = NULL;
	//}

	return TRUE;
}

/*****************************************************************************
 Called once when the application starts and again every time the window is
 resized by the user.
*****************************************************************************/
void CSence2::Resize(int width, int height)
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
  gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 50000.0f);

  // select modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
} 

void CSence2::OnKeydown(WPARAM wPara)
{
	m_keys[wPara] = TRUE;
}

void CSence2::OnKeyUp(WPARAM wPara)
{
	m_keys[wPara] = FALSE;
}
void CSence2::OnTimer(UINT nIDEvent)
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

void CSence2::ResetExp()
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
void CSence2::InitExplosion(GLvoid)
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

	/*static unsigned *explosionimage,*smokeimage;
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

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50., 1., .1, 20.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0., 0., -5.5);
	glClearColor(.25f, .25f, .75f, .25f);

	glAlphaFunc(GL_GREATER, 0.016f);
	glEnable(GL_ALPHA_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);*/

}

void CSence2::explode(void)
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


void CSence2::exploder(float x, float y, float z, float size, float intensity, float opacity, float delay, float scale)
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
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-1.0, -1.0);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(1.0, -1.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(1.0, 1.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-1.0, 1.0);
	glEnd();
	glPopMatrix();
}

void CSence2::buildRot(float theta, float x, float y, float z, float m[16])
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

void CSence2::calcMatrix(void)
{
	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	buildRot(-180 * atan2f(mat[8], mat[10]) / M_PI, 0, 1, 0, mat);
	glMultMatrixf(mat);
}

void CSence2::DrawExplosion()
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

			//////////////////////////////////////////////////////////////////////////
	//float normals[10][3] = {
	//   {0.0, 0.0, -1.0},
	//   {0.0, 0.0, -1.0},
	//   {0.0, 0.0, 1.0},
	//   {0.0, 0.0, 1.0},
	//   {0.0, 1.0, 0.0},
	//   {0.0, 1.0, 0.0},
	//   {1.0, 0.0, 0.0},
	//   {1.0, 0.0, 0.0},
	//   {-1.0, 0.0, 0.0},
	//   {-1.0, 0.0, 0.0}};
	//float axes[10][3] = {
	//   {-1.0, 1.0, 0.0},
	//   {1.0, -1.0, 0.0},
	//   {-1.0, 1.0, 0.0},
	//   {1.0, -1.0, 0.0},
	//   {1.0, 0.0, -1.0},
	//   {-1.0, 0.0, 1.0},
	//   {0.0, 1.0, -1.0},
	//   {0.0, -1.0, 1.0},
	//   {0.0, 1.0, -1.0},
	//   {0.0, -1.0, 1.0}};
	//float omega[10] = {20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0};
	//   int i;

	//   gluLookAt(-sinf(RAD(0)) * 5.5, 0, cosf(RAD(0)) * 5.5, 0., 0., 0., 0., 1., 0.);
	//   glColor4f(0.f, .2f, 0.f, 1.f);
	// 
	//   glEnable(GL_LIGHTING);
	//   glPushMatrix();
	//	   glColor3f(.3f, .3f, .3f);
	//	   glPushMatrix();
	//		   glTranslatef(-1.f, -1.f + .2f, -1.5f);
	//		   glScalef(.2f, .2f, .2f);

	//		   for (i = 0; i < 10; i++) 
	//		   {
	//				float ax = (corners[3 * i][0] + corners[3 * i + 1][0] + corners[3 * i + 2][0]) / 3,
	//				ay = (corners[3 * i][1] + corners[3 * i + 1][1] + corners[3 * i + 2][1]) / 3,
	//				az = (corners[3 * i][2] + corners[3 * i + 1][2] + corners[3 * i + 2][2]) / 3;
	//		   }
	//	   glPopMatrix();


	//	   glDisable(GL_LIGHTING);//如果允许灯光效果，amibient就会是InitLight中的淡绿色背景色。
	//	   glTranslatef(-1.f, -1.f, -1.5f);
	//	   glEnable(GL_TEXTURE_2D);
	//	   glBindTexture(GL_TEXTURE_2D, texnames[1]);
	//	   glDepthMask(0);
	//	   //time++;
	//	   glDisable(GL_TEXTURE_2D);
	//	   glDisable(GL_DEPTH_TEST);
	//	   exploder(0.f, 0.f, 0.f, m_fSize, m_fIntensity, m_fOpacity, 0.f, 1.0/*3.f*/);
	//	   
	//	   glEnable(GL_DEPTH_TEST);
	//	   glDepthMask(1);
	//   glPopMatrix();
	//   glDisable(GL_TEXTURE_2D);
}

void CSence2::DrawClippane1()
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

void CSence2::DrawClippane3()
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
void CSence2::DrawClippane2()
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
void CSence2::DrawClippane4()
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

void CSence2::DrawPlane()
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

void CSence2::DrawStaticPlaneAndSignal()
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
void CSence2::DrawSignal( Point3D start, Point3D end) 
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
	glColor3f(0.0, 1.0, 0.0);

	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f(x1,y1,z1);
	glVertex3f(x2,y2,z2);
	glEnd();
	glPopMatrix();

	glDisable(GL_LINES) ;
	glLineWidth(1);
}

void CSence2::drawCNString(float x, float y, float z, const char* str)
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

void CSence2::selectFont(int size, int charset, const char* face) 
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_BOLD, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(/*wglGetCurrentDC()*/m_hDC, hFont);
	DeleteObject(hOldFont);
}

void CSence2::SetStartExp(BOOL bStart)
{
	m_bStartExplosion = bStart;
}
void CSence2::SetStartPlane(BOOL bStart)
{
	m_bStartCollData = bStart;
	SetTimer(m_hWnd, TIMER_GET_DATA, 3500, NULL);
}

