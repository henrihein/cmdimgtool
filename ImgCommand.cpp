#include "ImgCommand.h"
#include "GdiHelpers.h"

CmdExpectedParameters g_parameterCounts[] = {
	CmdExpectedParameters(CIT_COMMAND::extract, XYDxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::resize, DxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::filter, ColorColorDxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::overlay, XYDxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::rotate, ExpectedArgsValue()),
	CmdExpectedParameters(CIT_COMMAND::fliphorz, ExpectedArgsValue()),
	CmdExpectedParameters(CIT_COMMAND::flipvert, ExpectedArgsValue())
};
ExpectedArgsValue CmdToolCommand::m_defExpectedArgs;

int OnUsage(const wchar_t* trigger)
{
	int ret = 0;

	if (trigger)
	{
		wprintf(L"Confused by: %s\r\n", trigger);
		ret = 1;
	}
	printf("Usage:\r\n\r\n   cmdimgtool command [options] source destination image-parameters\r\n");
	printf("                              image parameters are : x y dx dy dx-target dy-target] \r\n\r\n");
	printf("  Possible commands:\r\n");
	printf("   help\r\n");
	printf("   extract\r\n");
	printf("   resize\r\n");
	printf("   filter\r\n");
	printf("   rotate\r\n");
	printf("   flipvertical\r\n");
	printf("   fliphorizontal\r\n");
	printf("   overlay\r\n");
	return ret;
}

void CmdToolCommand::EmitColor(COLORREF color)
{
	if (!m_quiet)
	{
		if (m_color1)
			wprintf(L"  Color used: R:%02X G:%02X B:%02X A:%02X\n", GetRValue(color), GetGValue(color), GetBValue(color), GetAValue(color));
		else
			puts("  Not using color for this command.");
	}
}
void CmdToolCommand::EmitColor1()
{
	if (!m_quiet)
	{
		if (m_color1)
			EmitColor(m_color);
		else
			puts("  Not using color for this command.");
	}
}
void CmdToolCommand::EmitColor2()
{
	if (!m_quiet)
	{
		if (m_color2)
			EmitColor(m_targetColor);
	}
}

CIT_COMMAND CommandFromArg(const wchar_t* arg)
{
	if (0 == _wcsicmp(arg, L"help")) return CIT_COMMAND::extract;
	if (0 == _wcsicmp(arg, L"extract")) return CIT_COMMAND::extract;
	if (0 == _wcsicmp(arg, L"resize")) return CIT_COMMAND::resize;
	if (0 == _wcsicmp(arg, L"filter")) return CIT_COMMAND::filter;
	if (0 == _wcsicmp(arg, L"overlay")) return CIT_COMMAND::overlay;
	if (0 == _wcsicmp(arg, L"rotate")) return CIT_COMMAND::rotate;
	if (0 == _wcsicmp(arg, L"fliphorz")) return CIT_COMMAND::fliphorz;
	if (0 == _wcsicmp(arg, L"fliphorizontal")) return CIT_COMMAND::fliphorz;
	if (0 == _wcsicmp(arg, L"flipvert")) return CIT_COMMAND::flipvert;
	if (0 == _wcsicmp(arg, L"flipvertical")) return CIT_COMMAND::flipvert;
	return CIT_COMMAND::unknown;
}
#define GetBValueFromArg(arg)      (LOBYTE(arg))
#define GetGValueFromArg(arg)      (LOBYTE(((WORD)(arg)) >> 8))
#define GetRValueFromArg(arg)      (LOBYTE((arg)>>16))
COLORREF CmdToolCommand::ColorFromArg(const wchar_t* arg)
{
	if ('0' == arg[0])
		if ('x' == arg[1])
		{
			const ULONG ulArg = wcstol(arg, NULL, 16);

			return RGB(GetRValueFromArg(ulArg), GetGValueFromArg(ulArg), GetBValueFromArg(ulArg));
		}
	if (0 == _wcsicmp(arg, L"black")) return RGB(0x00, 0x00, 0x00);
	if (0 == _wcsicmp(arg, L"white")) return RGB(0xFF, 0xFF, 0xFF);
	if (0 == _wcsicmp(arg, L"gray")) return RGB(0x7F, 0x7F, 0x7F);
	if (0 == _wcsicmp(arg, L"red")) return RGB(0xFF, 0x00, 0x00);
	if (0 == _wcsicmp(arg, L"green")) return RGB(0x00, 0xFF, 0x00);
	if (0 == _wcsicmp(arg, L"blue")) return RGB(0x00, 0x00, 0xFF);
	if (0 == _wcsicmp(arg, L"black")) return RGB(0x00, 0x00, 0x00);
	if (0 == _wcsicmp(arg, L"transparent"))
	{
		m_transparent = true;
		return RGB(0x00, 0x00, 0x00);
	}
	wprintf(L"Defaulting color %s to black\n", arg);
	return color_black;
}
int NumFromArg(const wchar_t* arg)
{
	if ('0' == arg[0])
		if ('x' == arg[1])
			return wcstol(arg, NULL, 16);
	return wcstol(arg, NULL, 10);
}

