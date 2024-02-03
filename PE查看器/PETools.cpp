#include "PETools.h"

/**
 * 读取PE文件到内存中，返回值为读取到的字节数
 * filePath 文件路径
 * fileBuffer 读取到的内存buffer
 */
DWORD PETools::ReadPEFile(IN LPCTSTR filePath, OUT LPVOID* pFileBuffer)
{
	//打开文件
	LPSTR str = NULL;
	TCHARToChar(filePath, &str);

	FILE* fp;
	if (fopen_s(&fp, str, "rb") != 0)
	{
		DWORD code = GetLastError();
		return 0;
	}

	//获取文件的大小
	fseek(fp, 0, SEEK_END);
	DWORD fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//申请内存
	LPVOID fBuf = malloc(fileSize);
	if (fBuf == NULL)
	{
		fclose(fp);
		return 0;
	}

	//读取数据到申请的内存中
	memset(fBuf, 0, fileSize);
	fread(fBuf, fileSize, 1, fp);

	*pFileBuffer = fBuf;

	//关闭文件，返回文件大小
	fclose(fp);

	free(str);

	return fileSize;
}

/**
 * 将内存偏移转换为文件偏移
 * pFileBuffer 文件buffer
 * dwRva 内存偏移
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

	//如果RVA小于头部大小，直接返回RVA，因为文件和拉伸后的，头部并不会变
	//如果文件对齐与内存对齐一样，则文件和拉伸后的，也不会变
	if (dwRva < opt->SizeOfHeaders || opt->FileAlignment == opt->SectionAlignment)
	{
		return dwRva;
	}

	//遍历节，判断RVA在哪个节
	for (int i = 0; i < pe->NumberOfSections; i++)
	{
		//判断RVA哪个节
		if (dwRva >= section->VirtualAddress && dwRva < section->VirtualAddress + section->Misc.VirtualSize)
		{
			return section->PointerToRawData + (dwRva - section->VirtualAddress);
		}
		section++;
	}

	return 0;
}

/**
 * 将文件偏移转换为内存偏移
 * pFileBuffer 文件buffer
 * dwFoa 文件偏移
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

	//如果FOA小于头部大小，直接返回FOA，因为文件和拉伸后的，头部并不会变
	//如果文件对齐与内存对齐一样，则文件和拉伸后的，也不会变
	if (dwFoa < opt->SizeOfHeaders || opt->FileAlignment == opt->SectionAlignment)
	{
		return dwFoa;
	}

	//遍历节，判断FOA在哪个节
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
 * 将TCHAR转换成CHAR
 */
VOID PETools::TCHARToChar(IN LPCTSTR tstr, OUT LPSTR* str)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, tstr, -1, NULL, 0, NULL, NULL);
	LPSTR buf = (LPSTR)malloc(sizeof(CHAR) * size_needed);
	WideCharToMultiByte(CP_ACP, 0, tstr, -1, buf, size_needed, NULL, NULL);
	*str = buf;
}

//获取DOS头
PIMAGE_DOS_HEADER PETools::GetDosHeader(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;

	dos = PIMAGE_DOS_HEADER(pFileBuffer);

	return dos;
}

//获取NT头
PIMAGE_NT_HEADERS PETools::GetNTHeader(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS nt;

	dos = PIMAGE_DOS_HEADER(pFileBuffer);
	nt = PIMAGE_NT_HEADERS((LPBYTE)dos + dos->e_lfanew);

	return nt;
}

//获取标准PE头
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

//获取可选PE头
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

//获取节表
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
 * 获取目录
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
 * 导出表详情
 */
_tstring PETools::GetExportDetail(IN LPVOID pFileBuffer)
{
	//暂时未实现，有兴趣的可以自已实现

	_tstring str;

	str += TEXT("我姓石1");

	return str;
}

/**
 * 导入表详情
 */
_tstring PETools::GetImportDetail(IN LPVOID pFileBuffer)
{
	//暂时未实现，有兴趣的可以自已实现

	_tstring str;

	str += TEXT("我姓石2");

	return str;
}

/**
 * 资源详情
 */
_tstring PETools::GetResourceDetail(IN LPVOID pFileBuffer)
{
	//暂时未实现，有兴趣的可以自已实现

	_tstring str;

	str += TEXT("我姓石3");

	return str;
}

/**
 * 重定位详情
 */
_tstring PETools::GetBaseRelocDetail(IN LPVOID pFileBuffer)
{
	//暂时未实现，有兴趣的可以自已实现

	_tstring str;

	str += TEXT("我姓石4");

	return str;
}

/**
 * 绑定导入详情
 */
_tstring PETools::GetBoundImportDetail(IN LPVOID pFileBuffer)
{
	//暂时未实现，有兴趣的可以自已实现

	_tstring str;

	str += TEXT("我姓石5");

	return str;
}

/**
 * IAT详情
 */
_tstring PETools::GetIATDetail(IN LPVOID pFileBuffer)
{
	//暂时未实现，有兴趣的可以自已实现

	_tstring str;

	str += TEXT("我姓石6");

	return str;
}