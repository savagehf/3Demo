// Textures.cpp: implementation of the CTextures class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Textures.h"
#include "include/gl/glaux.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define TEXTURE_LOAD_ERROR	_T("Failed to load texture")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextures::CTextures()
{

}

CTextures::~CTextures()
{

}

bool CTextures::Load ( char *fileName, int width, int height, int depth, GLenum colorType, GLenum filterType )
{
   
   FILE *file = fopen(fileName, "rb");

   if (file == NULL )   {
      return false;
   }

   GLubyte *rawBitmap = (GLubyte *)malloc(width * height * depth * (sizeof(GLubyte)));

   if ( rawBitmap == NULL ) {
      fclose ( file );
      return false;
   }

   fread  ( rawBitmap , width * height * depth, 1 , file );
   fclose ( file);

   //  设置过滤类型
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType );

   //  设置纹理环境
   glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

   gluBuild2DMipmaps ( GL_TEXTURE_2D, colorType, width, height, colorType,
                       GL_UNSIGNED_BYTE, rawBitmap );

   free ( rawBitmap );
   return true;
}

void CTextures::LoadTextures(GLuint *texture_id, int size)
{
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(size, texture_id);

	GLuint nID = *texture_id;
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	if(!LoadBMP(_T("ModelData\\EARTH.bmp"), texture_id))
	{
		MessageBox(NULL, TEXTURE_LOAD_ERROR, _T("ERROR"), MB_OK);
	}

	/*int textureNum = 3;
	char* textureName[3] = {"ModelData\\earth.raw","ModelData\\mars.raw","ModelData\\jupiter.raw"};

	for(int i=0; i<3; i++){
		glBindTexture(GL_TEXTURE_2D, texture_id[i]);
		if (!Load(textureName[i], 640, 320, 3, GL_RGB, GL_NEAREST)) {
			MessageBox(NULL, TEXTURE_LOAD_ERROR, _T("Error"), MB_OK);
			exit(1);
		}
	}*/
}


bool CTextures::LoadBMP(LPCTSTR Filename, GLuint *texture_id)									// Load Bitmaps And Convert To Textures
{
	AUX_RGBImageRec *pImage = NULL;
	FILE *pFile = NULL;

	if(!Filename) 
		return false;

	// Open a file pointer to the BMP file and check if it was found and opened 
	if((pFile = _tfopen(Filename, _T("rb"))) == NULL) 
	{
		// Display an error message saying the file was not found, then return NULL
		TCHAR buf[255];
		//sprintf(buf,"Unable to load BMP File:%s",Filename);
		//MessageBox(NULL, buf, _T("Error"), MB_OK);
		MessageBox(NULL, TEXTURE_LOAD_ERROR, _T("ERROR"), MB_OK);
		ExitProcess(0);
		return false;
	}
	// Load the bitmap using the aux function stored in glaux.lib
	pImage = auxDIBImageLoad(Filename);				

	// Make sure valid image data was given to pImage, otherwise return false
	if(pImage == NULL)								
		return false;

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, texture_id/*&m_nTxt*/);

	// This sets the alignment requirements for the start of each pixel row in memory.
	//	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, *texture_id/*m_nTxt*/);

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, 
		pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);

	
	//Assign the mip map levels and texture info
	//wenjie控制滤波
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	
	//wenjie说明映射方式
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);



	// Now we need to free the image data that we loaded since openGL stored it as a texture
	if (pImage)										// If we loaded the image
	{
		if (pImage->data)							// If there is texture data
		{
			free(pImage->data);						// Free the texture data, we don't need it anymore
		}
		free(pImage);								// Free the image structure
	}
	return true;
}