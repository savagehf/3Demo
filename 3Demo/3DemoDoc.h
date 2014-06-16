
// 3DemoDoc.h : interface of the C3DemoDoc class
//


#pragma once


class C3DemoDoc : public CDocument
{
protected: // create from serialization only
	C3DemoDoc();
	DECLARE_DYNCREATE(C3DemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~C3DemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


