
#include "stdafx.h"
#include "3Demo.h"
#include "3DemoDoc.h"
#include "3DemoView.h"
#include "mainfrm.h"
#include "Sence1.h"
#include "Sence2.h"
#include "Sence3.h"
#include "DlgIntroduce.h"



// #pragma comment( lib,"winmm.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(C3DemoView, CView)

BEGIN_MESSAGE_MAP(C3DemoView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()

	ON_COMMAND(ID_START_EXPLOSION, OnStartExplosion)
	//ON_COMMAND(ID_SENCE_1, OnSence1)
	//ON_COMMAND(ID_SENCE_2, OnSence2)
	ON_COMMAND(ID_SENCE_3, OnSence3)
	ON_COMMAND(ID_EXPORT_TXT, &C3DemoView::OnExportTxt)
	ON_COMMAND(ID_EXPORT_CSV, &C3DemoView::OnExportCsv)
	ON_COMMAND(ID_PROJ_INTRODUCE, &C3DemoView::OnProjIntroduce)
	ON_COMMAND(ID_CONTEXT_INTRO_1, &C3DemoView::OnContextIntro1)
	ON_COMMAND(ID_CONTEXT_INTRO_2, &C3DemoView::OnContextIntro2)
	ON_COMMAND(ID_CONTEXT_INTRO_3, &C3DemoView::OnContextIntro3)
	ON_COMMAND(ID_CONTEXT_INTRO_4, &C3DemoView::OnContextIntro4)
	ON_COMMAND(ID_CONTEXT_INTRO_5, &C3DemoView::OnContextIntro5)
	ON_COMMAND(ID_CONTEXT_INTRO_6, &C3DemoView::OnContextIntro6)
	ON_COMMAND(ID_CONTEXT_INTRO_7, &C3DemoView::OnContextIntro7)
END_MESSAGE_MAP()


C3DemoView::C3DemoView()
{
	m_pSence1 = NULL;
	m_pSence2 = NULL;
	m_pSence3 = NULL;
	m_eDrawSence = enm_Sence1;
	m_uIDRes  = IDB_INTRO_PIC_1;
	m_bDraw3D = FALSE;
}

C3DemoView::~C3DemoView()
{
	if (NULL != m_pSence1)
	{
		delete m_pSence1;
		m_pSence1 = NULL;
	}
	if (NULL != m_pSence2)
	{
		delete m_pSence2;
		m_pSence2 = NULL;
	}
	if (NULL != m_pSence3)
	{
		delete m_pSence3;
		m_pSence3 = NULL;
	}
}


BOOL C3DemoView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void C3DemoView::OnDraw(CDC* pDC)
{
	C3DemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!m_bDraw3D)
	{
		//CPaintDC dc(this);//error!对于CDialog可以使用PaintDC类
		CClientDC dc(this);
		CRect rect;
		GetClientRect(rect);

		CDC MemDC; 
		CBitmap MemBitmap;

		//随后建立与屏幕显示兼容的内存显示设备
		MemDC.CreateCompatibleDC(NULL);

		//这时还不能绘图，因为没有地方画
		//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
		MemBitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());

		//将位图选入到内存显示设备中
		//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
		CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);


		//1.画背景：先用背景色将位图清除干净，这里我用的是白色作为背景
		MemDC.FillSolidRect(0,0,rect.Width(),rect.Height(),RGB(255,255,255));

		//2.画前景色，例如bitmap图片：直接拷贝。
		CBitmap bmp;
		if (bmp.LoadBitmap(m_uIDRes))
		{
			BITMAP bmpInfo;
			bmp.GetBitmap(&bmpInfo);

			CDC dcMem2;
			dcMem2.CreateCompatibleDC(&MemDC);

			//将位图选入到内存DC中
			CBitmap* pOldBitmap = dcMem2.SelectObject(&bmp);

			int nXPos = rect.left + (rect.Width()-bmpInfo.bmWidth)/2;
			int nYPos = rect.top  + (rect.Height()-bmpInfo.bmHeight)/2;
			MemDC.BitBlt(nXPos, nYPos, bmpInfo.bmWidth, bmpInfo.bmHeight,
				&dcMem2, 0, 0, SRCCOPY);

			dcMem2.SelectObject(pOldBitmap);
		}


		//Flash到print dc.
		dc.BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);

		//绘图完成后的清理
		MemBitmap.DeleteObject();
		MemDC.SelectObject(pOldBit);
		MemDC.DeleteDC();
	}
	else
	{
		if (enm_Sence1 == m_eDrawSence)
		{
			RenderScene1();
		}
		else if (enm_Sence2 == m_eDrawSence)
		{
			RenderSence2();
		}
		else if (enm_Sence3 == m_eDrawSence)
		{
			RenderSence3();
		}
	}
}

#ifdef _DEBUG
C3DemoDoc* C3DemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C3DemoDoc)));
	return (C3DemoDoc*)m_pDocument;
}
#endif

