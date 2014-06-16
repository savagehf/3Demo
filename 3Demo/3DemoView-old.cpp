
// 3DemoView.cpp : implementation of the C3DemoView class
//

#include "stdafx.h"
#include "3Demo.h"

#include "3DemoDoc.h"
#include "3DemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// C3DemoView

IMPLEMENT_DYNCREATE(C3DemoView, CView)

BEGIN_MESSAGE_MAP(C3DemoView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &C3DemoView::OnFilePrintPreview)
END_MESSAGE_MAP()

// C3DemoView construction/destruction

C3DemoView::C3DemoView()
{
	// TODO: add construction code here

}

C3DemoView::~C3DemoView()
{
}

BOOL C3DemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// C3DemoView drawing

void C3DemoView::OnDraw(CDC* /*pDC*/)
{
	C3DemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// C3DemoView printing


void C3DemoView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL C3DemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void C3DemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void C3DemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void C3DemoView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void C3DemoView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// C3DemoView diagnostics

#ifdef _DEBUG
void C3DemoView::AssertValid() const
{
	CView::AssertValid();
}

void C3DemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

C3DemoDoc* C3DemoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C3DemoDoc)));
	return (C3DemoDoc*)m_pDocument;
}
#endif //_DEBUG


// C3DemoView message handlers
