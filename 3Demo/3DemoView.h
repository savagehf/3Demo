#pragma once

#include "BtnST.h"

class CSence1;
class CSence2;
class CSence3;

class C3DemoDoc;
class C3DemoView : public CView
{
public:
	enum EDrawS
	{
		enm_Sence1 = 1,
		enm_Sence2 = 2,
		enm_Sence3 = 3,
	};
public:
	C3DemoView();
	virtual ~C3DemoView();

	BOOL RenderScene1();
	BOOL RenderSence2();
	BOOL RenderSence3();
	void StartDrawPlane();

	void LoadSence();
	void StartSimulate();
	void StartTask1();
	void StartTask2();
	void CalcPostion();
	void CalMorePos();
	void ConfirmPos();
	void StartRightTask1();
	void StartRightTask2();

protected: 
	DECLARE_DYNCREATE(C3DemoView)
	virtual void OnDraw(CDC* pDC);  
	C3DemoDoc* GetDocument();

protected:

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnStartExplosion();
	afx_msg void OnSence1();
	afx_msg void OnSence2();
	afx_msg void OnSence3();

	/*afx_msg void OnExportTxt();
	afx_msg void OnExportCsv();*/
	afx_msg void OnProjIntroduce();
	afx_msg void OnContextIntro1();
	afx_msg void OnContextIntro2();
	afx_msg void OnContextIntro3();
	afx_msg void OnContextIntro4();
	afx_msg void OnContextIntro5();
	afx_msg void OnContextIntro6();
	afx_msg void OnContextIntro7();
	afx_msg void OnPrevPage();
	afx_msg void OnNextPage();

	DECLARE_MESSAGE_MAP()


protected:
	CButtonST* m_pBtnPrevPage;
	CButtonST* m_pBtnNextPage;
	CSence1* m_pSence1;
	CSence2* m_pSence2;
	CSence3* m_pSence3;
	EDrawS   m_eDrawSence;
	UINT	 m_uIDRes;
	BOOL	 m_bDraw3D;

};

#ifndef _DEBUG  // debug version in 3DemoView.cpp
inline C3DemoDoc* C3DemoView::GetDocument()
   { return (C3DemoDoc*)m_pDocument; }
#endif
