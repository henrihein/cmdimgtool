#include "stdafx.h"
#include "ImageLoader.h"

CImageLoader::CImageLoader()
{
	m_gdiImage = nullptr;
}
CImageLoader::CImageLoader(const wchar_t* imgSrc)
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
		m_gdiImage = new Gdiplus::Image(imgSrc);
	return Loaded();
}

bool CImageLoader::Loaded()
{
	if (nullptr == m_gdiImage)
		return false;
	return Gdiplus::Status::Ok == m_gdiImage->GetLastStatus();
}