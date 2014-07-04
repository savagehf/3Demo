#pragma once

#include "DlgChart.h"


class CChartView : public CDockablePane
{
public:
	CChartView();
	virtual ~CChartView();

	void AddErrorData1();
	void AddErrorData2();
	void AddChart1Data(float fCurPos, float fDensity);
	void AddChart2Data(float fCurPos, float fDensity);
	void AddBombData(/*float x, float y, float fDesity*/);

protected:
	void AdjustLayout();
	BOOL GetCurrentAppPath(CString& strAppPath);

protected:
	CDlgChart* m_pChartDlg;
// 	CImageList m_FileViewImages;
// 	CFileViewToolBar m_wndToolBar;
protected:
	//void FillFileView(); 
	

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
};

