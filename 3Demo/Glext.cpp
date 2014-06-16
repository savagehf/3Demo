//-------------------------------------------------------------------
//	File:		glExt.cpp
//	Created:	10/12/99 19:30:PM
//	Author:		Allard Jeremie
//	Comments:	Find and import OpenGL Extensions.
//-------------------------------------------------------------------
#include "stdafx.h"
#include "Std.h"
#include "HDef.h"
#include "glExt.h"
#include <stdio.h>

bool glGetExtension(glExtension *ext)
{
	if (ext->initok) return r_ok;
	char *strExt=(char*)glGetString(GL_EXTENSIONS);
	bool found=false;
	char *s;
	char *s2;
	//HMsg(strExt);
	s = strExt;
	while( *s )
	{
		s2 = s;
		while( *s2 && *s2 != ' ' ) 
			++s2;
		if (s<s2)
		if( strncmp( s,ext->name, s2-s)==0 )
		{
			found=true;
			break;
		}
		s=s2+1;
	}
	if (!found)
	{
		return HErrorN(HText("glExtension %s not found",ext->name));
		//return r_err;
	}
	int f;
	for (f=0;f<ext->nbfn;++f)
	{
		void *proc=wglGetProcAddress(ext->tblfn[f].name);
		if (!proc) return HErrorN(HText("Fonction %s of extension %s not found",ext->tblfn[f].name,ext->name));
		*ext->tblfn[f].ptr=proc;
	}
	ext->initok=true;
	return r_ok;
}
