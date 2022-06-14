#include "stdafx.h"
#include "GdiHelpers.h"

bool SizeFromBitmap(const HBITMAP hbm, int &dx, int &dy)
{
	assert(hbm);
	if (hbm)
	{
		BITMAP bmData;

		memset(&bmData, 0, sizeof(bmData));
		if (GetObject(hbm, sizeof(bmData), &bmData))
		{
			dx = bmData.bmWidth;
			dy = bmData.bmHeight;
			return true;
		}
	}
	//Most likely we are trying to load a bum bitmap
	return false;
}
bool SizeFromImage(Gdiplus::Image& img, long& dx, long& dy)
{
	Gdiplus::SizeF ds;

	if (Gdiplus::Status::Ok == img.GetPhysicalDimension(&ds))
	{ 
		dx = (long)ceil(ds.Width);
		dy = (long)ceil(ds.Height);
		return true;
	}
	return false;
}
LONG GetBitmapRes(const HBITMAP hbm)
{
	assert(hbm);
	if (hbm)
	{
		BITMAP bmData;

		memset(&bmData, 0, sizeof(bmData));
		if (GetObject(hbm, sizeof(bmData), &bmData))
			return bmData.bmBitsPixel;
	}
	//Most likely we are trying to load a bum bitmap
	assert(false);
	return 0;
}
bool DrawImageToCanvas(Gdiplus::Image& imgSrc, Gdiplus::Bitmap& bmDst, const LONG dx, const LONG dy, const LONG dxDst, const LONG dyDst)
{
	Gdiplus::Graphics drawer(&bmDst);
	const Gdiplus::REAL imgDX = static_cast<Gdiplus::REAL>(dx);
	const Gdiplus::REAL imgDY = static_cast<Gdiplus::REAL>(dy);
	const Gdiplus::REAL imgDXDst = static_cast<Gdiplus::REAL>(dxDst);
	const Gdiplus::REAL imgDYDst = static_cast<Gdiplus::REAL>(dyDst);
	const Gdiplus::RectF rect(0, 0, imgDXDst, imgDYDst);

	return (Gdiplus::Ok == drawer.DrawImage(&imgSrc, rect, 0, 0, imgDX, imgDY, Gdiplus::UnitPixel));
}
bool DrawImageToCanvas(Gdiplus::Image& imgSrc, HDC hdcDst, const LONG dx, const LONG dy, const LONG dxDst, const LONG dyDst)
{
	Gdiplus::Graphics drawer(hdcDst);
	const Gdiplus::REAL imgDX = static_cast<Gdiplus::REAL>(dx);
	const Gdiplus::REAL imgDY = static_cast<Gdiplus::REAL>(dy);
	const Gdiplus::REAL imgDXDst = static_cast<Gdiplus::REAL>(dxDst);
	const Gdiplus::REAL imgDYDst = static_cast<Gdiplus::REAL>(dyDst);
	const Gdiplus::RectF rect(0, 0, imgDXDst, imgDYDst);

	return (Gdiplus::Ok == drawer.DrawImage(&imgSrc, rect, 0, 0, imgDX, imgDY, Gdiplus::UnitPixel));
}
bool DrawMaskWithFilter(Gdiplus::Image& imgSrc, Gdiplus::Graphics& drawer, Gdiplus::Color colormask, Gdiplus::Color colordest,
						const LONG xSrc, const LONG ySrc, const LONG dx, const LONG dy, const LONG xDst, const LONG yDst)
{
	Gdiplus::ImageAttributes imageAttributes;
	Gdiplus::ColorMap colorMap = { colormask, colordest };
	const Gdiplus::REAL imgRx = static_cast<Gdiplus::REAL>(xSrc);
	const Gdiplus::REAL imgRy = static_cast<Gdiplus::REAL>(ySrc);
	const Gdiplus::REAL imgSizeRdx = static_cast<Gdiplus::REAL>(dx);
	const Gdiplus::REAL imgSizeRdy = static_cast<Gdiplus::REAL>(dy);
	const Gdiplus::REAL imgRxDst = static_cast<Gdiplus::REAL>(xDst);
	const Gdiplus::REAL imgRyDst = static_cast<Gdiplus::REAL>(yDst);
	const Gdiplus::RectF rect(imgRxDst, imgRyDst, imgSizeRdx, imgSizeRdy);

	imageAttributes.SetRemapTable(1, &colorMap, Gdiplus::ColorAdjustTypeBitmap);
	return (Gdiplus::Ok == drawer.DrawImage(&imgSrc, rect, imgRx, imgRy, imgSizeRdx, imgSizeRdy, Gdiplus::UnitPixel, &imageAttributes));
}
bool MaskToTransparent(Gdiplus::Image& imgSrc, Gdiplus::Graphics& drawer, COLORREF colormask, 
	const LONG xSrc, const LONG ySrc, const LONG dx, const LONG dy, const LONG xDst, const LONG yDst)
{
	return DrawMaskWithFilter(imgSrc, drawer, 
								Gdiplus::Color(GetRValue(colormask), GetGValue(colormask), GetBValue(colormask)), 
								Gdiplus::Color(0, 255, 0, 0),
								xSrc, ySrc, dx, dy, xDst, yDst);
}
bool MaskToTransparent(Gdiplus::Image& imgSrc, Gdiplus::Graphics& drawer, COLORREF colormask, const LONG dx, const LONG dy)
{
	return MaskToTransparent(imgSrc, drawer, colormask, 0, 0, dx, dy, 0, 0);
}
bool MaskToTransparent(HBITMAP hbmSrc, HDC hdcDst, COLORREF colormask, const LONG dx, const LONG dy)
{
	Gdiplus::Bitmap bmSrc(hbmSrc, NULL);
	Gdiplus::Graphics drawer(hdcDst);

	return MaskToTransparent(bmSrc, drawer, colormask, dx, dy);
}
bool MaskToTransparent(HBITMAP hbmSrc, Gdiplus::Bitmap &bmDst, COLORREF colormask, const LONG dx, const LONG dy)
{
	Gdiplus::Bitmap bmSrc(hbmSrc, NULL);
	Gdiplus::Graphics drawer(&bmDst);

	return MaskToTransparent(bmSrc, drawer, colormask, dx, dy);
}
bool DrawWithFilter(Gdiplus::Image& imgSrc, HDC hdcDst, COLORREF colormask, COLORREF colorDst, const LONG dx, const LONG dy)
{
	Gdiplus::Graphics drawer(hdcDst);

	return DrawMaskWithFilter(imgSrc, drawer,
		Gdiplus::Color(255, GetRValue(colormask), GetGValue(colormask), GetBValue(colormask)),
		Gdiplus::Color(255, GetRValue(colorDst), GetGValue(colorDst), GetBValue(colorDst)),
		0, 0, dx, dy, 0, 0);
}
bool DrawWithFilter(Gdiplus::Image& imgSrc, HDC hdcDst, COLORREF colormask, COLORREF colorDst, const LONG xDst, const LONG yDst, LONG dx, LONG dy)
{
	Gdiplus::Graphics drawer(hdcDst);

	if ((0 == dx) || (0 == dy)) SizeFromImage(imgSrc, dx, dy);
	return DrawMaskWithFilter(imgSrc, drawer,
		Gdiplus::Color(255, GetRValue(colormask), GetGValue(colormask), GetBValue(colormask)),
		Gdiplus::Color(255, GetRValue(colorDst), GetGValue(colorDst), GetBValue(colorDst)),
		0, 0, dx, dy, xDst, yDst);
}
bool DrawWithTransparent(Gdiplus::Image& imgSrc, HDC hdcDst, COLORREF colormask, const LONG dx, const LONG dy)
{
	Gdiplus::Graphics drawer(hdcDst);

	return DrawMaskWithFilter(imgSrc, drawer,
		Gdiplus::Color(255, GetRValue(colormask), GetGValue(colormask), GetBValue(colormask)),
		Gdiplus::Color(0, 0, 0, 0),
		0, 0, dx, dy, 0, 0);
}
bool DrawWithFilter(HBITMAP hbmSrc, HDC hdcDst, COLORREF colormask, COLORREF colorDst,
	const LONG xSrc, const LONG ySrc, const LONG xDst, const LONG yDst, const LONG dx, const LONG dy)
{
	Gdiplus::Bitmap bmSrc(hbmSrc, NULL);
	Gdiplus::Graphics drawer(hdcDst);

	return DrawMaskWithFilter(bmSrc, drawer,
		Gdiplus::Color(255, GetRValue(colormask), GetGValue(colormask), GetBValue(colormask)),
		Gdiplus::Color(255, GetRValue(colorDst), GetGValue(colorDst), GetBValue(colorDst)),
		xSrc, ySrc, dx, dy, xDst, yDst);
}
bool DrawWithFilter(HBITMAP hbmSrc, HDC hdcDst, COLORREF colormask, COLORREF colorDst, const LONG dx, const LONG dy)
{
	return DrawWithFilter(hbmSrc, hdcDst, colormask, colorDst, 0, 0, 0, 0, dx, dy);
}

