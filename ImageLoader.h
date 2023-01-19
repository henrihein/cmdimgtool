#pragma once
class CImageLoader
{
public:
	CImageLoader();
	CImageLoader(const wchar_t *imgSrc);
	virtual ~CImageLoader();

	bool Load(const wchar_t* imgSrc);
	bool Loaded();

	Gdiplus::Image* Image() { return m_gdiImage; }

private:
	Gdiplus::Image *m_gdiImage;
};

