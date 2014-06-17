

#pragma once
#include "FileView.h"
#include "ChartView.h"


#define WM_ADD_ONE_DATA		WM_USER+101
#define WM_EXPORT_AS_TEXT	WM_USER+201
#define WM_EXPORT_AS_CSV	WM_USER+202

#define WM_START_TASK_1		WM_USER+210
#define WM_START_TASK_2		WM_USER+211

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CChartView		  m_wndChartView;
	
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnStartTask1(WPARAM W, LPARAM l);
	afx_msg LRESULT OnStartTask2(WPARAM w, LPARAM l);
	afx_msg LRESULT OnAddOneData(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnExportAsText(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnExportAsCSV(WPARAM wp, LPARAM lp);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
};


