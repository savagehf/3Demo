// LeftDockWnd.cpp : implementation file
//

#include "stdafx.h"
#include "3Demo.h"
#include "LeftDockWnd.h"

IMPLEMENT_DYNAMIC(CLeftDockWnd, CDialog)

CLeftDockWnd::CLeftDockWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CLeftDockWnd::IDD, pParent)
	, m_fLongitude(0)
	, m_fLAttitude(0)
	, m_fHeight(0.0)
{
	m_nItemCount = 0;
}

CLeftDockWnd::~CLeftDockWnd()
{
}

void CLeftDockWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlResults);
	DDX_Text(pDX, IDC_EDIT_LONGITUDE, m_fLongitude);
	DDX_Text(pDX, IDC_EDIT_LATTITUDE, m_fLAttitude);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_fHeight);
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

	DWORD dwStyle = m_ctrlResults.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;        // 选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;            // 网格线（只适用与report风格的listctrl）
	m_ctrlResults.SetExtendedStyle(dwStyle);

	m_ctrlResults.InsertColumn(0, _T("经度"), LVCFMT_LEFT, 65);
	m_ctrlResults.InsertColumn(1, _T("纬度"), LVCFMT_LEFT, 65);
	m_ctrlResults.InsertColumn(2, _T("高度"), LVCFMT_LEFT, 65);
	m_ctrlResults.InsertColumn(3, _T("浓度"), LVCFMT_LEFT, 65);

	
	return TRUE;
}


void CLeftDockWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

// CLeftDockWnd message handlers

void CLeftDockWnd::OnBnClickedButtonCalc()
{
	m_fLongitude = 125.9f;
	m_fLAttitude = 198.4f;
	m_fHeight	 = 2010.45;

	UpdateData(FALSE);
}

#define MAX_COUNT 6
void CLeftDockWnd::AddOneItem(/*float fDensity, float fHeight, float fLatitude, float fLogitude*/)
{
// 	if (m_nItemCount < 6)
// 	{
		float f1 = 125.0 + (rand()%10)/10.0f;
		float f2 = 198.0 + (rand()%10)/10.0f;
		float f3 = 2000.0 +(rand()%100)/100.0f;
		float f4 = 89.45 + (rand()%10)/10.0f;
		CString str;
		str.Format(_T("%.2f"), f1);
		m_ctrlResults.InsertItem(m_nItemCount, str.GetBuffer());

		str.Empty();
		str.Format(_T("%.2f"), f2);
		m_ctrlResults.SetItemText(m_nItemCount, 1, str.GetBuffer());
		
		str.Empty();
		str.Format(_T("%.2f"), f3);
		m_ctrlResults.SetItemText(m_nItemCount, 2, str.GetBuffer());

		str.Empty();
		str.Format(_T("%.2f"), f4);
		m_ctrlResults.SetItemText(m_nItemCount, 3, str.GetBuffer());

		
		m_nItemCount++;
//	}
}