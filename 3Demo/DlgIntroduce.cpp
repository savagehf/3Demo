// DlgIntroduce.cpp : implementation file
//

#include "stdafx.h"
#include "3Demo.h"
#include "DlgIntroduce.h"


// CDlgIntroduce dialog

IMPLEMENT_DYNAMIC(CDlgIntroduce, CDialog)

CDlgIntroduce::CDlgIntroduce(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgIntroduce::IDD, pParent)
{

}

CDlgIntroduce::~CDlgIntroduce()
{
}

void CDlgIntroduce::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgIntroduce, CDialog)
END_MESSAGE_MAP()


BOOL CDlgIntroduce::OnInitDialog() 
{
	CDialog::OnInitDialog();

	pHtmlPage.CreateFromStatic(IDC_HTML_PAGE, this);

	m_strHtmlBasePath = GetCurrentPath();
	m_strHtmlBasePath = m_strHtmlBasePath + _T("\\Database\\html");

	pHtmlPage.Navigate2(m_strHtmlBasePath + _T("\\nuclear.htm"));	

	return TRUE;  
}
CString CDlgIntroduce::GetCurrentPath()
{
	CString path;
	GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
	path.ReleaseBuffer(); 
	int pos = path.ReverseFind('\\'); 
	path = path.Left(pos); 

	return path;
}
