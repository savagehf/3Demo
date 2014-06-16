// RImage.h: interface for the RImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIMAGE_H__A66B9427_EFBA_11D3_837B_00A0C9DE7E0E__INCLUDED_)
#define AFX_RIMAGE_H__A66B9427_EFBA_11D3_837B_00A0C9DE7E0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ptypes.h"

class RImage;
typedef bool (*fncImageLoader)(char* szImageName, RImage *img);

class RImage
{
public:
	RImage();
	virtual ~RImage();

  // Create using an image loader.
  // Usage:   myTexture.Create( "Nifty.bmp", imgLoadBMP );
  bool Create( char *szImageName, fncImageLoader imgLoader);

  // Create using basic parameters to allocate an image buffer.
  // Usage:   myImage.Create( 128, 256, 24 );  // Creates a 128x256x24bpp image.
  bool Create( UInt32 width, UInt32 height, UInt32 bpp );
  virtual void Toast();

  int     GetBPP()    {return m_nBPP;}    // Bits per pixel. Valid types are 8(intensity), 24(RGB), 32(RGBA)
  UInt32  GetWidth()  {return m_nWidth;}  // Width of image.
  UInt32  GetStride() {return m_nStride;} // Number of bytes in each horizontal scan line.
  UInt32  GetHeight() {return m_nHeight;} // Height of image.

  UInt32  GetImageSize() {return m_nData;}// Allocated pixel data space used by image.
  void*   GetData()   {return m_pData;}   // Get a pointer to the pixel data.

protected:
  int     m_nBPP;
  UInt32  m_nWidth, m_nHeight, m_nStride;
  UInt32  m_nData;
  void*   m_pData;
};

#endif // !defined(AFX_RIMAGE_H__A66B9427_EFBA_11D3_837B_00A0C9DE7E0E__INCLUDED_)
