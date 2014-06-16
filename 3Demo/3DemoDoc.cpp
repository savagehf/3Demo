
// 3DemoDoc.cpp : implementation of the C3DemoDoc class
//

#include "stdafx.h"
#include "3Demo.h"

#include "3DemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// C3DemoDoc

IMPLEMENT_DYNCREATE(C3DemoDoc, CDocument)

BEGIN_MESSAGE_MAP(C3DemoDoc, CDocument)
END_MESSAGE_MAP()


// C3DemoDoc construction/destruction

C3DemoDoc::C3DemoDoc()
{
	// TODO: add one-time construction code here

}

C3DemoDoc::~C3DemoDoc()
{
}

BOOL C3DemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_strTitle = "Ôàµ¯Ì½Ñ°Ä£Äâ";

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// C3DemoDoc serialization

void C3DemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// C3DemoDoc diagnostics

#ifdef _DEBUG
void C3DemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void C3DemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// C3DemoDoc commands
