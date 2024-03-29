#include "Tools.h"

/**
 * 提权
 */
BOOL Tools::AdjustPrivileges(HANDLE hProcess, LPCTSTR lpPrivilegeName)
{
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, lpPrivilegeName, &tp.Privileges[0].Luid);

		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}

/**
 * 调试输出
 */
VOID Tools::OutputDebugStringFormat(const TCHAR* format, ...)
{
	va_list args;
	va_start(args, format);

	int length = _vsctprintf(format, args);

	TCHAR* buffer = new TCHAR[length + 1];

	_vstprintf_s(buffer, length + 1, format, args);

	OutputDebugString(buffer);

	delete[] buffer;
	va_end(args);
}

/**
 * 调试输出
 */
VOID Tools::OutputDebugStringFormatA(const CHAR* format, ...)
{
	va_list args;
	va_start(args, format);

	int length = _vscprintf(format, args);

	CHAR* buffer = new CHAR[length + 1];

	vsprintf_s(buffer, length + 1, format, args);

	OutputDebugStringA(buffer);

	delete[] buffer;
	va_end(args);
}

/**
 * 将TCHAR转换成CHAR
 */
VOID Tools::TCHARToChar(IN LPCTSTR tstr, OUT LPSTR* str)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, tstr, -1, NULL, 0, NULL, NULL);
	LPSTR buf = (LPSTR)malloc(sizeof(CHAR) * size_needed);
	WideCharToMultiByte(CP_ACP, 0, tstr, -1, buf, size_needed, NULL, NULL);
	*str = buf;
}

/**
 * 将CHAR转换成TCHAR
 */
VOID Tools::CHARToTChar(IN LPCSTR str, OUT LPTSTR* tstr)
{
	int size_needed = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	LPTSTR buf = (LPTSTR)malloc(sizeof(CHAR) * size_needed);
	MultiByteToWideChar(CP_ACP, 0, str, -1, buf, size_needed);
	*tstr = buf;
}

/**
 * 将DWORD转换为TCHAR
 */
VOID Tools::DWordToTStr(IN DWORD data, OUT LPTSTR buf, IN SIZE_T len)
{
	memset(buf, 0, len * sizeof(TCHAR));
	_stprintf_s(buf, len, TEXT("%08x"), data);
}

/**
 * 将WORD转换为TCHAR
 */
VOID Tools::WordToTStr(IN WORD data, OUT LPTSTR buf, IN SIZE_T len)
{
	memset(buf, 0, len * sizeof(TCHAR));
	_stprintf_s(buf, MAX_PATH, TEXT("%04x"), data);
}