const wchar_t* GetImageType(const wchar_t pathname[])
{
	const wchar_t* ztExt = wcsrchr(pathname, '.');

	if (nullptr != ztExt)
	{
		if (0 == wcscmp(ztExt, L".png"))
			return L"image/png";
		if (0 == wcscmp(ztExt, L".gif"))
			return L"image/gif";
		if (0 == wcscmp(ztExt, L".jpg"))
			return L"image/jpeg";
		if (0 == wcscmp(ztExt, L".jpeg"))
			return L"image/jpeg";
		if (0 == wcscmp(ztExt, L".bmp"))
			return L"image/bmp";
	}
	//Default to png
	return L"image/png";
}
bool SaveWithTransparent(const wchar_t pathname[], HBITMAP hbmSrc, COLORREF colormask, const LONG dx, const LONG dy)
{
	Gdiplus::Bitmap bmDst(dx, dy, PixelFormat32bppPARGB);
	CLSID pngClsid;

	MaskToTransparent(hbmSrc, bmDst, colormask, dx, dy);
	if (UINT_MAX == GetEncoderClsid(GetImageType(pathname), pngClsid))
		return false;
	return Gdiplus::Ok == bmDst.Save(pathname, &pngClsid, NULL);
}
bool SaveImage(const wchar_t pathname[], Gdiplus::Image& img)
{
	CLSID pngClsid;

	if (UINT_MAX == GetEncoderClsid(GetImageType(pathname), pngClsid))
		return false;
	return Gdiplus::Ok == img.Save(pathname, &pngClsid, NULL);
}
bool SaveImage(const wchar_t pathname[], HBITMAP hbmSrc)
{
	Gdiplus::Bitmap bmSrc(hbmSrc, nullptr);

	return SaveImage(pathname, bmSrc);
}

