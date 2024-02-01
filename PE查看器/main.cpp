#include <windows.h>
#include <vector>
#include "resource.h"
#include "ListCtrl.h"
#include "ProcessTools.h"
#include "Tools.h"

HINSTANCE g_hInstance;
UINT_PTR g_timer;
//���ھ��
HWND hListProcess;
HWND hListModule;
//�ؼ���
ListCtrl lcProcess;
ListCtrl lcModule;
//���߶���
ProcessTools pt;
Tools tools;
//�����б�����
BOOL pidAsc = TRUE;
BOOL imageBaseAsc = TRUE;
BOOL imageSizeAsc = TRUE;
//�������������
int LastColumnIndex = 1;
//���������PID
DWORD LastRowPID = -1;

/**
 * ���½����б�
 */
VOID flushProcessList()
{
	std::vector<ProcessItem> vpi = pt.GetProcessList();
	std::vector<ProcessItem>::iterator it;

	LPCTSTR row[4]{ 0 };

	TCHAR pidBuf[32]{ 0 };
	TCHAR imageBaseBuf[32]{ 0 };
	TCHAR imageSizeBuf[32]{ 0 };

	lcProcess.ClearAllRows();

	for (it = vpi.begin(); it != vpi.end(); ++it)
	{
		memset(pidBuf, 0, 32 * sizeof(TCHAR));
		memset(imageBaseBuf, 0, 32 * sizeof(TCHAR));
		memset(imageSizeBuf, 0, 32 * sizeof(TCHAR));

		row[0] = it->szExeFile;

		_itot_s(it->pid, pidBuf, 32, 10);
		row[1] = pidBuf;

		_itot_s(it->imageBase, imageBaseBuf, 32, 10);
		row[2] = imageBaseBuf;

		_itot_s(it->imageSize, imageSizeBuf, 32, 10);
		row[3] = imageSizeBuf;

		lcProcess.AddOneRow(row, 4);
	}
}

/**
 * ���ֽ���ˢ��
 */
VOID partFlushProcessList()
{
	std::vector<ProcessItem> vpi = pt.GetProcessList();
	std::vector<ProcessItem>::iterator it;

	// ��ȡListView�ؼ��е�����
	int itemCount = ListView_GetItemCount(hListProcess);
	BOOL flag = false;

	LPCTSTR row[4]{ 0 };

	TCHAR pidBuf[32]{ 0 };
	TCHAR imageBaseBuf[32]{ 0 };
	TCHAR imageSizeBuf[32]{ 0 };

	TCHAR pidStr[MAX_PATH]{ 0 };
	DWORD pid{ 0 };

	//ѭ���ж��»�ȡ�Ľ����б��Ƿ��Ѿ���ListView��
	for (it = vpi.begin(); it != vpi.end(); ++it)
	{
		flag = false;
		for (int i = 0; i < itemCount; i++)
		{
			memset(pidStr, 0, MAX_PATH * sizeof(TCHAR));
			ListView_GetItemText(hListProcess, i, 1, pidStr, MAX_PATH);
			pid = _ttoi(pidStr);

			if (it->pid == pid)
			{
				flag = true;
				break;
			}
		}
		//���û�У�����ӽ�ListView
		if (!flag)
		{
			memset(pidBuf, 0, 32 * sizeof(TCHAR));
			memset(imageBaseBuf, 0, 32 * sizeof(TCHAR));
			memset(imageSizeBuf, 0, 32 * sizeof(TCHAR));

			row[0] = it->szExeFile;

			_itot_s(it->pid, pidBuf, 32, 10);
			row[1] = pidBuf;

			_itot_s(it->imageBase, imageBaseBuf, 32, 10);
			row[2] = imageBaseBuf;

			_itot_s(it->imageSize, imageSizeBuf, 32, 10);
			row[3] = imageSizeBuf;

			lcProcess.AddOneRow(row, 4);
		}
	}
}

/**
 * ����ģ���б�
 */
VOID flushModuleList(DWORD pid)
{
	std::vector<ModuleItem> vmi = pt.GetModuleList(pid);
	std::vector<ModuleItem>::iterator it;

	LPCTSTR row[2]{ 0 };

	lcModule.ClearAllRows();

	for (it = vmi.begin(); it != vmi.end(); ++it)
	{
		row[0] = it->szName;
		row[1] = it->szPath;

		lcModule.AddOneRow(row, 2);
	}
}

/**
 * ���ڶԻ�����Ϣ������
 */
INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	}
	return FALSE;
}

