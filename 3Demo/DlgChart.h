#pragma once

#include "resource.h"
#include "ChartCtrl/ChartCtrl.h"
#include "BtnST.h"

class CDlgChart : public CDialog
{
	DECLARE_DYNAMIC(CDlgChart)

public:
	CDlgChart(CWnd* pParent = NULL);  
	virtual ~CDlgChart();

	enum { IDD = IDD_DLG_CHART };

	void AddData1(float fPos, float fDesity);
	void AddData2(float fPos, float fDesity);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:

	CChartCtrl m_ChartCtrlFisrt;
	CChartCtrl m_ChartCtrlSecond;

	int m_nFirstSerieID;
	int m_nSecSerieID;
	double m_dXValue;
	double m_dYValue;

	CButtonST m_colorBtnRoute1;
	CButtonST m_colorBtnRoute2;
	CButtonST m_colorBtnCalc;
	CButtonST m_colorBtnMorePos;
	CButtonST m_colorBtnConfirm;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartExplore1();
	afx_msg void OnBnClickedStartExplore2();
	afx_msg void OnBnClickedBtnGetPosition();
	afx_msg void OnBnClickedBtnMorePos();
	afx_msg void OnBnClickedBtnSpecifyPos();
};
