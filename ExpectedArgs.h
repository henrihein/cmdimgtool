#include "stdafx.h"

struct ExpectedArgsValue
{
private:
	const unsigned m_expectedArgs;

protected:
	static const unsigned EAV_UNKNOWN = (unsigned)-1;
	static const unsigned EAV_NONE = 0x0000000;
	static const unsigned EAV_XY = 0x0000001;
	static const unsigned EAV_DXDY = 0x0000002;
	static const unsigned EAV_COLOR1 = 0x0000004;
	static const unsigned EAV_COLOR2 = 0x0000008;
	static const unsigned EAV_SOURCE = 0x0000100;
	static const unsigned EAV_TARGET = 0x0000200;
	static const unsigned EAV_XYDXDY = EAV_XY | EAV_DXDY;
	static const unsigned EAV_COLORCOLORDXDY = EAV_DXDY | EAV_COLOR1 | EAV_COLOR2;
	static const unsigned EAV_ALL = 0x7FFFFFF;

	ExpectedArgsValue(unsigned value) : m_expectedArgs(value)
	{
	}

public:
	ExpectedArgsValue() : m_expectedArgs(EAV_NONE)
	{}
	ExpectedArgsValue(const ExpectedArgsValue& rhs) : m_expectedArgs(rhs.m_expectedArgs)
	{}

	operator unsigned()
	{
		return m_expectedArgs;
	}
	bool IsNone() const
	{
		return EAV_NONE == m_expectedArgs;
	}
	bool SourceExpected() const
	{
		return 0 != (EAV_SOURCE & m_expectedArgs);
	}
	bool TargetExpected() const
	{
		return 0 != (EAV_TARGET & m_expectedArgs);
	}
	bool XYExpected() const
	{
		return 0 != (EAV_XY & m_expectedArgs);
	}
	bool DxDyExpected() const
	{
		return 0 != (EAV_DXDY & m_expectedArgs);
	}
	bool CoordinatesExpected() const
	{
		return XYExpected() || DxDyExpected();
	}
	bool Color1Expected() const
	{
		return 0 != (EAV_COLOR1 & m_expectedArgs);
	}
	bool Color2Expected() const
	{
		return 0 != (EAV_COLOR2 & m_expectedArgs);
	}
};

struct XYDxDyExpected : ExpectedArgsValue
{
public:
	XYDxDyExpected() : ExpectedArgsValue(EAV_XYDXDY)
	{}
};
struct TargetXYExpected : ExpectedArgsValue
{
public:
	TargetXYExpected() : ExpectedArgsValue(EAV_XY | EAV_TARGET)
	{}
};
struct SrcExpected : ExpectedArgsValue
{
public:
	SrcExpected() : ExpectedArgsValue(EAV_SOURCE)
	{}
};
struct SrcTargetExpected : ExpectedArgsValue
{
public:
	SrcTargetExpected() : ExpectedArgsValue(EAV_SOURCE | EAV_TARGET)
	{}
};
struct SrcTargetXYDxDyExpected : ExpectedArgsValue
{
public:
	SrcTargetXYDxDyExpected() : ExpectedArgsValue(EAV_SOURCE | EAV_TARGET | EAV_XYDXDY)
	{}
};
struct SrcTargetDxDyExpected : ExpectedArgsValue
{
public:
	SrcTargetDxDyExpected() : ExpectedArgsValue(EAV_SOURCE | EAV_TARGET | EAV_DXDY)
	{}
};
struct SrcTargetColorColorDxDyExpected : ExpectedArgsValue
{
public:
	SrcTargetColorColorDxDyExpected() : ExpectedArgsValue(EAV_SOURCE | EAV_TARGET | EAV_COLORCOLORDXDY)
	{}
};
