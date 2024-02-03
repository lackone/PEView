#include "PETools.h"

/**
 * ��ȡPE�ļ����ڴ��У�����ֵΪ��ȡ�����ֽ���
 * filePath �ļ�·��
 * fileBuffer ��ȡ�����ڴ�buffer
 */
DWORD PETools::ReadPEFile(IN LPCTSTR filePath, OUT LPVOID* pFileBuffer)
{
	//���ļ�
	LPSTR str = NULL;
	TCHARToChar(filePath, &str);

	FILE* fp;
	if (fopen_s(&fp, str, "rb") != 0)
	{
		DWORD code = GetLastError();
		return 0;
	}

	//��ȡ�ļ��Ĵ�С
	fseek(fp, 0, SEEK_END);
	DWORD fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//�����ڴ�
	LPVOID fBuf = malloc(fileSize);
	if (fBuf == NULL)
	{
		fclose(fp);
		return 0;
	}

	//��ȡ���ݵ�������ڴ���
	memset(fBuf, 0, fileSize);
	fread(fBuf, fileSize, 1, fp);

	*pFileBuffer = fBuf;

	//�ر��ļ��������ļ���С
	fclose(fp);

	free(str);

	return fileSize;
}

/**
 * ���ڴ�ƫ��ת��Ϊ�ļ�ƫ��
 * pFileBuffer �ļ�buffer
 * dwRva �ڴ�ƫ��
 */
DWORD PETools::RvaToFoa(IN LPVOID pFileBuffer, IN DWORD dwRva)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS32 nt;
	PIMAGE_FILE_HEADER pe;
	PIMAGE_OPTIONAL_HEADER32 opt;
	PIMAGE_SECTION_HEADER section;

	dos = (PIMAGE_DOS_HEADER)pFileBuffer;
	nt = (PIMAGE_NT_HEADERS32)((LPBYTE)dos + dos->e_lfanew);
	pe = (PIMAGE_FILE_HEADER)((LPBYTE)nt + 4);
	opt = (PIMAGE_OPTIONAL_HEADER32)((LPBYTE)pe + IMAGE_SIZEOF_FILE_HEADER);
	section = (PIMAGE_SECTION_HEADER)((LPBYTE)opt + pe->SizeOfOptionalHeader);

	//���RVAС��ͷ����С��ֱ�ӷ���RVA����Ϊ�ļ��������ģ�ͷ���������
	//����ļ��������ڴ����һ�������ļ��������ģ�Ҳ�����
	if (dwRva < opt->SizeOfHeaders || opt->FileAlignment == opt->SectionAlignment)
	{
		return dwRva;
	}

	//�����ڣ��ж�RVA���ĸ���
	for (int i = 0; i < pe->NumberOfSections; i++)
	{
		//�ж�RVA�ĸ���
		if (dwRva >= section->VirtualAddress && dwRva < section->VirtualAddress + section->Misc.VirtualSize)
		{
			return section->PointerToRawData + (dwRva - section->VirtualAddress);
		}
		section++;
	}

	return 0;
}

/**
 * ���ļ�ƫ��ת��Ϊ�ڴ�ƫ��
 * pFileBuffer �ļ�buffer
 * dwFoa �ļ�ƫ��
 */
DWORD PETools::FoaToRva(IN LPVOID pFileBuffer, IN DWORD dwFoa)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS32 nt;
	PIMAGE_FILE_HEADER pe;
	PIMAGE_OPTIONAL_HEADER32 opt;
	PIMAGE_SECTION_HEADER section;

	dos = (PIMAGE_DOS_HEADER)pFileBuffer;
	nt = (PIMAGE_NT_HEADERS32)((LPBYTE)dos + dos->e_lfanew);
	pe = (PIMAGE_FILE_HEADER)((LPBYTE)nt + 4);
	opt = (PIMAGE_OPTIONAL_HEADER32)((LPBYTE)pe + IMAGE_SIZEOF_FILE_HEADER);
	section = (PIMAGE_SECTION_HEADER)((LPBYTE)opt + pe->SizeOfOptionalHeader);

	//���FOAС��ͷ����С��ֱ�ӷ���FOA����Ϊ�ļ��������ģ�ͷ���������
	//����ļ��������ڴ����һ�������ļ��������ģ�Ҳ�����
	if (dwFoa < opt->SizeOfHeaders || opt->FileAlignment == opt->SectionAlignment)
	{
		return dwFoa;
	}

	//�����ڣ��ж�FOA���ĸ���
	for (int i = 0; i < pe->NumberOfSections; i++)
	{
		if (dwFoa >= section->PointerToRawData && dwFoa < section->PointerToRawData + section->SizeOfRawData)
		{
			return section->VirtualAddress + (dwFoa - section->PointerToRawData);
		}
		section++;
	}

	return 0;
}

/**
 * ��TCHARת����CHAR
 */
VOID PETools::TCHARToChar(IN LPCTSTR tstr, OUT LPSTR* str)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, tstr, -1, NULL, 0, NULL, NULL);
	LPSTR buf = (LPSTR)malloc(sizeof(CHAR) * size_needed);
	WideCharToMultiByte(CP_ACP, 0, tstr, -1, buf, size_needed, NULL, NULL);
	*str = buf;
}