bool CmdToolCommand::Initialize()
{
	if ((0 >= dxSrc) || (0 >= dySrc))
	{
		Gdiplus::Image imgSrc(m_wszSrcFilename);
		long dxFromSrc, dyFromSrc;

		if (Gdiplus::Status::Ok != imgSrc.GetLastStatus())
			return false;
		if (SizeFromImage(imgSrc, dxFromSrc, dyFromSrc))
		{
			if (0 >= dxSrc)
				dxSrc = dxFromSrc;
			if (0 >= dySrc)
				dySrc = dyFromSrc;
		}
	}
	if (-1 == dxDst) dxDst = dxSrc;
	if (-1 == dyDst) dyDst = dySrc;
	return true;
}
const ExpectedArgsValue& CmdToolCommand::GetExpectedArguments()
{
	for (int ixCmdArgCount = 0; ixCmdArgCount < _countof(g_parameterCounts); ixCmdArgCount++)
		if (g_parameterCounts[ixCmdArgCount].m_cmd == m_cit)
		{
			return g_parameterCounts[ixCmdArgCount].m_expectedArgs;
		}
	return m_defExpectedArgs;
}
bool CmdToolCommand::ParseOption(const wchar_t* argOption)
{
	static const wchar_t colArg[] = L"/color:";
	static const wchar_t targetColArg[] = L"/targetcolor:";
	static const wchar_t sourceColArg[] = L"/sourcecolor:";
	static const wchar_t verboseArg[] = L"/verbose";
	static const wchar_t quietArg[] = L"/quiet";

	if ((0 == _wcsnicmp(colArg, argOption, _countof(colArg) - 1)) || (0 == _wcsnicmp(sourceColArg, argOption, _countof(sourceColArg) - 1)))
	{
		m_color = ColorFromArg(argOption + _countof(colArg) - 1);
		m_color1 = true;
		return true;
	}
	if (0 == _wcsnicmp(targetColArg, argOption, _countof(targetColArg) - 1))
	{
		m_targetColor = ColorFromArg(argOption + _countof(targetColArg) - 1);
		m_color2 = true;
		return true;
	}
	if (0 == _wcsnicmp(verboseArg, argOption, _countof(verboseArg) - 1))
	{
		m_verbose = true;
		return true;
	}
	if (0 == _wcsnicmp(quietArg, argOption, _countof(quietArg) - 1))
	{
		m_quiet = true;
		return true;
	}
	return false;
}
bool CmdToolCommand::CheckExpectedArgs()
{
	const ExpectedArgsValue& expectedArgs = GetExpectedArguments();

	if (expectedArgs.XYExpected())
		if ((0 > x) || (0 > y))
		{
			_putws(L"Expected: X and Y coordinates.");
			return false;
		}
	if (expectedArgs.DxDyExpected())
		if ((0 >= dxSrc) || (0 >= dySrc))
		{
			_putws(L"Expected: DX and DY dimensions.");
			return false;
		}
	if (expectedArgs.Color1Expected())
		if (!m_color1)
		{
			_putws(L"Expected: color specification for source.");
			return false;
		}
	if (expectedArgs.Color2Expected())
		if (!m_color2)
		{
			_putws(L"Expected: color specification for target.");
			return false;
		}
	return true;
}
int CmdToolCommand::ParseCommandLine(int argc, wchar_t* argv[])
{
	int ixCmd = 2;

	if (2 >= argc)
	{
		m_cit = CIT_COMMAND::none;
		OnUsage();
		return 0;
	}
	if (3 > argc)
		return OnUsage(L"Too few arguments");
	if (CIT_COMMAND::unknown == (m_cit = CommandFromArg(argv[1])))
		return OnUsage(argv[1]);
	if (CIT_COMMAND::help == m_cit)
	{
		OnUsage();
		return 0;
	}
	return ParseCommandLineArguments(argc, argv, ixCmd);
}
int CmdToolCommand::ParseCommandLineArguments(int argc, wchar_t* argv[], int ixCmd)
{
	const ExpectedArgsValue &expectedArgs = GetExpectedArguments();

	while ('/' == argv[ixCmd][0])
	{
		if (!ParseOption(argv[ixCmd]))
			return OnUsage(argv[ixCmd]);
		ixCmd++;
	}
	if (argc <= (1 + ixCmd))
		return OnUsage(L"No source file specified.");
	StringCbCopyW(m_wszSrcFilename, MAX_PATH, argv[ixCmd]);
	if (argc <= (ixCmd + 1))
		return OnUsage(L"No target file specified.");
	StringCbCopyW(m_wszDstFilename, MAX_PATH, argv[ixCmd + 1]);
	if (argc <= (3 + ixCmd))
	{
		if (expectedArgs.IsNone())
			return 0;
		return OnUsage(L"No coordinates.");
	}
	x = NumFromArg(argv[ixCmd + 2]);
	y = NumFromArg(argv[ixCmd + 3]);
	if (argc >= (6 + ixCmd))
	{
		dxSrc = NumFromArg(argv[ixCmd + 4]);
		dySrc = NumFromArg(argv[ixCmd + 5]);
	}
	if (argc >= (8 + ixCmd))
	{
		if (m_verbose)
			wprintf(L"Setting DX and DY from %s.%s\n", argv[ixCmd + 6], argv[ixCmd + 7]);
		dxDst = NumFromArg(argv[ixCmd + 6]);
		dyDst = NumFromArg(argv[ixCmd + 7]);
	}
	else
	{
		dxDst = -1;
		dyDst = -1;
	}
	return 0;
}
