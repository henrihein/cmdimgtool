#include "stdafx.h"

int OnUsage(const wchar_t* trigger = NULL);

enum CIT_COMMAND {
	CIT_UNKNOWN = -1,
	CIT_NONE = 0,
	CIT_HELP,
	CIT_RESIZE,
	CIT_EXTRACT,
	CIT_FILTER,
	CIT_OVERLAY
};
enum CIT_ARGCOUNT {
	CAC_UNKNOWN = -1,
	CAC_XY					= 0x0000001,
	CAC_DXDY				= 0x0000002,
	CAC_COLOR1				= 0x0000004,
	CAC_COLOR2				= 0x0000008,
	CAC_XYDXDY				= CAC_XY | CAC_DXDY,
	CAC_COLORCOLORDXDY		= CAC_DXDY | CAC_COLOR1 | CAC_COLOR2,
	CAC_ALL
};
struct CmdExpectedParameterCount
{
	CIT_COMMAND m_cmd;
	CIT_ARGCOUNT m_argCount;
	CmdExpectedParameterCount(CIT_COMMAND cmd, CIT_ARGCOUNT argCount)
	{
		m_cmd = cmd;
		m_argCount = argCount;
	}
};

struct CmdToolCommand
{
private:
	bool ParseOption(const wchar_t* argOption);
	void GetExpectedArgCount();

public:
	CIT_COMMAND m_cit;
	COLORREF m_color;
	COLORREF m_targetColor;
	bool m_color1, m_color2, m_transparent;
	wchar_t m_wszSrcFilename[MAX_PATH];
	wchar_t m_wszDstFilename[MAX_PATH];
	CIT_ARGCOUNT m_expectedArgs;
	int x, y, dxSrc, dySrc, dxDst, dyDst;
	bool m_quiet, m_verbose;

	CmdToolCommand()
	{
		m_cit = CIT_UNKNOWN;
		m_color = RGB(0xFF, 0xFF, 0xFF);
		m_color1 = m_color2 = m_transparent = false;
		m_quiet = false;
		memset(m_wszSrcFilename, 0, _countof(m_wszSrcFilename));
		memset(m_wszDstFilename, 0, _countof(m_wszDstFilename));
		m_expectedArgs = CAC_ALL;
		x = y = dxSrc = dySrc = dxDst = dyDst = 0;
		m_verbose = false;
	}
	bool Initialize();
	bool CheckExpectedArgs();
	COLORREF ColorFromArg(const wchar_t* arg);
	int ParseCommandLine(int argc, wchar_t* argv[]);
	void EmitColor(COLORREF color);
	void EmitColor1();
	void EmitColor2();
	void EmitCoordinates()
	{
		if (!m_quiet)
		{
			wprintf(L" X:%03d Y:%03d DX:%03d DY:%03d DX2:%03d DY2:%03d\n",
				x, y, dxSrc, dySrc, dxDst, dyDst);
		}
	}
	void Show(const wchar_t* text)
	{
		wprintf(L"Command: %s\n", text);
		EmitColor1();
		EmitColor2();
		EmitCoordinates();
	}
};