//��ȡDOSͷ
PIMAGE_DOS_HEADER PETools::GetDosHeader(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;

	dos = PIMAGE_DOS_HEADER(pFileBuffer);

	return dos;
}

//��ȡNTͷ
PIMAGE_NT_HEADERS PETools::GetNTHeader(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS nt;

	dos = PIMAGE_DOS_HEADER(pFileBuffer);
	nt = PIMAGE_NT_HEADERS((LPBYTE)dos + dos->e_lfanew);

	return nt;
}

//��ȡ��׼PEͷ
PIMAGE_FILE_HEADER PETools::GetFileHeader(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS nt;
	PIMAGE_FILE_HEADER pe;

	dos = PIMAGE_DOS_HEADER(pFileBuffer);
	nt = PIMAGE_NT_HEADERS((LPBYTE)dos + dos->e_lfanew);
	pe = PIMAGE_FILE_HEADER((LPBYTE)nt + 4);

	return pe;
}

//��ȡ��ѡPEͷ
PIMAGE_OPTIONAL_HEADER PETools::GetOptionHeader(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS nt;
	PIMAGE_FILE_HEADER pe;
	PIMAGE_OPTIONAL_HEADER opt;

	dos = PIMAGE_DOS_HEADER(pFileBuffer);
	nt = PIMAGE_NT_HEADERS((LPBYTE)dos + dos->e_lfanew);
	pe = PIMAGE_FILE_HEADER((LPBYTE)nt + 4);
	opt = PIMAGE_OPTIONAL_HEADER((LPBYTE)pe + IMAGE_SIZEOF_FILE_HEADER);

	return opt;
}

//��ȡ�ڱ�
std::vector<SectionItem> PETools::GetSectionList(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS32 nt;
	PIMAGE_FILE_HEADER pe;
	PIMAGE_OPTIONAL_HEADER32 opt;
	PIMAGE_SECTION_HEADER section;

	std::vector<SectionItem> vsi;

	dos = (PIMAGE_DOS_HEADER)pFileBuffer;
	nt = (PIMAGE_NT_HEADERS32)((LPBYTE)dos + dos->e_lfanew);
	pe = (PIMAGE_FILE_HEADER)((LPBYTE)nt + 4);
	opt = (PIMAGE_OPTIONAL_HEADER32)((LPBYTE)pe + IMAGE_SIZEOF_FILE_HEADER);
	section = (PIMAGE_SECTION_HEADER)((LPBYTE)opt + pe->SizeOfOptionalHeader);

	for (int i = 0; i < pe->NumberOfSections; i++)
	{
		SectionItem si{ 0 };

		strncpy_s(si.name, MAX_PATH, (char*)section->Name, 8);

		si.fileOffset = section->PointerToRawData;
		si.fileSize = section->SizeOfRawData;
		si.memoryOffset = section->VirtualAddress;
		si.memorySize = section->Misc.VirtualSize;
		si.attribute = section->Characteristics;

		vsi.push_back(si);

		section++;
	}

	return vsi;
}

/**
 * ��ȡĿ¼
 */
IMAGE_DATA_DIRECTORY* PETools::GetDataDirectory(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS32 nt;

	dos = (PIMAGE_DOS_HEADER)pFileBuffer;
	nt = (PIMAGE_NT_HEADERS32)((LPBYTE)dos + dos->e_lfanew);
	IMAGE_DATA_DIRECTORY* dir = nt->OptionalHeader.DataDirectory;

	return dir;
}

/**
 * ����������
 */
_tstring PETools::GetExportDetail(IN LPVOID pFileBuffer)
{
	//��ʱδʵ�֣�����Ȥ�Ŀ�������ʵ��

	_tstring str;

	str += TEXT("����ʯ1");

	return str;
}

/**
 * ���������
 */
_tstring PETools::GetImportDetail(IN LPVOID pFileBuffer)
{
	//��ʱδʵ�֣�����Ȥ�Ŀ�������ʵ��

	_tstring str;

	str += TEXT("����ʯ2");

	return str;
}

/**
 * ��Դ����
 */
_tstring PETools::GetResourceDetail(IN LPVOID pFileBuffer)
{
	//��ʱδʵ�֣�����Ȥ�Ŀ�������ʵ��

	_tstring str;

	str += TEXT("����ʯ3");

	return str;
}

/**
 * �ض�λ����
 */
_tstring PETools::GetBaseRelocDetail(IN LPVOID pFileBuffer)
{
	//��ʱδʵ�֣�����Ȥ�Ŀ�������ʵ��

	_tstring str;

	str += TEXT("����ʯ4");

	return str;
}

/**
 * �󶨵�������
 */
_tstring PETools::GetBoundImportDetail(IN LPVOID pFileBuffer)
{
	//��ʱδʵ�֣�����Ȥ�Ŀ�������ʵ��

	_tstring str;

	str += TEXT("����ʯ5");

	return str;
}

/**
 * IAT����
 */
_tstring PETools::GetIATDetail(IN LPVOID pFileBuffer)
{
	//��ʱδʵ�֣�����Ȥ�Ŀ�������ʵ��

	_tstring str;

	str += TEXT("����ʯ6");

	return str;
}