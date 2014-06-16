#ifndef __HDEF_H__
#define __HDEF_H__

#include "Ptypes.h"


//
//typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
//typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);
//


typedef UInt8 Byte;
typedef Int8 SByte;
typedef UInt16 Word;
typedef Int16 SWord;
typedef UInt32 DWord;
typedef Int32 SDWord;
#define INLINE __inline

typedef bool r_val;
#define r_ok true
#define r_err false
#define Test(p) if ((p)!=r_ok) return r_err; else
#define TestP(p) if ((p)==NULL) return r_err; else

//#define HErrorN(s) r_err

bool HErrorAff(char *s,char *file,int line);
#define HErrorN(msg) HErrorAff(msg,__FILE__,__LINE__)
bool HMsgAff(char *s,char *file,int line);
#define HMsg(msg) HMsgAff(msg,__FILE__,__LINE__)
char *HText(char *s,...);

#ifdef _MSC_VER
// warning C4035: function : no return value
#pragma warning(push)
#pragma warning(disable:4035)

INLINE DWord dswap(DWord i)
{
	__asm
	{
		mov eax,i;
		bswap eax;
	}
}

INLINE Word wswap(Word i)
{
	/*__asm
	{
		movzx eax,i;
		xchg ah,al;
	}*/
	return (i<<8)|(i>>8);
}

#pragma warning(pop)

#else

INLINE DWord dswap(DWord i)
{
	return (i<<24)|((i<<8)&0x00ff0000)|((i>>8)&0x0000ff00)|(i>>24);
}

INLINE Word wswap(Word i)
{
	return (i<<8)|(i>>8);
}

#endif

#endif
