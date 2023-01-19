#include "ImgCommand.h"
#include "GdiHelpers.h"

CmdExpectedParameters g_parameterCounts[] = {
	CmdExpectedParameters(CIT_COMMAND::info, SrcExpected()),
	CmdExpectedParameters(CIT_COMMAND::convert, SrcTargetExpected()),
	CmdExpectedParameters(CIT_COMMAND::canvas, TargetXYExpected()),
	CmdExpectedParameters(CIT_COMMAND::extract, SrcTargetXYDxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::resize, SrcTargetDxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::filter, SrcTargetColorColorDxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::overlay, SrcTargetXYDxDyExpected()),
	CmdExpectedParameters(CIT_COMMAND::rotate, SrcTargetExpected()),
	CmdExpectedParameters(CIT_COMMAND::fliphorz, SrcTargetExpected()),
	CmdExpectedParameters(CIT_COMMAND::flipvert, SrcTargetExpected())
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
	printf("   info\r\n");
	printf("   convert\r\n");
	printf("   canvas\r\n");
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
		wprintf(L"  Color used: R:%02X G:%02X B:%02X A:%02X\n", GetRValue(color), GetGValue(color), GetBValue(color), GetAValue(color));
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
	if (0 == _wcsicmp(arg, L"help")) return CIT_COMMAND::help;
	if (0 == _wcsicmp(arg, L"info")) return CIT_COMMAND::info;
	if (0 == _wcsicmp(arg, L"convert")) return CIT_COMMAND::convert;
	if (0 == _wcsicmp(arg, L"canvas")) return CIT_COMMAND::canvas;
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

bool CmdToolCommand::Initialize(const Gdiplus::Image *srcImage)
{
	if (nullptr == m_actualExpectedArgs)
		return false;
	if ((0 >= dxSrc) || (0 >= dySrc))
	{
		if (m_actualExpectedArgs->SourceExpected())
		{
			if (nullptr == srcImage)
				return false;
			long dxFromSrc, dyFromSrc;

			if (Gdiplus::Status::Ok != srcImage->GetLastStatus())
				return false;
			if (SizeFromImage(*srcImage, dxFromSrc, dyFromSrc))
			{
				if (0 >= dxSrc)
					dxSrc = dxFromSrc;
				if (0 >= dySrc)
					dySrc = dyFromSrc;
			}
		}
	}
	if (-1 == dxDst) dxDst = dxSrc;
	if (-1 == dyDst) dyDst = dySrc;
	return true;
}
const ExpectedArgsValue* CmdToolCommand::GetExpectedArguments()
{
	for (int ixCmdArgCount = 0; ixCmdArgCount < _countof(g_parameterCounts); ixCmdArgCount++)
		if (g_parameterCounts[ixCmdArgCount].m_cmd == m_cit)
		{
			return &g_parameterCounts[ixCmdArgCount].m_expectedArgs;
		}
	return &m_defExpectedArgs;
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
	if (nullptr == m_actualExpectedArgs)
	{
		_putws(L"Internal error parsing command line.");
		return false;
	}
	if (m_actualExpectedArgs->SourceExpected())
		if ('\0' == m_wszSrcFilename[0])
		{
			_putws(L"Expected: source image file.");
			return false;
		}
	if (m_actualExpectedArgs->XYExpected())
		if ((0 > x) || (0 > y))
		{
			_putws(L"Expected: X and Y coordinates.");
			return false;
		}
	if (m_actualExpectedArgs->DxDyExpected())
		if ((0 >= dxSrc) || (0 >= dySrc))
		{
			_putws(L"Expected: DX and DY dimensions.");
			return false;
		}
	if (m_actualExpectedArgs->Color1Expected())
		if (!m_color1)
		{
			_putws(L"Expected: color specification for source.");
			return false;
		}
	if (m_actualExpectedArgs->Color2Expected())
		if (!m_color2)
		{
			_putws(L"Expected: color specification for target.");
			return false;
		}
	return true;
}
bool CmdToolCommand::NeedSourceFile() const
{
	if (nullptr != m_actualExpectedArgs)
		return m_actualExpectedArgs->SourceExpected();
	return false;
}
int CmdToolCommand::ParseCommandLine(int argc, wchar_t* argv[])
{
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
	m_actualExpectedArgs = GetExpectedArguments();
	return ParseCommandLineArguments(argc, argv);
}
int CmdToolCommand::ParseCommandLineArguments(int argc, wchar_t* argv[])
{
	int ixCmd = 2;

	if (nullptr == m_actualExpectedArgs)
	{
		puts("Internal error parsing aruguments.");
		return 1;
	}
	while ('/' == argv[ixCmd][0])
	{
		if (!ParseOption(argv[ixCmd]))
			return OnUsage(argv[ixCmd]);
		ixCmd++;
	}
	if (argc < (2 + ixCmd))
	{
		if (argc < (1 + ixCmd))
		{
			if (m_actualExpectedArgs->SourceExpected() || m_actualExpectedArgs->TargetExpected())
				return OnUsage(L"No source or target file specified.");
			if (m_actualExpectedArgs->SourceExpected())
				StringCbCopyW(m_wszSrcFilename, MAX_PATH, argv[ixCmd++]);
			else
				StringCbCopyW(m_wszDstFilename, MAX_PATH, argv[ixCmd++]);
		}
		else
		{
			if (m_actualExpectedArgs->SourceExpected() && m_actualExpectedArgs->TargetExpected())
				return OnUsage(L"No target file specified.");
			StringCbCopyW(m_wszSrcFilename, MAX_PATH, argv[ixCmd++]);
			if (m_actualExpectedArgs->TargetExpected())
				StringCbCopyW(m_wszDstFilename, MAX_PATH, argv[ixCmd++]);
		}
	}
	else
	{
		wprintf(L"%u args as expected %u\n", argc, ixCmd);
		if (m_actualExpectedArgs->SourceExpected())
			StringCbCopyW(m_wszSrcFilename, MAX_PATH, argv[ixCmd++]);
		if (m_actualExpectedArgs->TargetExpected())
			StringCbCopyW(m_wszDstFilename, MAX_PATH, argv[ixCmd++]);
	}
	if (argc < (2 + ixCmd))
	{
		if (!m_actualExpectedArgs->CoordinatesExpected())
			return 0;
		return OnUsage(L"No coordinates.");
	}
	if (m_actualExpectedArgs->XYExpected())
	{
		x = NumFromArg(argv[ixCmd++]);
		y = NumFromArg(argv[ixCmd++]);
	}
	if (argc >= (2 + ixCmd))
	{
		dxSrc = NumFromArg(argv[ixCmd++]);
		dySrc = NumFromArg(argv[ixCmd++]);
	}
	if (argc >= (2 + ixCmd))
	{
		if (m_verbose)
			wprintf(L"Setting DX and DY from %s.%s\n", argv[ixCmd], argv[ixCmd + 1]);
		dxDst = NumFromArg(argv[ixCmd++]);
		dyDst = NumFromArg(argv[ixCmd++]);
	}
	else
	{
		dxDst = -1;
		dyDst = -1;
	}
	return 0;
}
