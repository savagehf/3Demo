#pragma once

#include "resource.h"
#include "ChartCtrl/ChartCtrl.h"


class CDlgChart : public CDialog
{
	DECLARE_DYNAMIC(CDlgChart)

public:
	CDlgChart(CWnd* pParent = NULL);  
	virtual ~CDlgChart();

	enum { IDD = IDD_DLG_CHART };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:

	CChartCtrl m_ChartCtrlFisrt;
	CChartCtrl m_ChartCtrlSecond;

	int m_nFirstSerieID;
	int m_nSecSerieID;
	double m_dXValue;
	double m_dYValue;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartExplore1();
	afx_msg void OnBnClickedStartExplore2();
};
