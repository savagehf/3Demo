#pragma once
#include "resource.h"
#include "afxcmn.h"

class CLeftDockWnd : public CDialog
{
	DECLARE_DYNAMIC(CLeftDockWnd)

public:
	CLeftDockWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLeftDockWnd();

	

// Dialog Data
	enum { IDD = IDD_DIALOG_LEFT_DOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support    
	BOOL OnInitDialog();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedButtonCalc();
	


	
};
