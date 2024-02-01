#pragma once

#include <windows.h>
#include <Tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#include <vector>

typedef struct
{
	DWORD pid;
	TCHAR szExeFile[MAX_PATH];
	DWORD imageBase;
	DWORD imageSize;
} ProcessItem, * PProcessItem;

typedef struct
{
	TCHAR szName[MAX_PATH];
	TCHAR szPath[MAX_PATH];
} ModuleItem, * PModuleItem;

class ProcessTools
{
public:
	//获取进程列表
	std::vector<ProcessItem> GetProcessList();
	//获取模块列表
	std::vector<ModuleItem> GetModuleList(DWORD pid);
	//获取进程基址
	VOID GetProcessImageBase(IN DWORD pid, OUT DWORD* imageBase, DWORD* imageSize);
};

