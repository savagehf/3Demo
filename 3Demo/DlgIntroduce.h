#pragma once

#include "HtmlCtl\HtmlCtrl.h"
#include "HtmlCtl\HtmlTool.h"

class CDlgIntroduce : public CDialog
{
	DECLARE_DYNAMIC(CDlgIntroduce)

public:
	CDlgIntroduce(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIntroduce();
	virtual BOOL OnInitDialog();
	CString GetCurrentPath();


	enum { IDD = IDD_DLG_INTRODUCTION };


public:
	CHtmlCtrl pHtmlPage;
	CString m_strHtmlBasePath ;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
