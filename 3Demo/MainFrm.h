

#pragma once
#include "FileView.h"
#include "ChartView.h"


#define WM_ADD_ONE_DATA		WM_USER+101
#define WM_EXPORT_AS_TEXT	WM_USER+201
#define WM_EXPORT_AS_CSV	WM_USER+202

#define WM_LOAD_SENCE		WM_USER+208
#define WM_START_SIMULATE	WM_USER+209
#define WM_START_TASK_1		WM_USER+210
#define WM_START_TASK_2		WM_USER+211
#define WM_CALC_POSITION	WM_USER+212
#define WM_MORE_POSITION	WM_USER+213
#define WM_CONFIRM_POS		WM_USER+214

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)


public:
	void SendDataChart1(float fCurPos, float fDensity);
	void SendDataChart2(float fCurPos, float fDensity);


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
	afx_msg LRESULT OnLoadSence(WPARAM w, LPARAM l);
	afx_msg LRESULT OnStartSimulate(WPARAM w, LPARAM l);
	afx_msg LRESULT OnStartTask1(WPARAM W, LPARAM l);
	afx_msg LRESULT OnStartTask2(WPARAM w, LPARAM l);
	afx_msg LRESULT OnCalcPosition(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMorePosition(WPARAM w, LPARAM l);
	afx_msg LRESULT OnConfirmPos(WPARAM w, LPARAM l);
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


