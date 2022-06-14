#include "stdafx.h"

struct ExpectedArgsValue
{
private:
	const unsigned m_expectedArgs;

protected:
	static const unsigned EAV_UNKNOWN = -1;
	static const unsigned EAV_NONE = 0x0000000;
	static const unsigned EAV_XY = 0x0000001;
	static const unsigned EAV_DXDY = 0x0000002;
	static const unsigned EAV_COLOR1 = 0x0000004;
	static const unsigned EAV_COLOR2 = 0x0000008;
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
	bool XYExpected() const
	{
		return 0 != (EAV_XY & m_expectedArgs);
	}
	bool DxDyExpected() const
	{
		return 0 != (EAV_DXDY & m_expectedArgs);
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
struct DxDyExpected : ExpectedArgsValue
{
public:
	DxDyExpected() : ExpectedArgsValue(EAV_DXDY)
	{}
};
struct ColorColorDxDyExpected : ExpectedArgsValue
{
public:
	ColorColorDxDyExpected() : ExpectedArgsValue(EAV_COLORCOLORDXDY)
	{}
};
