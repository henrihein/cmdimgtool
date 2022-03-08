#pragma once

#define ROPPDSao            (DWORD)0x00F802E5 /* PDSao   */
#define	ROPDPSao     		(DWORD)0x00EA02E9 /* DPSao  */
#define	ROPDSna     		(DWORD)0x00220326 /* DSna  */

#define GetAValue(rgb)      (LOBYTE((rgb)>>24))
#define GetAlphaValue GetAValue

const COLORREF color_bad = RGB(1, 2, 3);
const COLORREF color_black = RGB(0, 0, 0);
const COLORREF color_white = RGB(255, 255, 255);
const COLORREF color_elite = RGB(237, 200, 27);
const COLORREF color_normal = RGB(248, 248, 248);
const COLORREF color_none = RGB(0, 0, 0);
const COLORREF color_mask = RGB(0, 128, 128);

bool SizeFromBitmap(const HBITMAP hbm, int& dx, int& dy);
bool SizeFromImage(Gdiplus::Image &img, long& dx, long& dy);
LONG GetBitmapRes(const HBITMAP hbm);
bool DrawMaskWithFilter(Gdiplus::Image& imgSrc, Gdiplus::Graphics& drawer, Gdiplus::Color colormask, Gdiplus::Color colordest,
						const LONG xSrc, const LONG ySrc, const LONG dx, const LONG dy, const LONG xDst, const LONG yDst);
bool DrawWithFilter(HBITMAP hbmSrc, HDC hdcDst, COLORREF colormask, COLORREF colorDst, const LONG dx, const LONG dy);
bool DrawWithFilter(Gdiplus::Image& imgSrc, HDC hdcDst, COLORREF colormask, COLORREF colorDst, const LONG dx, const LONG dy);
bool DrawWithFilter(Gdiplus::Image& imgSrc, HDC hdcDst, COLORREF colormask, COLORREF colorDst, const LONG xDst, const LONG yDst, const LONG dx, const LONG dy);
bool DrawWithTransparent(Gdiplus::Image& imgSrc, HDC hdcDst, COLORREF colormask, const LONG dx, const LONG dy);
bool MaskToTransparent(Gdiplus::Image& imgSrc, Gdiplus::Graphics& drawer, COLORREF colormask,
						const LONG xSrc, const LONG ySrc, const LONG dx, const LONG dy, const LONG xDst, const LONG yDst);
bool MaskToTransparent(HBITMAP hbmSrc, HDC hdcDst, COLORREF colormask, const LONG dx, const LONG dy);
bool SaveImage(const wchar_t pathname[], Gdiplus::Bitmap& img);
bool SaveImage(const wchar_t pathname[], HBITMAP hbmSrc);
bool SaveWithTransparent(const wchar_t pathname[], HBITMAP hbmSrc, COLORREF colormask, const LONG dx, const LONG dy);
bool DrawImageToCanvas(Gdiplus::Image& imgSrc, Gdiplus::Bitmap& bmDst, const LONG dx, const LONG dy, const LONG dxDst, const LONG dyDst);
bool DrawImageToCanvas(Gdiplus::Image& imgSrc, HDC hdcDst, const LONG dx, const LONG dy, const LONG dxDst, const LONG dyDst);
UINT GetEncoderClsid(const WCHAR* format, CLSID& clsid);

class CGDIInit
{
public:
	CGDIInit();
	virtual ~CGDIInit();

private:
	Gdiplus::GdiplusStartupInput	m_startupInput;
	ULONG_PTR						m_token;
};

class CDCScreen
{
public:
	CDCScreen(void);
	virtual ~CDCScreen(void);

	operator HDC() const { return m_hdc; }

private:
	CDCScreen(const CDCScreen &);
	HDC m_hdc;
};


class CDCMem
{
public:
	CDCMem(void);
	virtual ~CDCMem(void);

	operator HDC() const { return m_hdc; }

	void FillWith(COLORREF color, LONG dx, LONG dy);
	void FillTransparent(const LONG dx, const LONG dy);

private:
	CDCMem(const CDCMem &);
	HDC m_hdc;
};

class CBitmapMem
{
public:
	CBitmapMem(const int dx, const int dy);
	CBitmapMem(unsigned int imgSize = 32);
	CBitmapMem(const wchar_t *imgSrc);
	virtual ~CBitmapMem();

	operator HBITMAP() const { return m_hbm; }
	bool operator ()() const { return nullptr != m_hbm; }

private:
	CBitmapMem(const CBitmapMem &);
	HBITMAP m_hbm;
};

class CBmSelector
{
public:
	CBmSelector(HDC hdc, HBITMAP hbm);
	CBmSelector(HDC hdc, Gdiplus::Bitmap &img);
	virtual ~CBmSelector();

	bool operator()() const { return NULL != m_hbmSav; }

private:
	CBmSelector(const CBmSelector &);
	HGDIOBJ m_hbmSav;
	const HDC m_hdc;
};

class CSolidBrushSelector
{
public:
	CSolidBrushSelector(COLORREF color, HDC hdc);
	virtual ~CSolidBrushSelector();

	operator HBRUSH() const			{ return m_hbr; }
	bool operator ()() const		{ return NULL != m_hbr; }

private:
	const HDC m_hdc;
	HBRUSH m_hbr;
	HGDIOBJ m_hbrSav;
};
