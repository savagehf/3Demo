// LeftDockWnd.cpp : implementation file
//

#include "stdafx.h"
#include "3Demo.h"
#include "LeftDockWnd.h"

IMPLEMENT_DYNAMIC(CLeftDockWnd, CDialog)

CLeftDockWnd::CLeftDockWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CLeftDockWnd::IDD, pParent)
	
{
	
}

CLeftDockWnd::~CLeftDockWnd()
{
}

void CLeftDockWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	

	//DDX_Text(pDX, IDC_EDIT_HEIGHT, m_fHeight);
}


BEGIN_MESSAGE_MAP(CLeftDockWnd, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CLeftDockWnd::OnBnClickedButtonCalc)
END_MESSAGE_MAP()

int CLeftDockWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_CHECKBOXES;


	return 0;
}


BOOL CLeftDockWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	
	return TRUE;
}


void CLeftDockWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

// CLeftDockWnd message handlers

void CLeftDockWnd::OnBnClickedButtonCalc()
{
	
}

