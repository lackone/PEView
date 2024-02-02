#include <windows.h>
#include <vector>
#include "resource.h"
#include "ListCtrl.h"
#include "ProcessTools.h"
#include "Tools.h"
#include "PETools.h"

HINSTANCE g_hInstance;
UINT_PTR g_timer;
//���ھ��
HWND hListProcess;
HWND hListModule;
HWND hListSection;
//�ؼ���
ListCtrl lcProcess;
ListCtrl lcModule;
ListCtrl lcSection;
//���߶���
ProcessTools pt;
Tools tools;
PETools pe;
//�����б�����
BOOL pidAsc = TRUE;
BOOL imageBaseAsc = TRUE;
BOOL imageSizeAsc = TRUE;
//�������������
int LastColumnIndex = 1;
//���������PID
DWORD LastRowPID = -1;
//��ѡEXE��·��
TCHAR exePath[MAX_PATH]{ 0 };
LPVOID exeFileBuffer = NULL;

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
 * ���½ڱ�
 */
VOID flushSectionList()
{
	std::vector<SectionItem> vsi = pe.GetSectionList(exeFileBuffer);
	std::vector<SectionItem>::iterator it;

	LPCTSTR row[6]{ 0 };

	lcSection.ClearAllRows();

	for (it = vsi.begin(); it != vsi.end(); ++it)
	{
		tools.CHARToTChar(it->name, (LPTSTR*)(&row[0]));

		TCHAR buf1[MAX_PATH]{ 0 };
		tools.DWordToTStr(it->fileOffset, buf1, MAX_PATH);
		row[1] = buf1;

		TCHAR buf2[MAX_PATH]{ 0 };
		tools.DWordToTStr(it->fileSize, buf2, MAX_PATH);
		row[2] = buf2;

		TCHAR buf3[MAX_PATH]{ 0 };
		tools.DWordToTStr(it->memoryOffset, buf3, MAX_PATH);
		row[3] = buf3;

		TCHAR buf4[MAX_PATH]{ 0 };
		tools.DWordToTStr(it->memorySize, buf4, MAX_PATH);
		row[4] = buf4;

		TCHAR buf5[MAX_PATH]{ 0 };
		tools.DWordToTStr(it->attribute, buf5, MAX_PATH);
		row[5] = buf5;

		lcSection.AddOneRow(row, 6);
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
 * �ڱ���
 */
INT_PTR CALLBACK SectionDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	case WM_INITDIALOG:
	{
		hListSection = GetDlgItem(hwnd, IDC_LIST_SECTION);

		LPCTSTR sectionCols[] = { TEXT("����"), TEXT("�ļ�ƫ��"), TEXT("�ļ���С"), TEXT("�ڴ�ƫ��"), TEXT("�ڴ��С"), TEXT("��������") };
		lcSection.SetHwnd(hListSection);
		lcSection.SetId(IDC_LIST_PROCESS);
		lcSection.Init();
		lcSection.AddColumn(sectionCols, 6);

		flushSectionList();
	}
	return TRUE;
	}
	return FALSE;
}

/**
 * PE�鿴������
 */
INT_PTR CALLBACK PeInfoDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		EndDialog(hwnd, 0);
	}
	return TRUE;
	case WM_INITDIALOG:
	{
		//��ȡPE�ļ�
		pe.ReadPEFile(exePath, &exeFileBuffer);

		//��ȡDOSͷ
		PIMAGE_DOS_HEADER dosHeader = pe.GetDosHeader(exeFileBuffer);
		//��ȡNTͷ
		PIMAGE_NT_HEADERS ntHeader = pe.GetNTHeader(exeFileBuffer);
		//��ȡ��׼PEͷ
		PIMAGE_FILE_HEADER fileHeader = pe.GetFileHeader(exeFileBuffer);
		//��ȡ��ѡPEͷ
		PIMAGE_OPTIONAL_HEADER optHeader = pe.GetOptionHeader(exeFileBuffer);

		TCHAR buf[MAX_PATH]{ 0 };

		tools.DWordToTStr(optHeader->AddressOfEntryPoint, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_AddressOfEntryPoint), buf);

		tools.DWordToTStr(optHeader->ImageBase, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_ImageBase), buf);

		tools.DWordToTStr(optHeader->SizeOfImage, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_SizeOfImage), buf);

		tools.DWordToTStr(optHeader->BaseOfCode, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_BaseOfCode), buf);

		tools.DWordToTStr(optHeader->BaseOfData, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_BaseOfData), buf);

		tools.DWordToTStr(optHeader->SectionAlignment, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_SectionAlignment), buf);

		tools.DWordToTStr(optHeader->FileAlignment, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_FileAlignment), buf);

		tools.WordToTStr(optHeader->Magic, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_Magic), buf);

		tools.WordToTStr(optHeader->Subsystem, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_Subsystem), buf);

		tools.WordToTStr(fileHeader->NumberOfSections, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_NumberOfSections), buf);

		tools.DWordToTStr(fileHeader->TimeDateStamp, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_TimeDateStamp), buf);

		tools.DWordToTStr(optHeader->SizeOfHeaders, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_SizeOfHeaders), buf);

		tools.WordToTStr(fileHeader->Characteristics, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_Characteristics), buf);

		tools.DWordToTStr(optHeader->CheckSum, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_CheckSum), buf);

		tools.WordToTStr(fileHeader->SizeOfOptionalHeader, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_SizeOfOptionalHeader), buf);

		tools.DWordToTStr(optHeader->NumberOfRvaAndSizes, buf, MAX_PATH);
		SetWindowText(GetDlgItem(hwnd, IDC_EDIT_NumberOfRvaAndSizes), buf);
	}
	return TRUE;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_PE_INFO_EXIT:
			EndDialog(hwnd, 0);
			break;
		case IDC_BUTTON_PE_INFO_DIR:
			break;
		case IDC_BUTTON_PE_INFO_SECTION:
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTION), hwnd, SectionDlgProc);
			break;
		}
	}
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
			OPENFILENAME ofn{ 0 };

			memset(exePath, 0, MAX_PATH * sizeof(TCHAR));

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = exePath;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = TEXT("EXE Files (*.exe)\0*.exe\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (!GetOpenFileName(&ofn))
			{
				MessageBox(hwnd, TEXT("��ѡ��һ��PE�ļ�"), TEXT("����"), MB_OK);
				return FALSE;
			}
			if (_tcslen(exePath) <= 0)
			{
				MessageBox(hwnd, TEXT("��ѡ��һ��PE�ļ�"), TEXT("����"), MB_OK);
				return FALSE;
			}

			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_PE_INFO), hwnd, PeInfoDlgProc);
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