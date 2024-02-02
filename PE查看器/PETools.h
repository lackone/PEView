#pragma once

#include <windows.h>
#include <stdio.h>

class PETools
{
public:
	//读取PE文件到内存中
	DWORD ReadPEFile(IN LPCTSTR filePath, OUT LPVOID* pFileBuffer);
	//将内存偏移转换为文件偏移
	DWORD RvaToFoa(IN LPVOID pFileBuffer, IN DWORD dwRva);
	//将文件偏移转换为内存偏移
	DWORD FoaToRva(IN LPVOID pFileBuffer, IN DWORD dwFoa);
	//将TCHAR转换成CHAR
	VOID TCHARToChar(IN LPCTSTR tstr, OUT LPSTR* str);
	//获取DOS头
	PIMAGE_DOS_HEADER GetDosHeader(IN LPVOID pFileBuffer);
	//获取NT头
	PIMAGE_NT_HEADERS GetNTHeader(IN LPVOID pFileBuffer);
	//获取标准PE头
	PIMAGE_FILE_HEADER GetFileHeader(IN LPVOID pFileBuffer);
	//获取可选PE头
	PIMAGE_OPTIONAL_HEADER GetOptionHeader(IN LPVOID pFileBuffer);
};

