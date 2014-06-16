
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "3Demo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CFileView::CFileView()
{
	m_pLeftWnd = NULL;
}

CFileView::~CFileView()
{
	if (NULL != m_pLeftWnd)
	{
		m_pLeftWnd->DestroyWindow();
		delete m_pLeftWnd;
		m_pLeftWnd = NULL;
	}
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	m_pLeftWnd = new CLeftDockWnd;
	m_pLeftWnd->Create(CLeftDockWnd::IDD, this);
	m_pLeftWnd->ShowWindow(SW_SHOW);

	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	if (NULL != m_pLeftWnd)
	{
		m_pLeftWnd->SetWindowPos(NULL, rectClient.left,rectClient.top,
			rectClient.Width(),rectClient.Height(),SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	
}


void CFileView::AddOneData(/*float fDensity, float fHeight, float fLatitude, float fLogitude*/)
{
	if (m_pLeftWnd != NULL)
	{
		m_pLeftWnd->AddOneItem(/*fDensity,fHeight,fLatitude, fLogitude*/);
	}
}

void CFileView::ExportAsText()
{
	//2.show save as txt dialog and create saved file.
	CString strAppPath;
	GetCurrentAppPath(strAppPath);
	CString strLogFileName;
	CTime time = CTime::GetCurrentTime();
	CString strSavedFile;

	strLogFileName = _T("数据") + time.Format("%d-%b-%y %H;%M;%S") + _T(".txt");
	CFileDialog fileDlg(FALSE , _T("txt"), strLogFileName,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text Files (*.txt)|*.txt||"), this);

	fileDlg.m_ofn.lpstrInitialDir = strAppPath + strLogFileName;
	fileDlg.m_ofn.lpstrTitle = _T("保存为txt");

	if (fileDlg.DoModal() != IDOK)
		return;

	strSavedFile = fileDlg.GetPathName();

	CString strDelimeter(_T("\t"));
	CStdioFile file;
	if(!file.Open(strSavedFile, CFile::modeCreate|CFile::modeWrite))
	{
		CString strError;
		strError.Format("创建文件[%s]失败",strSavedFile);
		AfxMessageBox(strError);
		return;
	}

	CString strTemp;
	strTemp.Format("%s\t%s\t%s\t%s\t\n", "经度","纬度","高度", "浓度");
	file.WriteString(strTemp);
	int nCount = m_pLeftWnd->m_ctrlResults.GetItemCount();
	if (nCount == 0)
	{
		AfxMessageBox("没有采样数据可以保存！");
		return;
	}
	for (int nRow=0; nRow<nCount; nRow++)
	{
		strTemp.Empty();
		for (int nCol=0; nCol<4; nCol++)
		{
			CString strItem = m_pLeftWnd->m_ctrlResults.GetItemText(nRow, nCol);
			strTemp += strItem;
			strTemp += strDelimeter;
		}
		strTemp += _T("\n");
		file.WriteString(strTemp);
	}
	
	file.Close();

	AfxMessageBox(_T("保存成功"));
}
void CFileView::ExportAsCSV()
{
	//2.show save as txt dialog and create saved file.
	CString strAppPath;
	GetCurrentAppPath(strAppPath);
	CString strLogFileName;
	CTime time = CTime::GetCurrentTime();
	CString strSavedFile;

	strLogFileName = _T("数据") + time.Format("%d-%b-%y %H;%M;%S") + _T(".CSV");
	CFileDialog fileDlg(FALSE , _T("txt"), strLogFileName,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text Files (*.CSV)|*.CSV||"), this);

	fileDlg.m_ofn.lpstrInitialDir = strAppPath + strLogFileName;
	fileDlg.m_ofn.lpstrTitle = _T("保存为CSV");

	if (fileDlg.DoModal() != IDOK)
		return;

	strSavedFile = fileDlg.GetPathName();

	CString strDelimeter(_T(","));
	CStdioFile file;
	if(!file.Open(strSavedFile, CFile::modeCreate|CFile::modeWrite))
	{
		CString strError;
		strError.Format("创建文件[%s]失败",strSavedFile);
		AfxMessageBox(strError);
		return;
	}

	CString strTemp;
	strTemp.Format("%s,%s,%s,%s,\n", "经度","纬度","高度", "浓度");
	file.WriteString(strTemp);
	
	int nCount = m_pLeftWnd->m_ctrlResults.GetItemCount();
	if (nCount == 0)
	{
		AfxMessageBox("没有采样数据可以保存！");
		return;
	}

	for (int nRow=0; nRow<nCount; nRow++)
	{
		strTemp.Empty();
		for (int nCol=0; nCol<4; nCol++)
		{
			CString strItem = m_pLeftWnd->m_ctrlResults.GetItemText(nRow, nCol);
			strTemp += strItem;
			strTemp += strDelimeter;
		}
		strTemp += _T("\n");
		file.WriteString(strTemp);
	}

	file.Close();

	AfxMessageBox(_T("保存成功"));
}

//"D:\..\Exec\"
BOOL CFileView::GetCurrentAppPath(CString& strAppPath)
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