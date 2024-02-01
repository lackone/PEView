#include "ListCtrl.h"

ListCtrl::ListCtrl()
{

}

ListCtrl::ListCtrl(HWND hwnd, DWORD id)
{
	m_hwnd = hwnd;
	m_id = id;
}

VOID ListCtrl::Init()
{
	//设置整行选中
	SendMessage(m_hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
}

VOID ListCtrl::SetHwnd(HWND hwnd)
{
	m_hwnd = hwnd;
}

VOID ListCtrl::SetId(DWORD id)
{
	m_id = id;
}

HWND ListCtrl::GetHwnd()
{
	return m_hwnd;
}

DWORD ListCtrl::GetId()
{
	return m_id;
}

//添加列
VOID ListCtrl::AddColumn(LPCTSTR cols[], SIZE_T size)
{
	LV_COLUMN lc{ 0 };

	lc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	for (SIZE_T ix = 0; ix < size; ix++)
	{
		lc.pszText = (LPTSTR)cols[ix];
		lc.cx = 140;
		lc.iSubItem = ix;
		ListView_InsertColumn(m_hwnd, ix, &lc);
	}
}

//添加一行
VOID ListCtrl::AddOneRow(LPCTSTR rows[], SIZE_T size)
{
	LV_ITEM li{ 0 };

	li.mask = LVIF_TEXT;

	// 获取行数
	int itemCount = ListView_GetItemCount(m_hwnd); 

	for (SIZE_T ix = 0; ix < size; ix++)
	{
		li.pszText = (LPTSTR)rows[ix];
		li.iItem = itemCount;
		li.iSubItem = ix;

		if (ix == 0)
		{
			ListView_InsertItem(m_hwnd, &li);
		}
		else {
			ListView_SetItem(m_hwnd, &li);
		}
	}
}

//清空所有的行
VOID ListCtrl::ClearAllRows()
{
	ListView_DeleteAllItems(m_hwnd);
}