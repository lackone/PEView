#pragma once

#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

class ListCtrl
{
public:
	//���캯��
	ListCtrl();
	ListCtrl(HWND hwnd, DWORD id);
	//��ʼ������
	VOID Init();
	//���úͻ�ȡhwnd�Ϳؼ�ID
	VOID SetHwnd(HWND hwnd);
	VOID SetId(DWORD id);
	HWND GetHwnd();
	DWORD GetId();
	//�����
	VOID AddColumn(LPCTSTR cols[], SIZE_T size);
	//���һ��
	VOID AddOneRow(LPCTSTR cols[], SIZE_T size);
	//������е���
	VOID ClearAllRows();
public:
	//�ؼ��Ĵ��ھ��
	HWND m_hwnd;
	//�ؼ���ID
	DWORD m_id;
};

