#pragma once

#include <windows.h>
#include <stdarg.h>
#include <tchar.h>
#include <stdio.h>

class Tools
{
public:
	//��Ȩ
	static BOOL AdjustPrivileges(HANDLE hProcess, LPCTSTR lpPrivilegeName);
	//�������
	static VOID OutputDebugStringFormat(const TCHAR* format, ...);
	//�������
	static VOID OutputDebugStringFormatA(const CHAR* format, ...);
	//��TCHARת����CHAR
	VOID TCHARToChar(IN LPCTSTR tstr, OUT LPSTR* str);
	//��DWORDת��ΪTCHAR
	VOID DWordToTStr(IN DWORD data, OUT LPTSTR buf, IN SIZE_T len);
	//��WORDת��ΪTCHAR
	VOID WordToTStr(IN WORD data, OUT LPTSTR buf, IN SIZE_T len);
};
