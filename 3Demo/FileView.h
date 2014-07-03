
#pragma once

#include "LeftDockWnd.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// Construction
public:
	CFileView();

	void AddOneData(/*float fDensity, float fHeight, float fLatitude, float fLogitude*/);
	//void ExportAsCSV();
	//void ExportAsText();

protected:
	void AdjustLayout();
	void OnChangeVisualStyle();
	BOOL GetCurrentAppPath(CString& strAppPath);

// Attributes
protected:

	CLeftDockWnd* m_pLeftWnd;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;
protected:
	void FillFileView();

// Implementation
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
};