int C3DemoView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;


	SetTimer(TIMER_DRAW_EXP, 20, NULL);

	//OnSence3();
	
	return 0;
}

void C3DemoView::OnDestroy() 
{
	CView::OnDestroy();
	
	KillTimer(TIMER_DRAW_EXP);
	KillTimer(TIMER_DRAW_PLANE);
	KillTimer(TIMER_EXP_OVER);
}

void C3DemoView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (enm_Sence1 == m_eDrawSence)
	{
		if (NULL != m_pSence1)
		{
			m_pSence1->ChangeViewSize(cx, cy);
		}
	}
	else if (enm_Sence2 == m_eDrawSence)
	{
		if (NULL != m_pSence2)
		{
			m_pSence2->Resize(cx, cy);
		}
	}
	else if (enm_Sence3 == m_eDrawSence)
	{
		if (NULL != m_pSence3)
		{
			m_pSence3->Resize(cx,cy);
		}
	}
}

void C3DemoView::OnTimer(UINT nIDEvent) 
{
	//必须加上，否则爆炸效果看不到
	Invalidate(FALSE);	

	if (enm_Sence1 == m_eDrawSence)
	{
		if (NULL != m_pSence1)
		{
			if (nIDEvent == TIMER_EXP_OVER)
			{
				m_pSence1->ResetExp();
				StartDrawPlane();
			}
			else
			{
				m_pSence1->OnTimer(nIDEvent);
			}
		}
	}
	else if (enm_Sence2 == m_eDrawSence)
	{
		if (NULL != m_pSence2)
		{
			if (nIDEvent == TIMER_EXP_OVER)
			{
				m_pSence2->ResetExp();
				StartDrawPlane();
			}
			else
			{
				m_pSence2->OnTimer(nIDEvent);
			}
		}
	}
	else if (enm_Sence3 == m_eDrawSence)
	{
		if(NULL != m_pSence3)
		{
			if(nIDEvent == TIMER_EXP_OVER)
			{
				m_pSence3->ResetExp();
				StartDrawPlane();
			}
			else
			{
				m_pSence3->OnTimer(nIDEvent);
			}

		}
	}
	
	CView::OnTimer(nIDEvent);
}

void C3DemoView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE)
	{
		PostMessage(WM_CLOSE);
	}
	else
	{
		if (enm_Sence1 == m_eDrawSence)
		{
			if (NULL != m_pSence1)
			{
				//m_pSence1->appKeyDown(nChar);
			}
		}
		else if (enm_Sence2 == m_eDrawSence)
		{
			if (NULL != m_pSence2)
			{
				m_pSence2->OnKeydown(nChar);
			}
		}
		else if (enm_Sence3 == m_eDrawSence)
		{
			if (NULL != m_pSence3)
			{
				m_pSence3->OnKeyDown(nChar);
			}
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void C3DemoView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (enm_Sence1 == m_eDrawSence)
	{
		if (NULL != m_pSence1)
		{
			//m_pSence1->appKeyUp(nChar);
		}
	}
	else if (enm_Sence2 == m_eDrawSence)
	{
		if(NULL != m_pSence2)
		{
			m_pSence2->OnKeyUp(nChar);
		}
	}
	else if (enm_Sence3 == m_eDrawSence)
	{
		if (NULL != m_pSence3)
		{
			m_pSence3->OnKeyUp(nChar);
		}
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void C3DemoView::StartDrawPlane()
{
	if (enm_Sence1 == m_eDrawSence)
	{
		if (NULL != m_pSence1)
		{
			m_pSence1->SetStartPlane(TRUE);
		}
	}
	else if (enm_Sence2 == m_eDrawSence)
	{
		if (NULL != m_pSence2)
		{
			m_pSence2->SetStartPlane(TRUE);
		}
	}
	else if (enm_Sence3 == m_eDrawSence)
	{
		if (NULL != m_pSence3)
		{
			m_pSence3->SetStartPlane(TRUE);
		}
	}
	
	SetTimer(TIMER_DRAW_PLANE, 200, NULL);
}

void C3DemoView::OnStartExplosion()
{
	if (enm_Sence1 == m_eDrawSence)
	{
		if (NULL != m_pSence1)
		{
			m_pSence1->SetStartExp(TRUE);
		}
	}
	else if (enm_Sence2 == m_eDrawSence)
	{
		if (NULL != m_pSence2)
		{
			m_pSence2->SetStartExp(TRUE);
		}
	}
	else if(enm_Sence3 == m_eDrawSence)
	{
		if(NULL != m_pSence3)
		{
			m_pSence3->SetStartExp(TRUE);
		}
	}	

	SetTimer(TIMER_EXP_OVER, 8000, NULL);
}

void C3DemoView::OnSence1()
{
	delete m_pSence3;
	m_pSence3 = NULL;
	delete m_pSence2;
	m_pSence2 = NULL;


	m_eDrawSence = enm_Sence1;

	if (NULL != m_pSence1)
	{
		delete m_pSence1;
		m_pSence1 = NULL;
	}

	m_pSence1 = new CSence1(this->GetSafeHwnd());
	m_pSence1->Init();

	//规避初始化后显示背景不对的情况。
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		if (pFrame->IsZoomed())
		{
			pFrame->ShowWindow(SW_RESTORE);
		}
		else
		{
			pFrame->ShowWindow(SW_SHOWMAXIMIZED);
		}
	}
	
}

BOOL C3DemoView::RenderScene1() 
{
	if (NULL != m_pSence1)
	{
		m_pSence1->RenderParticule();
	}
	
	return TRUE;
}


void C3DemoView::OnSence2()
{
	delete m_pSence1;
	m_pSence1 = NULL;
	delete m_pSence3;
	m_pSence3 = NULL;

	m_eDrawSence = enm_Sence2;

	if (NULL != m_pSence2)
	{
		delete m_pSence2;
		m_pSence2 = NULL;
	}

	m_pSence2 = new CSence2(this->GetSafeHwnd());
	m_pSence2->Init();

	//规避初始化后显示背景不对的情况。
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		if (pFrame->IsZoomed())
		{
			pFrame->ShowWindow(SW_RESTORE);
		}
		else
		{
			pFrame->ShowWindow(SW_SHOWMAXIMIZED);
		}
	}
}

BOOL C3DemoView::RenderSence2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	if (NULL != m_pSence2)
	{
		m_pSence2->Draw();
	}
	glPopMatrix();

	return TRUE;
}

