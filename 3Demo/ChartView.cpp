
#include "stdafx.h"
#include "mainfrm.h"
#include "chartview.h"
#include "Resource.h"
#include "3Demo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CChartView::CChartView()
{
	m_pChartDlg = NULL;
}

CChartView::~CChartView()
{
	if (NULL != m_pChartDlg)
	{
		m_pChartDlg->DestroyWindow();
		delete m_pChartDlg;
		m_pChartDlg = NULL;
	}
}

BEGIN_MESSAGE_MAP(CChartView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int CChartView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	m_pChartDlg = new CDlgChart;
	m_pChartDlg->Create(CDlgChart::IDD, this);
	m_pChartDlg->ShowWindow(SW_SHOW);

	AdjustLayout();

	return 0;
}

void CChartView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CChartView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	if (NULL != m_pChartDlg)
	{
		m_pChartDlg->SetWindowPos(NULL, rectClient.left,rectClient.top,
			rectClient.Width(),rectClient.Height(),SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CChartView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CChartView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
}
//"D:\..\Exec\"
BOOL CChartView::GetCurrentAppPath(CString& strAppPath)
{
	TCHAR szFullExeName[MAX_PATH];
	DWORD nResult = ::GetModuleFileName(NULL, szFullExeName, MAX_PATH);
	if (0 == nResult)
	{
		return FALSE;
	}

	CString strFullFileName(szFullExeName);
	UINT nIndex = strFullFileName.ReverseFind('\\');
	strAppPath = strFullFileName.Left(nIndex+1);

	return TRUE;
}

void CChartView::AddChart1Data(float fCurPos, float fDensity)
{
	if (NULL != m_pChartDlg)
	{
		m_pChartDlg->AddData1(fCurPos, fDensity);
	}
}
void CChartView::AddChart2Data(float fCurPos, float fDensity)
{
	if (NULL != m_pChartDlg)
	{
		m_pChartDlg->AddData2(fCurPos, fDensity);
	}
}
void CChartView::AddBombData(/*float x, float y, float fDesity*/)
{
	if (NULL != m_pChartDlg)
	{
		m_pChartDlg->AddOneBomb(/*x, y, fDesity*/);
	}
}