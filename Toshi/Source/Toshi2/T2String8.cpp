#include "ToshiPCH.h"
#include "T2String8.h"
#include "T2CharTraits.h"

TOSHI_NAMESPACE_START

TCHAR T2String8::ms_aScratchMem[T2String8::SCRATCH_MEM_SIZE];

TINT T2String8::FormatV(TCHAR* a_pcString, const TCHAR* a_pcFormat, va_list args)
{
	TINT iResult = vsprintf(a_pcString, a_pcFormat, args);
	TASSERT(iResult != -1, "PS2/GC/X360 do not correctly support _vsnprintf, this code will cause memory to be clobbered on those platforms! Increase the size of the destination string to avoid this problem");
	return iResult;
}

TINT T2String8::FormatV(TCHAR* a_pcString, TINT size, const TCHAR* a_pcFormat, va_list args)
{
	TINT iResult = _vsnprintf(a_pcString, size, a_pcFormat, args);
	TASSERT(iResult != -1, "PS2/GC/X360 do not correctly support _vsnprintf, this code will cause memory to be clobbered on those platforms! Increase the size of the destination string to avoid this problem");
	a_pcString[size - 1] = '\0';
	return iResult;
}

TINT T2String8::Format(TCHAR* a_pcString, const TCHAR* a_pcFormat, ...)
{
	va_list args;

	va_start(args, a_pcFormat);
	TINT iResult = FormatV(a_pcString, a_pcFormat, args);
	va_end(args);

	return iResult;
}

TINT T2String8::Format(TCHAR* a_pcString, TINT size, const TCHAR* a_pcFormat, ...)
{
	va_list args;

	va_start(args, a_pcFormat);
	TINT iResult = FormatV(a_pcString, size, a_pcFormat, args);
	va_end(args);

	return iResult;
}

TINT T2String8::Compare(const TCHAR* str1, const TCHAR* str2, TSIZE size)
{
	if (size != -1) return strncmp(str1, str2, size);

	return strcmp(str1, str2);
}

TINT T2String8::CompareNoCase(const TCHAR* str1, const TCHAR* str2, TSIZE size /*= -1*/)
{
	if (size != -1) return _strnicmp(str1, str2, size);

	return _stricmp(str1, str2);
}

TCHAR* T2String8::Copy(TCHAR* dst, const TCHAR* src, TSIZE size /*= -1*/)
{
	if (size != -1) return strncpy(dst, src, size);

	return strcpy(dst, src);
}

TCHAR* T2String8::Concat(TCHAR* dst, const TCHAR* src, TSIZE size /*= -1*/)
{
	if (size != -1) return strncat(dst, src, size);

	return strcat(dst, src);
}

TCHAR* T2String8::CopySafe(TCHAR* dst, const TCHAR* src, TSIZE size)
{
	TSIZE srcLen = Length(src);
	size         = TMath::Min(size - 1, srcLen);
	strncpy(dst, src, size);
	dst[size] = '\0';
	return dst;
}

const TCHAR* T2String8::FindChar(const TCHAR* str, TCHAR character)
{
	while (TTRUE)
	{
		if (*str == '\0') return TNULL;
		if (*str == character) return str;
		str++;
	}

	return TNULL;
}

TCHAR* T2String8::FindChar(TCHAR* str, TCHAR character)
{
	while (TTRUE)
	{
		if (*str == '\0') return TNULL;
		if (*str == character) return str;
		str++;
	}

	return TNULL;
}

const TCHAR* T2String8::FindString(const TCHAR* str, const TCHAR* substr)
{
	return strstr(str, substr);
}

TCHAR* T2String8::FindString(TCHAR* str, const TCHAR* substr)
{
	return strstr(str, substr);
}

TSIZE T2String8::Length(const TCHAR* str)
{
	if (str != TNULL) return strlen(str);
	return -1;
}

TBOOL T2String8::IsLowerCase(const TCHAR* str)
{
	while (*str != '\0' && T2Char8::IsLowerCase(*str)) str++;

	return *str == '\0';
}

TBOOL T2String8::IsUpperCase(const TCHAR* str)
{
	while (*str != '\0' && T2Char8::IsUpperCase(*str)) str++;

	return *str == '\0';
}

void T2String8::ToLowerCase(TCHAR* str)
{
	while (*str)
	{
		*str = T2Char8::ToLowerCase(*str);
		str++;
	}
}

void T2String8::ToUpperCase(TCHAR* str)
{
	while (*str)
	{
		*str = T2Char8::ToUpperCase(*str);
		str++;
	}
}

void T2String8::IntToString(TINT value, TCHAR* dst, TINT radix)
{
	_itoa(value, dst, radix);
}

void T2String8::IntToString(TINT value, TCHAR* dst, TINT unused, TINT radix)
{
	_itoa(value, dst, radix);
}

// $Barnyard: FUNCTION 006bb2a0
TINT T2String8::StringToInt(const TCHAR* src)
{
	return atoi(src);
}

TFLOAT T2String8::StringToFloat(const TCHAR* src)
{
	return TFLOAT(atof(src));
}

TCHAR* T2String8::SkipSpaces(TCHAR* str)
{
	TCHAR* pszCursor = str;
	while (T2Char8::IsSpace(*pszCursor)) pszCursor++;

	return pszCursor;
}

const TCHAR* T2String8::SkipSpaces(const TCHAR* str)
{
	const TCHAR* pszCursor = str;
	while (T2Char8::IsSpace(*pszCursor)) pszCursor++;

	return pszCursor;
}

TCHAR* T2String8::CreateCopy(const TCHAR* a_pcString, TSIZE a_uiSize /*= -1 */)
{
	TSIZE uiSize = (a_uiSize == -1) ? T2String8::Length(a_pcString) : a_uiSize;

	TCHAR* pcBuffer = new TCHAR[uiSize + 1];
	TUtil::MemCopy(pcBuffer, a_pcString, uiSize * sizeof(TCHAR));
	pcBuffer[uiSize] = '\0';

	return pcBuffer;
}

TOSHI_NAMESPACE_END
