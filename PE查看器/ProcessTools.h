#pragma once

#include <windows.h>
#include <Tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#include <vector>

typedef struct
{
	DWORD pid; //PID
	TCHAR szExeFile[MAX_PATH]; //进程名
	DWORD imageBase; //基址
	DWORD imageSize; //镜像大小
} ProcessItem, * PProcessItem;

typedef struct
{
	TCHAR szName[MAX_PATH]; //模块名
	TCHAR szPath[MAX_PATH]; //模块路径
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