void C3DemoView::OnSence3()
{
	 delete m_pSence1;
	 m_pSence1 = NULL;
	 delete m_pSence2;
	 m_pSence2 = NULL;


	m_bDraw3D = TRUE;
 	m_eDrawSence = enm_Sence3;

	if (NULL != m_pSence3)
	{
		delete m_pSence3;
		m_pSence3 = NULL;
	}

	m_pSence3 = new CSence3(this->GetSafeHwnd());
	m_pSence3->Init();

 	//规避初始化后显示背景不对的情况。
 	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
 	if (NULL != pFrame)
 	{
 		if (pFrame->IsZoomed())
 		{
 			pFrame->ShowWindow(SW_RESTORE);
 		}
 		else
 		{
 			pFrame->ShowWindow(SW_SHOWMAXIMIZED);
 		}
 	}
}
 
BOOL C3DemoView::RenderSence3()
 {
	if (NULL != m_pSence3)
	{
		m_pSence3->Draw();
	}

	return TRUE;
 }

void C3DemoView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CMenu   menu;
	VERIFY(menu.LoadMenu(IDR_MENU_VIEW_CONTX));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	//ScreenToClient(&ptCursor);
	pPopup-> TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, this);
}

void C3DemoView::OnLButtonDown(UINT nFlags, CPoint point)
{
 	if (NULL != m_pSence1)
 	{
 		m_pSence1->Setfire();
 	}
}
void C3DemoView::OnExportTxt()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		pFrame->PostMessage(WM_EXPORT_AS_TEXT);
	}
}

void C3DemoView::OnExportCsv()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		pFrame->PostMessage(WM_EXPORT_AS_CSV);
	}
}

void C3DemoView::StartTask1()
{
	if (NULL != m_pSence3)
	{
		m_pSence3->StartFlyTask1();
	}
}

void C3DemoView::StartTask2()
{
	if (NULL != m_pSence3)
	{
		m_pSence3->StartFlyTask2();
	}
}

void C3DemoView::CalcPostion()
{
	if (NULL != m_pSence3)
	{
		m_pSence3->CalcNuclearPos();
	}
}

void C3DemoView::CalMorePos()
{
	if (NULL != m_pSence3)
	{
		m_pSence3->Draw2MorePos();
	}
}

void C3DemoView::ConfirmPos()
{
	if (NULL != m_pSence3)
	{
		m_pSence3->DrawConfirmedPos();
	}
}

void C3DemoView::OnProjIntroduce()
{
	CDlgIntroduce dlg;
	if (dlg.DoModal() == IDOK)
	{

	}
}

void C3DemoView::OnContextIntro1()
{
	m_uIDRes = IDB_INTRO_PIC_1;	
}

void C3DemoView::OnContextIntro2()
{
	m_uIDRes = IDB_INTRO_PIC_2;
}

void C3DemoView::OnContextIntro3()
{
	m_uIDRes = IDB_INTRO_PIC_3;
}

void C3DemoView::OnContextIntro4()
{
	m_uIDRes = IDB_INTRO_PIC_4;
}

void C3DemoView::OnContextIntro5()
{
	m_uIDRes = IDB_INTRO_PIC_5;
}

void C3DemoView::OnContextIntro6()
{
	m_uIDRes = IDB_INTRO_PIC_6;
}

void C3DemoView::OnContextIntro7()
{
	m_uIDRes = IDB_INTRO_PIC_7;
}
