#pragma once

#include "resource.h"
#include "ChartCtrl/ChartCtrl.h"
#include "BtnST.h"
#include <vector>

using namespace std;

class CDlgChart : public CDialog
{
	DECLARE_DYNAMIC(CDlgChart)

public:
	CDlgChart(CWnd* pParent = NULL);  
	virtual ~CDlgChart();

	enum { IDD = IDD_DLG_CHART };

	void RemoveAllError1Pts();
	void RemoveAllError2Pts();
	void AddErrorData1(float fCurPos);
	void AddErrorData2(float fCurPos);
	void AddData1(float fPos, float fDesity);
	void AddData2(float fPos, float fDesity);
	void AddOneBomb(/*float x, float y, float fDesity*/);
	void ImportAdapter(CString& strImport);
	void ExportAdapter(CString& strOut);

	struct SDataItem
	{
		float fx;
		float fy;
	};


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:

	CChartCtrl m_ChartCtrlFisrt;
	//CChartCtrl m_ChartCtrlSecond;

	int m_nFirstSerieID;
	//int m_nSecSerieID;
	double m_dXValue;
	double m_dYValue;
	COLORREF m_crPoints1;
	COLORREF m_crPoints2;

	CButtonST m_colorBtnLoadScene;
	CButtonST m_colorBtnStart;
	CButtonST m_colorBtnRoute1;
	CButtonST m_colorBtnRoute2;
	CButtonST m_colorBtnCalc;
	CButtonST m_colorBtnMorePos;
	CButtonST m_colorBtnConfirm;

	CListCtrl m_ctrlResults;
	float m_fLongitude;
	float m_fLAttitude;
	int m_nItemCount;
	vector<SDataItem> m_vecBombDatas;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartExplore1();
	afx_msg void OnBnClickedStartExplore2();
	afx_msg void OnBnClickedBtnGetPosition();
	afx_msg void OnBnClickedBtnMorePos();
	afx_msg void OnBnClickedBtnSpecifyPos();
	afx_msg void OnBnClickedBtnLoadSence();
	afx_msg void OnBnClickedBtnStartSimulate();
};
