#pragma once

#include <windows.h>
#include <stdio.h>
#include <vector>

typedef struct
{
	CHAR name[MAX_PATH]; //����
	DWORD fileOffset; //�ļ�ƫ��
	DWORD fileSize; //�ļ���С
	DWORD memoryOffset; //�ڴ�ƫ��
	DWORD memorySize; //�ڴ��С
	DWORD attribute; //��������
} SectionItem, * PSectionItem;

class PETools
{
public:
	//��ȡPE�ļ����ڴ���
	DWORD ReadPEFile(IN LPCTSTR filePath, OUT LPVOID* pFileBuffer);
	//���ڴ�ƫ��ת��Ϊ�ļ�ƫ��
	DWORD RvaToFoa(IN LPVOID pFileBuffer, IN DWORD dwRva);
	//���ļ�ƫ��ת��Ϊ�ڴ�ƫ��
	DWORD FoaToRva(IN LPVOID pFileBuffer, IN DWORD dwFoa);
	//��TCHARת����CHAR
	VOID TCHARToChar(IN LPCTSTR tstr, OUT LPSTR* str);
	//��ȡDOSͷ
	PIMAGE_DOS_HEADER GetDosHeader(IN LPVOID pFileBuffer);
	//��ȡNTͷ
	PIMAGE_NT_HEADERS GetNTHeader(IN LPVOID pFileBuffer);
	//��ȡ��׼PEͷ
	PIMAGE_FILE_HEADER GetFileHeader(IN LPVOID pFileBuffer);
	//��ȡ��ѡPEͷ
	PIMAGE_OPTIONAL_HEADER GetOptionHeader(IN LPVOID pFileBuffer);
	//��ȡ�ڱ�
	std::vector<SectionItem> GetSectionList(IN LPVOID pFileBuffer);
	//��ȡĿ¼
	IMAGE_DATA_DIRECTORY* GetDataDirectory(IN LPVOID pFileBuffer);
};

