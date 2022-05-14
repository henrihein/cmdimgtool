// ghmonstertype.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GdiHelpers.h"
#include "ImgCommand.h"
#include "resource.h"


size_t Log(const wchar_t message[], va_list args)
{
	wchar_t wszOut[120];

	if (S_OK == StringCbVPrintfW(wszOut, _countof(wszOut), message, args))
		return wprintf(wszOut);
	return 0;
}
void LogError(const wchar_t message[], ...)
{
	Log(message, (va_list)(&message + 1));
}
void GetImagePath(wchar_t* pathname, const wchar_t *tag)
{
	wchar_t* ppath = NULL;

	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &ppath)))
	{
		StringCbCopyW(pathname, MAX_PATH, ppath);
		CoTaskMemFree(ppath);
	}
	else
		GetCurrentDirectoryW(MAX_PATH, pathname);
	StringCbCatW(pathname, MAX_PATH, L"\\");
	StringCbCatW(pathname, MAX_PATH, tag);
	StringCbCatW(pathname, MAX_PATH, L".png");
}
int LogImage(const wchar_t* tag, HBITMAP hbm)
{
	wchar_t pathname[MAX_PATH];

	GetImagePath(pathname, tag);
	if (SaveImage(pathname, hbm))
		return 0;
	return 213;
}
int LogBitmap(const wchar_t* tag, Gdiplus::Bitmap& bm)
{
	HBITMAP hbm = nullptr;

	bm.GetHBITMAP(Gdiplus::Color(RGB(0xFF, 0xFF, 0xFF)), &hbm);
	if (hbm)
		return LogImage(tag, hbm);
	return 31;
}
bool LogImage(const wchar_t* tag, Gdiplus::Image& img)
{
	wchar_t pathname[MAX_PATH];
	CLSID pngClsid;

	GetImagePath(pathname, tag);
	if (UINT_MAX == GetEncoderClsid(L"image/png", pngClsid))
		return false;
	return Gdiplus::Ok == img.Save(pathname, &pngClsid, NULL);
}

int SaveImageTo(const wchar_t *filename, HBITMAP hbm)
{
	if (SaveImage(filename, hbm))
		return 0;
	_putws(filename);
	_putws(L"  -> Saving to file failed.\r\n");
	return 11;
}
int SaveImageTo(const wchar_t* filename, Gdiplus::Bitmap &img)
{
	if (SaveImage(filename, img))
		return 0;
	_putws(filename);
	_putws(L"  -> Saving to file failed.\r\n");
	return 11;
}

//Blt a piece
bool BltPiece(HDC hdcDst, LONG xDst, LONG yDst, LONG dxDst, LONG dyDst, HDC hdcSrc, LONG xSrc, LONG ySrc, LONG dxSrc, LONG dySrc)
{
	HBRUSH mtBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

	return (StretchBlt(hdcDst, xDst, yDst, dxDst, dyDst, hdcSrc, xSrc, ySrc, dxSrc, dySrc, SRCCOPY))
			? true : false;
}


