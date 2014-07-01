// MyFun.h: interface for the MyFun class.
// 说明: 打开指定地址的网页函数
// 作者：依星(airen3339@163.com)
// 日期：2005.12
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYFUN_H__12A0BD00_3E77_40B3_B43A_AD80C2BD3611__INCLUDED_)
#define AFX_MYFUN_H__12A0BD00_3E77_40B3_B43A_AD80C2BD3611__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHtmlTool  
{
public:
	void GetUrl(CString sURL);
	BOOL ShowHtml(LPWSTR url);
	CHtmlTool();
	virtual ~CHtmlTool();

};

#endif // !defined(AFX_MYFUN_H__12A0BD00_3E77_40B3_B43A_AD80C2BD3611__INCLUDED_)
