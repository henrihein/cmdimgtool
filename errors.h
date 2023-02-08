#pragma once

enum Cit_Error
{
	cit_None = 0,
	cit_Fatal = 8999,
	cit_UserError = 1,
	cit_InputError = 2,
	cit_MissingArguments = 3,
	cit_MissingFile = 4,
	cit_FileTooBig = 5,
	cit_ImageDecoreError = 6,
	cit_ImageWebpDataError = 7,
	cit_IOError = 101,
	cit_FileOpenError = 102,
	cit_MemoryError = 103,
	cit_NoImage = 104,
	cit_gdiError = 200,
	cit_gdiNoType = 201,
	cit_gdiInvalidData = 202,
	cit_gdiWin32 = 203,
	cit_gdiAborted = 204,
	cit_Internal = 1001,
	cit_InternalImage = 1002
};
