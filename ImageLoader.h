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

protected:
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
	class CData
	{
	public:
		CData(ULONG ulSize)
		{
			m_size = ulSize;
			m_error = 0;
			m_data = nullptr;
			if (LPVOID data = VirtualAlloc(nullptr, ulSize, MEM_RESERVE, PAGE_READWRITE))
				m_data = (BYTE*)VirtualAlloc(data, ulSize, MEM_COMMIT, PAGE_READWRITE);
			else
				m_error = GetLastError();
		}
		virtual ~CData()
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
		ULONG Size() const
		{
			return m_size;
		}
	private:
		ULONG m_size;
		BYTE* m_data;
		DWORD m_error;
	};
	int GetStride(Gdiplus::PixelFormat pf, int dx);
	CData* LoadFileContents(const wchar_t* imgSrc);
	bool LoadWebp(const wchar_t* imgSrc);
	bool LoadWebpData(CData* data);

private:
	Gdiplus::Image *m_gdiImage;
	DWORD m_error;
};