/**
 * ListView����
 */
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TCHAR data1Str[MAX_PATH]{ 0 };
	TCHAR data2Str[MAX_PATH]{ 0 };

	int columnIndex = (int)lParamSort;

	// ��ȡ���ı�
	ListView_GetItemText(hListProcess, lParam1, columnIndex, data1Str, MAX_PATH);
	ListView_GetItemText(hListProcess, lParam2, columnIndex, data2Str, MAX_PATH);

	DWORD data1 = _ttoi(data1Str);
	DWORD data2 = _ttoi(data2Str);

	if (columnIndex == 1)
	{
		return data1 > data2 ? (pidAsc ? 1 : 0) : (pidAsc ? 0 : 1);
	}
	else if (columnIndex == 2)
	{
		return data1 > data2 ? (imageBaseAsc ? 1 : 0) : (imageBaseAsc ? 0 : 1);
	}
	else if (columnIndex == 3)
	{
		return data1 > data2 ? (imageSizeAsc ? 1 : 0) : (imageSizeAsc ? 0 : 1);
	}
}

/**
 * ��ʱ��
 */
VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	partFlushProcessList();
	//����
	ListView_SortItemsEx(hListProcess, CompareFunc, LastColumnIndex);
	//����ѡ����
	if (LastRowPID != -1)
	{
		TCHAR pidStr[MAX_PATH]{ 0 };
		DWORD pid{ 0 };

		// ��ȡListView�ؼ��е�����
		int itemCount = ListView_GetItemCount(hListProcess);
		for (int i = 0; i < itemCount; i++)
		{
			memset(pidStr, 0, MAX_PATH * sizeof(TCHAR));

			ListView_GetItemText(hListProcess, i, 1, pidStr, MAX_PATH);

			pid = _ttoi(pidStr);

			if (LastRowPID == pid)
			{
				ListView_SetItemState(hListProcess, i, LVIS_SELECTED, LVIS_SELECTED);
				break;
			}
		}
	}
}

/**
 * ���Ի�����Ϣ������
 */
INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hListProcess = GetDlgItem(hwnd, IDC_LIST_PROCESS);
		hListModule = GetDlgItem(hwnd, IDC_LIST_MODULE);

		LPCTSTR processCols[] = { TEXT("����"), TEXT("PID"), TEXT("�����ַ"), TEXT("�����С") };
		lcProcess.SetHwnd(hListProcess);
		lcProcess.SetId(IDC_LIST_PROCESS);
		lcProcess.Init();
		lcProcess.AddColumn(processCols, 4);

		LPCTSTR moduleCols[] = { TEXT("ģ������"), TEXT("ģ��λ��") };
		lcModule.SetHwnd(hListModule);
		lcModule.SetId(IDC_LIST_MODULE);
		lcModule.Init();
		lcModule.AddColumn(moduleCols, 2);

		//���½����б�
		flushProcessList();

		//����
		ListView_SortItemsEx(hListProcess, CompareFunc, LastColumnIndex);

		g_timer = SetTimer(hwnd, 1, 5000, TimerProc);
	}
	return TRUE;
	case WM_CLOSE:
	{
		KillTimer(hwnd, g_timer);
		EndDialog(hwnd, 0);
	}
	return TRUE;
	case WM_NOTIFY:
	{
		NMHDR* pnmh = (NMHDR*)lParam;
		//�е���¼�
		if (pnmh->hwndFrom == hListProcess && pnmh->code == NM_CLICK)
		{
			LPNMITEMACTIVATE pNMItemActivate = (LPNMITEMACTIVATE)lParam;
			if (pNMItemActivate->iItem != -1)
			{
				int nIndex = ListView_GetNextItem(hListProcess, -1, LVNI_SELECTED);
				if (nIndex != -1)
				{
					TCHAR pidStr[MAX_PATH]{ 0 };
					ListView_GetItemText(hListProcess, nIndex, 1, pidStr, MAX_PATH);
					DWORD pid = _ttoi(pidStr);
					LastRowPID = pid;
					flushModuleList(pid);
				}
			}
		}
		//�е���¼�
		if (pnmh->hwndFrom == hListProcess && pnmh->code == LVN_COLUMNCLICK)
		{
			NMLISTVIEW* pNMListView = (NMLISTVIEW*)lParam;
			LastColumnIndex = pNMListView->iSubItem;
			if (pNMListView->iSubItem == 1)
			{
				pidAsc = !pidAsc;
			}
			else if (pNMListView->iSubItem == 2)
			{
				imageBaseAsc = !imageBaseAsc;
			}
			else if (pNMListView->iSubItem == 3)
			{
				imageSizeAsc = !imageSizeAsc;
			}
			ListView_SortItemsEx(hListProcess, CompareFunc, LastColumnIndex);
		}
	}
	return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_PE_VIEW:
		{
			//PE�鿴��
		}
		break;
		case IDC_BUTTON_EXIT:
		{
			//�˳�
			KillTimer(hwnd, g_timer);
			EndDialog(hwnd, 0);
		}
		break;
		case IDC_BUTTON_ABOUT:
		{
			//���ڶԻ���
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwnd, AboutDlgProc);
		}
		break;
		}
		return TRUE;
	}

	return FALSE;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	tools.AdjustPrivileges(GetCurrentProcess(), SE_DEBUG_NAME);

	g_hInstance = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, MainDlgProc);
	return 0;
}