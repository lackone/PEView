#include "ProcessTools.h"

//获取进程列表
std::vector<ProcessItem> ProcessTools::GetProcessList()
{
	std::vector<ProcessItem> vpi;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return vpi;
	}
	PROCESSENTRY32 pe{ 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	BOOL bRet = Process32First(hSnapshot, &pe);
	while (bRet)
	{
		ProcessItem pi{ 0 };

		//获取PID和路径
		pi.pid = pe.th32ProcessID;
		_tcscpy_s(pi.szExeFile, MAX_PATH, pe.szExeFile);

		//获取基址和大小
		GetProcessImageBase(pe.th32ProcessID, &pi.imageBase, &pi.imageSize);

		vpi.push_back(pi);

		bRet = Process32Next(hSnapshot, &pe);
	}
	CloseHandle(hSnapshot);

	return vpi;
}

//获取模块列表
std::vector<ModuleItem> ProcessTools::GetModuleList(DWORD pid)
{
	std::vector<ModuleItem> vmi;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return vmi;
	}

	MODULEENTRY32 me{ 0 };
	me.dwSize = sizeof(MODULEENTRY32);

	BOOL bRet = Module32First(hSnapshot, &me);
	while (bRet)
	{
		ModuleItem mi{ 0 };

		_tcscpy_s(mi.szName, MAX_PATH, me.szModule);
		_tcscpy_s(mi.szPath, MAX_PATH, me.szExePath);

		vmi.push_back(mi);

		bRet = Module32Next(hSnapshot, &me);
	}

	CloseHandle(hSnapshot);

	return vmi;
}

//获取进程的基址
VOID ProcessTools::GetProcessImageBase(IN DWORD pid, OUT DWORD* imageBase, DWORD* imageSize)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return;
	}

	MODULEENTRY32 me{ 0 };
	me.dwSize = sizeof(MODULEENTRY32);

	BOOL bRet = Module32First(hSnapshot, &me);
	if (bRet)
	{
		*imageBase = (DWORD)me.hModule;
		*imageSize = me.modBaseSize;
	}

	CloseHandle(hSnapshot);
}