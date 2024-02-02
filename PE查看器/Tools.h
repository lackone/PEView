#pragma once

#include <windows.h>
#include <stdarg.h>
#include <tchar.h>
#include <stdio.h>

class Tools
{
public:
	//提权
	static BOOL AdjustPrivileges(HANDLE hProcess, LPCTSTR lpPrivilegeName);
	//调试输出
	static VOID OutputDebugStringFormat(const TCHAR* format, ...);
	//调试输出
	static VOID OutputDebugStringFormatA(const CHAR* format, ...);
	//将TCHAR转换成CHAR
	VOID TCHARToChar(IN LPCTSTR tstr, OUT LPSTR* str);
	//将CHAR转换成TCHAR
	VOID CHARToTChar(IN LPCSTR str, OUT LPTSTR *tstr);
	//将DWORD转换为TCHAR
	VOID DWordToTStr(IN DWORD data, OUT LPTSTR buf, IN SIZE_T len);
	//将WORD转换为TCHAR
	VOID WordToTStr(IN WORD data, OUT LPTSTR buf, IN SIZE_T len);
};
