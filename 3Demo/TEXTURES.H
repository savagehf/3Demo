// Textures.h: interface for the CTextures class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURES_H__7586694F_51C1_4B03_9941_6E04EA1E2453__INCLUDED_)
#define AFX_TEXTURES_H__7586694F_51C1_4B03_9941_6E04EA1E2453__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTextures  
{
public:
	CTextures();
	virtual ~CTextures();

	void LoadTextures(GLuint *texture_id, int MaxNrOfTextures);
private:
	bool LoadBMP(LPCTSTR Filename, GLuint *texture_id);
	bool Load (char *file_name, int width, int height, int depth, GLenum colour_type, GLenum filter_type);
};

#endif // !defined(AFX_TEXTURES_H__7586694F_51C1_4B03_9941_6E04EA1E2453__INCLUDED_)
