#include "stdafx.h"
#include "ImageLoader.h"
#include "errors.h"
#include "webp/decode.h"

CBmData::CBmData(const BYTE* raw, ULONG dx, ULONG dy, ULONG bytesPerPixel, ULONG stride) : CDataPtr()
{
	const ULONG rawBytesPerRow = bytesPerPixel * dx;

	m_size = stride * dy;
	Allocate(m_size);
	if (nullptr != Data())
	for (ULONG ixScanline = 0; ixScanline < dy; ixScanline++)
	{
		memcpy(Data() + ixScanline * stride, raw + ixScanline * rawBytesPerRow, rawBytesPerRow);
		//Set the tailing bytes to 0
		if (stride > rawBytesPerRow)
			memset(Data() + ixScanline * stride + rawBytesPerRow, 0, stride - rawBytesPerRow);
	}
}
BYTE* CDataPtr::Allocate(ULONG size)
{
	if (LPVOID data = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE))
		m_data = (BYTE*)VirtualAlloc(data, size, MEM_COMMIT, PAGE_READWRITE);
	else
		m_error = GetLastError();
	return m_data;
}


CImageLoader::CImageLoader() : m_imageData(nullptr)
{
	m_error = cit_NoImage;
	m_gdiImage = nullptr;
}
CImageLoader::CImageLoader(const wchar_t* imgSrc) : m_imageData(nullptr)
{
	m_gdiImage = nullptr;
	Load(imgSrc);
}
CImageLoader::~CImageLoader()
{
	if (nullptr != m_gdiImage)
		delete m_gdiImage;
}
bool CImageLoader::Load(const wchar_t* imgSrc)
{
	if ((nullptr != imgSrc) && imgSrc[0])
	{
		m_gdiImage = new Gdiplus::Image(imgSrc);
		m_error = 0;

		if (nullptr == m_gdiImage)
			return LoadWebp(imgSrc);
		if (Gdiplus::Status::Ok != m_gdiImage->GetLastStatus())
		{
			delete m_gdiImage;
			m_gdiImage = nullptr;
			//Try loading as webp
			return LoadWebp(imgSrc);
		}
	}
	m_error = cit_MissingFile;
	return Loaded();
}

bool CImageLoader::Loaded()
{
	if (nullptr == m_gdiImage)
		return false;
	return Gdiplus::Status::Ok == m_gdiImage->GetLastStatus();
}
Gdiplus::PixelFormat GetPixelFormat(const WebPBitstreamFeatures& features)
{
	if (features.has_alpha)
		return PixelFormat32bppARGB;
	return PixelFormat24bppRGB;
}
int CImageLoader::GetStride(Gdiplus::PixelFormat pf, int dx)
{
	int stride = 3 * dx;

	switch (pf)
	{
	case PixelFormat32bppARGB:
		stride = 4 * dx;
		break;
	case PixelFormat24bppRGB:
		break;
	default:
		m_error = cit_InternalImage;
		break;
	}
	//Must be multiplier of 4
	return ((3 + stride) >> 2) << 2;
}
int CImageLoader::GetBPP(Gdiplus::PixelFormat pf)
{
	switch (pf)
	{
	case PixelFormat32bppARGB:
		return 4;
	case PixelFormat24bppRGB:
		return 3;
	default:
		break;
	}
	return 3;
}
Gdiplus::Image* MakeTestImage()
{
	const int dx = 24, dy = 12;
	BYTE imgData[3 * dx * dy];

	for (int ix = 0; ix < 3 * dx * dy; ix++)
	{
		if (0 == (ix % 3))
			imgData[ix] = 0x1f;
		else
			imgData[ix] = 0;
	}
	return new Gdiplus::Bitmap(dx, dy, 3 * dx, PixelFormat24bppRGB, imgData);
}
bool CImageLoader::LoadWebpData(CBmData* data)
{
	int dx = 0, dy = 0;

	if (WebPGetInfo(*data, data->Size(), &dx, &dy))
	{
		WebPBitstreamFeatures features;

		memset(&features, 0, sizeof(features));
		if (VP8_STATUS_OK == WebPGetFeatures(*data, data->Size(), &features))
		{
			Gdiplus::PixelFormat pf = GetPixelFormat(features);
			BYTE* imageDataRaw = nullptr;

			if (features.has_alpha)
				//TODO: need to reverse R and B
				imageDataRaw = WebPDecodeARGB(data->Data(), data->Size(), &dx, &dy);
			else
				imageDataRaw = WebPDecodeBGR(data->Data(), data->Size(), &dx, &dy);
			if (imageDataRaw)
			{
				ULONG stride = GetStride(pf, dx);
				CBmData imageDataUse(imageDataRaw, dx, dy, GetBPP(pf), stride);

				m_gdiImage = new Gdiplus::Bitmap(dx, dy, stride, pf, imageDataUse);
				m_imageData << imageDataUse;
				WebPFree(imageDataRaw);
				return true;
			}
			else
				m_error = cit_ImageWebpDataError;
		}
		else
			m_error = cit_ImageDecoreError;
	}
	return false;
}
CBmData* CImageLoader::LoadFileContents(const wchar_t* imgSrc)
{
	CImageLoader::CFileHandle fh(CreateFileW(imgSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL));

	if (fh())
	{
		LARGE_INTEGER fileSize = { 0 };

		if (GetFileSizeEx(fh, &fileSize))
		{
			if (UINT_MAX > fileSize.QuadPart)
			{
				CBmData* data = new CBmData(fileSize.LowPart);

				if ((nullptr != data) && (*data)())
				{
					CBmData buf(1024 * 64);
					DWORD cbRed = 0, cbTotal = 0;

					if (buf())
					{
						while (ReadFile(fh, buf, buf.Size(), &cbRed, NULL) && (0 < cbRed))
						{
							memcpy(data->Offset(cbTotal), buf, cbRed);
							cbTotal += cbRed;
						}
						if (fileSize.LowPart == cbTotal)
							return data;
					}
					else
						m_error = cit_MemoryError;
					delete data;
				}
				else
					m_error = cit_MemoryError;
			}
			else
				m_error = cit_FileTooBig;
		}
		else
			m_error = cit_IOError;
	}
	else
		m_error = cit_FileOpenError;
	return nullptr;
}
bool CImageLoader::LoadWebp(const wchar_t* imgSrc)
{
	CBmData *data = LoadFileContents(imgSrc);

	if (data)
	{
		bool bRet = LoadWebpData(data);

		delete data;
		return bRet;
	}
	return false;
}

