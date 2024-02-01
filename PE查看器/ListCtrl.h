#pragma once

#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

class ListCtrl
{
public:
	//构造函数
	ListCtrl();
	ListCtrl(HWND hwnd, DWORD id);
	//初始化操作
	VOID Init();
	//设置和获取hwnd和控件ID
	VOID SetHwnd(HWND hwnd);
	VOID SetId(DWORD id);
	HWND GetHwnd();
	DWORD GetId();
	//添加列
	VOID AddColumn(LPCTSTR cols[], SIZE_T size);
	//添加一行
	VOID AddOneRow(LPCTSTR cols[], SIZE_T size);
	//清空所有的行
	VOID ClearAllRows();
public:
	//控件的窗口句柄
	HWND m_hwnd;
	//控件的ID
	DWORD m_id;
};

