#pragma once

class CDataPtr
{
public:
	CDataPtr() : m_data(nullptr), m_error(0)
	{}
	CDataPtr(BYTE* data) : m_error(0)
	{
		m_data = data;
	}
	virtual ~CDataPtr()
	{
		if (nullptr != m_data)
			VirtualFree((LPVOID)m_data, 0, MEM_RELEASE);
	}
	operator BYTE* ()
	{
		return m_data;
	}
	BYTE* Offset(DWORD ix)
	{
		return m_data + ix;
	}
	bool operator()() const
	{
		return nullptr != m_data;
	}
	BYTE* Data()
	{
		return m_data;
	}
	CDataPtr& operator <<(CDataPtr& rhs)
	{
		m_data = rhs.m_data;
		rhs.m_data = nullptr;
		return *this;
	}
protected:
	CDataPtr(const CDataPtr&) {}
	BYTE* Allocate(ULONG size);

private:
	BYTE* m_data;
	DWORD m_error;
};
class CBmData : public CDataPtr
{
public:
	//Raw data, just an array of bytes of ulSize
	CBmData(ULONG ulSize) : CDataPtr()
	{
		m_size = ulSize;
		Allocate(m_size);
	}
	//Bitmap specific data. Copy pixel data by scanline.
	CBmData(const BYTE* raw, ULONG dx, ULONG dy, ULONG bytesPerPixel, ULONG stride);
	virtual ~CBmData()
	{
	}
	ULONG Size() const
	{
		return m_size;
	}
protected:
private:
	ULONG m_size;
};

class CImageLoader
{
public:
	CImageLoader(const wchar_t *imgSrc);
	virtual ~CImageLoader();

	bool Load(const wchar_t* imgSrc);
	bool Loaded();

	Gdiplus::Image* ImagePtr() { return m_gdiImage; }
	Gdiplus::Image& Image()
	{
		if (nullptr == m_gdiImage)
			throw std::exception("No image loaded");
		return *m_gdiImage;
	}
protected:
	CImageLoader();
	class CFileHandle
	{
	public:
		CFileHandle(HANDLE fh) { m_fh = fh; }
		virtual ~CFileHandle()
		{
			if (INVALID_HANDLE_VALUE != m_fh)
				CloseHandle(m_fh);
		}
		operator HANDLE() const { return m_fh; }
		bool operator()() const { return INVALID_HANDLE_VALUE != m_fh; }
	private:
		HANDLE m_fh;
	};
	int GetStride(Gdiplus::PixelFormat pf, int dx);
	int GetBPP(Gdiplus::PixelFormat pf);
	CBmData* LoadFileContents(const wchar_t* imgSrc);
	bool LoadWebp(const wchar_t* imgSrc);
	bool LoadWebpData(CBmData* data);

private:
	Gdiplus::Image *m_gdiImage;
	CDataPtr m_imageData;
	DWORD m_error;
};

class CEmptyImage : public CImageLoader
{
public:
	CEmptyImage() : CImageLoader()
	{
	}
};