#include "StdAfx.h"
#include "MemoryDC.h"

CMemoryDC::CMemoryDC(void)
{
	m_hdcSrc = NULL;
	m_hdcMemory = NULL;
	m_hbmpPlaceHolder = NULL;
	m_hbmpOld = NULL;
	m_bOutputed = false;
	m_bAutoReleaseDc = false;
}

CMemoryDC::CMemoryDC(HDC hdc, RECT rc, bool bAutoReleaseDc/* = false*/)
{
	m_bOutputed = false;
	m_bAutoReleaseDc = bAutoReleaseDc;
	FromHdc(hdc, rc);
}

CMemoryDC::~CMemoryDC(void)
{
	if (m_bOutputed == false)
	{
		Output();
		Clear();
	}
}

void CMemoryDC::FromHdc(HDC hdc, RECT rc)
{
	m_hdcSrc = hdc;
	m_rcClip = rc;

	m_hdcMemory = ::CreateCompatibleDC(m_hdcSrc);
	//::BitBlt(m_hdcMemory, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, m_hdcSrc, 0, 0, SRCCOPY);
	m_hbmpPlaceHolder = ::CreateCompatibleBitmap(m_hdcSrc, m_rcClip.right - m_rcClip.left, m_rcClip.bottom - m_rcClip.top);
	m_hbmpOld = (HBITMAP)::SelectObject(m_hdcMemory, m_hbmpPlaceHolder);

	::SetBkColor(m_hdcMemory, 0xffffff);
	CRect rc2(rc);
	rc2.OffsetRect(-rc2.TopLeft());
	::ExtTextOut(m_hdcMemory, 0, 0, ETO_OPAQUE, &rc2, NULL, 0, NULL);

	HFONT h = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
	m_hfontOld = (HFONT)::SelectObject(m_hdcMemory, h);
}

void CMemoryDC::Output()
{
	::BitBlt(m_hdcSrc, m_rcClip.left, m_rcClip.top, m_rcClip.right - m_rcClip.left, m_rcClip.bottom - m_rcClip.top, m_hdcMemory, 0, 0, SRCCOPY);
	m_bOutputed = true;
}

void CMemoryDC::Clear()
{
	::SelectObject(m_hdcMemory, (HGDIOBJ)m_hbmpOld);
	::SelectObject(m_hdcMemory, (HGDIOBJ)m_hfontOld);
	::DeleteObject((HGDIOBJ)m_hbmpPlaceHolder);
	::DeleteDC(m_hdcMemory);

	if (m_bAutoReleaseDc)
	{
		HWND hwndSrc = ::WindowFromDC(m_hdcSrc);
		::ReleaseDC(hwndSrc, m_hdcSrc);
	}
}