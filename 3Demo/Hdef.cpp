
#include "stdafx.h"
#include "std.h"
#include "HDef.h"
#include <stdio.h>

extern HWND g_hwnd;

bool HErrorAff(char *s,char *file,int line)
{
	static char buf[1024];
	char *f;
	f=file;
	while (*f) ++f;
	while (f>file && f[-1]!='\\' && f[-1]!='/' && f>file) --f;
	sprintf(buf,"%s(%d)",f,line);
	//MessageBox(g_hwnd,s,buf,MB_ICONERROR|MB_OK);lee
	return r_err;
}

bool HMsgAff(char *s,char *file,int line)
{
	static char buf[1024];
	char *f;
	f=file;
	while (*f) ++f;
	while (f>file && f[-1]!='\\' && f[-1]!='/' && f>file) --f;
	sprintf(buf,"%s(%d)",f,line);
	//int m=MessageBox(g_hwnd,s,buf,MB_ICONWARNING|MB_OKCANCEL);
	//lee
	return/* m==*/IDOK;
}

char HTextBuf[1024];
char *HText(char *s,...)
{
	va_list arglist;
	va_start( arglist, s );
	vsprintf( HTextBuf,s, arglist );
	va_end( arglist );
	return HTextBuf;
}
