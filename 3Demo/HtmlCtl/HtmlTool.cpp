// MyFun.cpp: implementation of the MyFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HtmlTool.h"

#include "urlmon.h"
typedef HRESULT STDAPICALLTYPE SHOWHTMLDIALOGFN
 (HWND hwndParent, IMoniker
*pmk, VARIANT *pvarArgIn, TCHAR* pchOptions,
   VARIANT *pvArgOut);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHtmlTool::CHtmlTool()
{

}

CHtmlTool::~CHtmlTool()
{

}

////////////////////////////////////////////////////////////////
// 说明: 利用ShowHTMLDialog对话框打开网页
// 作者：依星(airen3339@163.com)
// 日期：2005.12
////////////////////////////////////////////////////////////////
BOOL CHtmlTool::ShowHtml(LPWSTR url)
{
	//装载动态连
   HINSTANCE hinstMSHTML = LoadLibrary(_T("MSHTML.DLL"));
   //此地址名称可直接用html文件名代替
   LPWSTR lpUrl=url; 
   if(hinstMSHTML)//装载动态连接库成功
   {
      SHOWHTMLDIALOGFN *pfnShowHTMLDialog;
      pfnShowHTMLDialog = (SHOWHTMLDIALOGFN*) GetProcAddress(hinstMSHTML, "ShowHTMLDialog");
      if(pfnShowHTMLDialog)
	  {
         IMoniker *moniker=NULL;
         //
         if( FAILED(CreateURLMoniker( NULL, (LPWSTR)lpUrl, &moniker ) ))
		 {
              FreeLibrary(hinstMSHTML);
              return FALSE;
		 }
         //调用ShowHTMLDialog函数显示URL上的HTML文件
         pfnShowHTMLDialog(NULL, moniker, NULL, NULL, NULL);

         if(moniker != NULL)
         moniker->Release();

         //显示成功，返回TRUE
         return TRUE;

	  }
      else //GetProcessAddress失败
         return FALSE;

      FreeLibrary(hinstMSHTML);
   }
   else //装载动态连接库失败
      return FALSE;
}

////////////////////////////////////////////////////////////////
// 说明: 利用默认浏览器打开网页
// 作者：依星(airen3339@163.com)
// 日期：2005.12
////////////////////////////////////////////////////////////////
void CHtmlTool::GetUrl(CString sURL)
{
	HKEY hkRoot,hSubKey; //定义注册表根关键字及子关键字 
	/*char*/TCHAR ValueName[256]; 
	/*unsigned char*/TCHAR DataValue[256]; 
	unsigned long cbValueName=256; 
	unsigned long cbDataValue=256; 
	/*char*/TCHAR ShellChar[256]; //定义命令行 
	DWORD dwType; 
	
	//打开注册表根关键字 
	if(RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hkRoot)==ERROR_SUCCESS) 
	{ 
		//打开子关键字 
		if(RegOpenKeyEx(hkRoot, 
			_T("htmlfile\\shell\\open\\command"), 
			0, 
			KEY_ALL_ACCESS, 
			&hSubKey)==ERROR_SUCCESS) 
		{ 
			//读取注册表，获取默认浏览器的命令行  
			RegEnumValue(hSubKey,  
				0, 
				ValueName, 
				&cbValueName, 
				NULL, 
				&dwType, 
				(LPBYTE)DataValue, 
				&cbDataValue); 
			// 调用参数（主页地址）赋值 
			/*strcpy*/_tcscpy(ShellChar,/*(char *)*/(TCHAR*)DataValue); 
			/*strcat*/_tcscat(ShellChar,sURL.GetBuffer(0)); 
			// 启动浏览器 
			WinExec((LPCSTR)ShellChar,SW_SHOW); 
			
		} 
	else 
	{
	    //关闭注册表 
	    RegCloseKey(hSubKey); 
	    RegCloseKey(hkRoot); 
	}
	}
}
