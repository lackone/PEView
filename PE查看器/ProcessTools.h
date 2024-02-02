#pragma once

#include <windows.h>
#include <Tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#include <vector>

typedef struct
{
	DWORD pid; //PID
	TCHAR szExeFile[MAX_PATH]; //������
	DWORD imageBase; //��ַ
	DWORD imageSize; //�����С
} ProcessItem, * PProcessItem;

typedef struct
{
	TCHAR szName[MAX_PATH]; //ģ����
	TCHAR szPath[MAX_PATH]; //ģ��·��
} ModuleItem, * PModuleItem;

class ProcessTools
{
public:
	//��ȡ�����б�
	std::vector<ProcessItem> GetProcessList();
	//��ȡģ���б�
	std::vector<ModuleItem> GetModuleList(DWORD pid);
	//��ȡ���̻�ַ
	VOID GetProcessImageBase(IN DWORD pid, OUT DWORD* imageBase, DWORD* imageSize);
};