int ExtractImageFrom(CmdToolCommand& cmd, Gdiplus::Image* imgSrc)
{
	int retOp = 2;
	CDCMem hdcMem, hdcNew, hdcFinal;
	CBitmapMem hbmNew(cmd.dxDst, cmd.dyDst);
	bool save = false;

	if (hdcMem && hbmNew)
	{
		CBmSelector bmSelNew(hdcNew, hbmNew);

		if (bmSelNew())
		{
			if (cmd.m_color1)
				hdcMem.FillWith(cmd.m_color, cmd.dxDst, cmd.dyDst);
			save = DrawImageToCanvas(*imgSrc, hdcNew, cmd.dxSrc, cmd.dySrc, cmd.dxDst, cmd.dyDst);
		}
		else
			wprintf(L"Could not create new image.\n");
	}
	if (save)
		retOp = SaveImageTo(cmd.m_wszDstFilename, hbmNew);
	if (0 == retOp)
		wprintf(L"Created %s\n", cmd.m_wszDstFilename);
	else
		wprintf(L"Failed to extract image.\n");
	return retOp;
}
int FilterImageFrom(CmdToolCommand& cmd, Gdiplus::Image* imgSrc)
{
	int retOp = 2;
	CDCMem hdcMem, hdcNew, hdcFinal;
	CBitmapMem hbmNew(cmd.dxDst, cmd.dyDst);
	bool save = false;

	if (hdcMem && hbmNew)
	{
		CBmSelector bmSelNew(hdcNew, hbmNew);

		if (bmSelNew())
		{
			hdcMem.FillWith(cmd.m_color, cmd.dxDst, cmd.dyDst);
			save = DrawWithFilter(*imgSrc, hdcNew, cmd.m_color, cmd.m_targetColor, cmd.dxDst, cmd.dyDst);
		}
		else
			wprintf(L"Could not create new image.\n");
	}
	if (save)
		retOp = SaveImageTo(cmd.m_wszDstFilename, hbmNew);
	if (0 == retOp)
		wprintf(L"Created %s\n", cmd.m_wszDstFilename);
	else
		wprintf(L"Failed to filter image.\n");
	return retOp;
}
int OverlayImageTo(CmdToolCommand& cmd, CBitmapMem& imgDst, Gdiplus::Image& imgSrc)
{
	int retOp = 2;
	CDCMem hdcDst;
	bool save = false;
	CBmSelector hdcDstSel(hdcDst, imgDst);

	save = DrawWithFilter(imgSrc, hdcDst, cmd.m_color, cmd.m_targetColor, cmd.x, cmd.y, cmd.dxDst, cmd.dyDst);
	if (save)
		retOp = SaveImageTo(cmd.m_wszDstFilename, imgDst);
	if (0 == retOp)
		wprintf(L"Created %s\n", cmd.m_wszDstFilename);
	else
		wprintf(L"Failed to overlay image.\n");
	return retOp;
}
int ExtractImage(CmdToolCommand &cmd)
{
	Gdiplus::Image* imgSrc = Gdiplus::Image::FromFile(cmd.m_wszSrcFilename, false);
	int xRetOp = 0;

	LogImage(L"extractimg", *imgSrc);
	cmd.Show(L"Extracting image");
	xRetOp = ExtractImageFrom(cmd, imgSrc);
	delete imgSrc;
	return xRetOp;
}
int ResizeImage(CmdToolCommand& cmd)
{
	Gdiplus::Image* imgSrc = Gdiplus::Image::FromFile(cmd.m_wszSrcFilename, false);
	int xRetOp = 0;

	LogImage(L"resizeimg", *imgSrc);
	cmd.Show(L"Resizing image");
	xRetOp = ExtractImageFrom(cmd, imgSrc);
	delete imgSrc;
	return xRetOp;
}
int FilterImage(CmdToolCommand& cmd)
{
	Gdiplus::Image* imgSrc = Gdiplus::Image::FromFile(cmd.m_wszSrcFilename, false);
	int xRetOp = 0;

	LogImage(L"filterimg", *imgSrc);
	cmd.Show(L"Filtering image");
	xRetOp = FilterImageFrom(cmd, imgSrc);
	delete imgSrc;
	return xRetOp;
}
int OverlayImage(CmdToolCommand& cmd)
{
	int oRetOp = 0;
	//Here we want to load the base image from dest:
	CBitmapMem imgDst(cmd.m_wszDstFilename);
	Gdiplus::Image* imgSrc = Gdiplus::Image::FromFile(cmd.m_wszSrcFilename, false);

	LogImage(L"overlayimgsrc", *imgSrc);
	cmd.Show(L"Overlaying image");
	oRetOp = OverlayImageTo(cmd, imgDst, *imgSrc);
	delete imgSrc;
	return oRetOp;
}
int RotateFlipImage(CmdToolCommand& cmd)
{
	Gdiplus::Image* imgSrc = Gdiplus::Image::FromFile(cmd.m_wszSrcFilename, false);
	int xRetOp = 0;

	LogImage(L"rotateflipimg", *imgSrc);
	switch (cmd.m_cit)
	{
	case CIT_COMMAND::rotate:
		cmd.Show(L"Rotating image");
		imgSrc->RotateFlip(Gdiplus::RotateFlipType::Rotate90FlipNone);
		break;
	case CIT_COMMAND::flipvert:
		cmd.Show(L"Flipping image vertically");
		break;
	case CIT_COMMAND::fliphorz:
		cmd.Show(L"Flipping image horizontally");
		break;
	default:
		break;
	}
	xRetOp = SaveImage(cmd.m_wszDstFilename, *imgSrc);
	delete imgSrc;
	return xRetOp;
}
int DoImageCommand(CmdToolCommand& cmd)
{
	CGDIInit gdi;

	if (!cmd.Initialize())
	{
		wprintf(L"Could not initialize with %s\n", cmd.m_wszSrcFilename);
		return 2;
	}
	if (!cmd.CheckExpectedArgs())
		return 3;
	switch (cmd.m_cit)
	{
	case CIT_COMMAND::extract:
		return ExtractImage(cmd);
	case CIT_COMMAND::resize:
		return ResizeImage(cmd);
	case CIT_COMMAND::filter:
		return FilterImage(cmd);
	case CIT_COMMAND::overlay:
		return OverlayImage(cmd);
	case CIT_COMMAND::rotate:
	case CIT_COMMAND::flipvert:
	case CIT_COMMAND::fliphorz:
		return RotateFlipImage(cmd);
	case CIT_COMMAND::help:
		return 0;
	}
	wprintf(L"Unknown command: %u\n", cmd.m_cit);
	return 1;
}
int wmain(int argc, wchar_t* argv[])
{
	CmdToolCommand cmd;
	int parseRes = cmd.ParseCommandLine(argc, argv);

	if (0 != parseRes)
		return parseRes;
	if (CIT_COMMAND::none == cmd.m_cit)
		return 0;
	if (CIT_COMMAND::unknown == cmd.m_cit)
	{
		_putws(L"Could not parse command line.");
		return 1;
	}
	return DoImageCommand(cmd);
}