CGDIInit::CGDIInit()
{
	GdiplusStartup(&m_token, &m_startupInput, NULL);
}
CGDIInit::~CGDIInit()
{
	Gdiplus::GdiplusShutdown(m_token);
}

CDCScreen::CDCScreen(void)
{
	m_hdc = GetDC(NULL);
}
CDCScreen::~CDCScreen(void)
{
	if (m_hdc) ReleaseDC(NULL, m_hdc);
}

CDCMem::CDCMem(void)
{
	m_hdc = CreateCompatibleDC(NULL);
}
CDCMem::~CDCMem(void)
{
	if (m_hdc) DeleteDC(m_hdc);
}
void CDCMem::FillWith(COLORREF color, LONG dx, LONG dy)
{
	CSolidBrushSelector brush(color, m_hdc);
	const RECT rc = { 0, 0, dx + 1, dy + 1 };

	FillRect(m_hdc, &rc, brush);
}
void CDCMem::FillTransparent(const LONG dx, const LONG dy)
{
	Gdiplus::Graphics plane(m_hdc);
	Gdiplus::SolidBrush transparentbrush(Gdiplus::Color(0, 0, 0, 255));

	plane.FillRectangle(&transparentbrush, 0, 0, dx + 1, dy + 1);
}

CBitmapMem::CBitmapMem(unsigned int imgSize) : m_hbm(nullptr)
{
	CDCScreen hdcScreen;
	BITMAPINFO bmi;
	void *pvBits = NULL;

	memset(&bmi, 0, sizeof(BITMAPINFO));
	if (hdcScreen)	
	{
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = imgSize;
		bmi.bmiHeader.biHeight = imgSize;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = imgSize * imgSize * 4;
		//Need to use CreateDIBSection to get 32 bit
		m_hbm = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	}
}
CBitmapMem::CBitmapMem(const int dx, const int dy) : m_hbm(nullptr)
{
	CDCScreen hdcScreen;
	BITMAPINFO bmi;
	void* pvBits = NULL;

	memset(&bmi, 0, sizeof(BITMAPINFO));
	if (hdcScreen)
	{
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = dx;
		bmi.bmiHeader.biHeight = dy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = dx * dy * 4;
		//Need to use CreateDIBSection to get 32 bit
		m_hbm = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	}
}
CBitmapMem::CBitmapMem(const wchar_t* imgSrc) : m_hbm(nullptr)
{
	Gdiplus::Bitmap bmSrc(imgSrc);

	bmSrc.GetHBITMAP(Gdiplus::Color(), &m_hbm);
}

