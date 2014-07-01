//lee add 2014-06-28

#pragma once

//usage-------
//void OnPaint()
//{
//	CPaintDC dc(this); 
//
//	RECT rcClient;
//	GetClientRect(&rcClient);
//
//	CMemoryDC memDC(dc, rcClient);
//	DrawBackGround(&memDC);
//	DrawItems(&memDC);
//	DrawLastItem(&memDC);
//}


class CMemoryDC
{
public:
	CMemoryDC();
	CMemoryDC(HDC hdc, RECT rc, bool bAutoReleaseDc = false);
	~CMemoryDC();

	void FromHdc(HDC hdc, RECT rc);
	void Output();

	HDC GetHdc()
	{
		return m_hdcMemory;
	}

	CDC* GetCdc()
	{
		return CDC::FromHandle(m_hdcMemory);
	}

	operator HDC()
	{
		return GetHdc();
	}

private:
	void Clear();

private:
	HDC m_hdcMemory;
	HDC m_hdcSrc;
	HBITMAP m_hbmpPlaceHolder;
	HBITMAP m_hbmpOld;
	RECT m_rcClip;
	HFONT m_hfontOld;

	bool m_bOutputed;
	bool m_bAutoReleaseDc;
};