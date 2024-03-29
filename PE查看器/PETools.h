#pragma once

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>

#ifdef _UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif

typedef struct
{
	CHAR name[MAX_PATH]; //节名
	DWORD fileOffset; //文件偏移
	DWORD fileSize; //文件大小
	DWORD memoryOffset; //内存偏移
	DWORD memorySize; //内存大小
	DWORD attribute; //节区属性
} SectionItem, * PSectionItem;

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
	//获取节表
	std::vector<SectionItem> GetSectionList(IN LPVOID pFileBuffer);
	//获取目录
	IMAGE_DATA_DIRECTORY* GetDataDirectory(IN LPVOID pFileBuffer);
	//导出表详情
	_tstring GetExportDetail(IN LPVOID pFileBuffer);
	//导入表详情
	_tstring GetImportDetail(IN LPVOID pFileBuffer);
	//资源详情
	_tstring GetResourceDetail(IN LPVOID pFileBuffer);
	//重定位详情
	_tstring GetBaseRelocDetail(IN LPVOID pFileBuffer);
	//绑定导入详情
	_tstring GetBoundImportDetail(IN LPVOID pFileBuffer);
	//IAT详情
	_tstring GetIATDetail(IN LPVOID pFileBuffer);
};