CBitmapMem::~CBitmapMem()
{
	if (m_hbm)
		DeleteObject(m_hbm);
}

CBmSelector::CBmSelector(HDC hdc, HBITMAP hbm) : m_hbmSav(NULL), m_hdc(hdc)
{
	if (hdc && hbm)
		m_hbmSav = SelectObject(hdc, hbm);
}
CBmSelector::CBmSelector(HDC hdc, Gdiplus::Bitmap& img) : m_hbmSav(NULL), m_hdc(hdc)
{
	HBITMAP hbm = nullptr;
	const Gdiplus::Color bkColor(0xFF, 0xFF, 0xFF, 0xFF);

	if (hdc && img.GetHBITMAP(bkColor, &hbm))
		m_hbmSav = SelectObject(hdc, hbm);
}

CBmSelector::~CBmSelector()
{
	if (m_hdc && m_hbmSav)
		SelectObject(m_hdc, m_hbmSav);
}

CSolidBrushSelector::CSolidBrushSelector(COLORREF color, HDC hdc) : m_hdc(hdc), m_hbrSav(NULL), m_hbr(NULL)
{
	if (m_hdc)
	{
		if (m_hbr = CreateSolidBrush(color))
			m_hbrSav = SelectObject(m_hdc, m_hbr);
	}
}
CSolidBrushSelector::~CSolidBrushSelector()
{
	if (m_hdc && m_hbrSav)
		SelectObject(m_hdc, m_hbrSav);
	if (m_hbr)
		DeleteObject(m_hbr);
}

UINT GetEncoderClsid(const WCHAR* format, CLSID &clsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	UINT ixEncoderFound = UINT_MAX;
	BYTE *data = NULL;
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (0 == size) return UINT_MAX;
	pImageCodecInfo = reinterpret_cast<Gdiplus::ImageCodecInfo*>(data = new BYTE[size]);
	if (NULL == pImageCodecInfo) return UINT_MAX;
	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT ixEncoder = 0; ixEncoder < num; ++ixEncoder)
	{
		if (wcscmp(pImageCodecInfo[ixEncoder].MimeType, format) == 0)
		{
			clsid = pImageCodecInfo[ixEncoder].Clsid;
			ixEncoderFound = ixEncoder;
		}    
	}
	if (data) delete [] data;
	return ixEncoderFound;
}
