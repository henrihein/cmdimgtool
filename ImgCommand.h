#include "stdafx.h"
#include "ExpectedArgs.h"

int OnUsage(const wchar_t* trigger = NULL);

enum class CIT_COMMAND {
	unknown = -1,
	none = 0,
	help,
	info,
	convert,
	canvas,
	resize,
	extract,
	filter,
	overlay,
	rotate,
	fliphorz,
	flipvert
};

struct CmdExpectedParameters
{
	const CIT_COMMAND m_cmd;
	const ExpectedArgsValue m_expectedArgs;

	CmdExpectedParameters(CIT_COMMAND cmd, const ExpectedArgsValue &expArgs) : m_expectedArgs(expArgs), m_cmd(cmd)
	{
	}
};

struct CmdToolCommand
{
protected:
	static ExpectedArgsValue m_defExpectedArgs;
	const ExpectedArgsValue* m_actualExpectedArgs;
	bool ParseOption(const wchar_t* argOption);
	const ExpectedArgsValue *GetExpectedArguments();
	int ParseCommandLineArguments(int argc, wchar_t* argv[]);

public:
	CIT_COMMAND m_cit;
	COLORREF m_color;
	COLORREF m_targetColor;
	bool m_color1, m_color2, m_transparent;
	wchar_t m_wszSrcFilename[MAX_PATH];
	wchar_t m_wszDstFilename[MAX_PATH];
	int x, y, dxSrc, dySrc, dxDst, dyDst;
	bool m_quiet, m_verbose, m_wrongArgs;

	CmdToolCommand()
	{
		m_cit = CIT_COMMAND::unknown;
		m_actualExpectedArgs = nullptr;
		m_color = RGB(0xFF, 0xFF, 0xFF);
		m_color1 = m_color2 = m_transparent = false;
		m_quiet = false;
		m_wrongArgs = true;
		memset(m_wszSrcFilename, 0, _countof(m_wszSrcFilename));
		memset(m_wszDstFilename, 0, _countof(m_wszDstFilename));
		x = y = dxSrc = dySrc = dxDst = dyDst = 0;
		m_verbose = false;
	}
	bool Initialize();
	bool Initialize(const Gdiplus::Image *srcImage);
	bool CheckExpectedArgs();
	bool NeedSourceFile() const;
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

