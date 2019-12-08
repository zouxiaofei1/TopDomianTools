//Ϊ�˼���GUI.cpp����
//һ���ֺ���������Class��ȫ�ֱ���û�й�ϵ��������������

#pragma once
#include "stdafx.h"
#include "GUI.h"
#pragma warning(disable:4996)
VOID RestartDirect();
VOID LockCursor();
DWORD WINAPI RestartThread(LPVOID pM);

void charTowchar(const char* chr, wchar_t* wchar, int size)
{
	MultiByteToWideChar(CP_ACP,
		0, chr, (int)strlen(chr) + 1,
		wchar,
		size / sizeof(wchar[0]));
}

const unsigned int Hash(const wchar_t* str)
{
	unsigned int seed = 131;
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

VOID LockCursor()//��ס���
{
	RECT rect;
	rect.bottom = rect.top = GetSystemMetrics(SM_CYSCREEN);
	rect.right = rect.left = GetSystemMetrics(SM_CXSCREEN);//��������������½�
	ClipCursor(&rect);//���ǰ�windows������ctrl+alt+del�ͻ�ָ�
}

__forceinline VOID RestartDirect()
{//�����߳�
	CreateThread(0, 0, RestartThread, 0, 0, 0);
}
DWORD WINAPI RestartThread(LPVOID pM)
{
	(pM);
	constexpr auto SE_SHUTDOWN_PRIVILEGE = 0x13;
	typedef int(__stdcall* PFN_RtlAdjustPrivilege)(int, BOOL, BOOL, int*);
	typedef int(__stdcall* PFN_ZwShutdownSystem)(int);
	HMODULE hModule = ::LoadLibrary(_T("ntdll.dll"));
	if (hModule != NULL)
	{//Ѱ��ZwShutdownSystem�ĵ�ַ
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress(hModule, "RtlAdjustPrivilege"); PFN_ZwShutdownSystem
			pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress(hModule, "ZwShutdownSystem");
		if (pfnRtl != NULL && pfnShutdown != NULL)//
		{//����SE_SHUTDOWN_PRIVILEGE��Ȩ��
			int en = 0;//��Ȥ�������Ȩ���ڷǹ���Ա�¾��ܵõ�
			int nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
			if (nRet == 0x0C000007C) nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en); //SH_SHUTDOWN = 0; //SH_RESTART = 1; //SH_POWEROFF = 2;
			nRet = pfnShutdown(1);
		}
	}
	return 0;
}

//typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL(WINAPI* PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL GetVersionEx2(LPOSVERSIONINFOW lpVersionInformation)
{//���ڲ�һ��ĺ�����ȡϵͳ�汾
	HMODULE hNtDll = GetModuleHandleW(L"NTDLL");

	typedef int(__stdcall* tRtlGetVersion)(PRTL_OSVERSIONINFOW);
	tRtlGetVersion pRtlGetVersion = nullptr;
	if (hNtDll)pRtlGetVersion = (tRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
	if (pRtlGetVersion)return pRtlGetVersion((PRTL_OSVERSIONINFOW)lpVersionInformation) >= 0;
	return FALSE;
}

BOOL GetOSDisplayString(wchar_t* pszOS)
{//��ȡϵͳ�汾�ĺ���
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));//��˵GetVersionEx�����жϰ汾Ч������
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);//�������ж�һ̨win10����
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);//�õ��Ľ��һ����6.2(9200)
	if (!bOsVersionInfoEx) return FALSE;//������ȷ��ʾ��ϸ�汾��

	wchar_t tmp[101];//����win7��ǰ��ϵͳ��GetVersionExû������
	_itow_s(osvi.dwMajorVersion, tmp, 10);//��汾��
	wcscpy(pszOS, tmp); wcscat(pszOS, L".");
	_itow_s(osvi.dwMinorVersion, tmp, 10);//С�汾��
	wcscat(pszOS, tmp); wcscat(pszOS, L".");
	_itow_s(osvi.dwBuildNumber, tmp, 10);//build
	wcscat(pszOS, tmp);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion >= 6)
	{//win7���Ժ�
		OSVERSIONINFOEXW ovi;
		ZeroMemory(&ovi, sizeof(OSVERSIONINFOEXW));
		if (!GetVersionEx2((LPOSVERSIONINFOW)&ovi)) return FALSE;
		osvi.dwMajorVersion = ovi.dwMajorVersion;
		osvi.dwMinorVersion = ovi.dwMinorVersion;
		osvi.dwBuildNumber = ovi.dwBuildNumber;
		_itow_s(osvi.dwMajorVersion, tmp, 10);
		wcscpy(pszOS, tmp); wcscat(pszOS, L".");
		_itow_s(osvi.dwMinorVersion, tmp, 10);//ƴ�Ӱ汾��
		wcscat(pszOS, tmp); wcscat(pszOS, L".");
		_itow_s(osvi.dwBuildNumber, tmp, 10);
		wcscat(pszOS, tmp);
	}
	return true;
}


BOOL AdjustPrivileges(const wchar_t* lpName)
{//����Ȩ��������ȡָ����Ȩ��
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp = { 0 };
	TOKEN_PRIVILEGES oldtp = { 0 };
	DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
	LUID luid = { 0 };

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
			return TRUE;
		else
			return FALSE;
	}
	if (!LookupPrivilegeValueW(NULL, lpName, &luid)) {
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		return FALSE;
	}

	// close handles
	CloseHandle(hToken);
	return TRUE;
}
wchar_t* wip;
__forceinline wchar_t* CheckIP()//ȡ������ip��ַ  
{
	WSADATA wsaData;
	char name[155];
	char* ip;
	wip=new wchar_t[60];
	PHOSTENT hostinfo;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{//����ԭ��̫���������ٿ�
		if (gethostname(name, sizeof(name)) == 0)
			if ((hostinfo = gethostbyname(name)) != NULL)
			{//wip��ip��ַ�ַ���
				ip = inet_ntoa(*(struct in_addr*) * hostinfo->h_addr_list);
				charTowchar(ip, wip, sizeof(wip) * 60);
			}//����
		WSACleanup();
	}
	return wip;
}

BOOL TakeOwner(LPWSTR FilePath)
{//��ȡָ���ļ�������Ȩ����ɾ��sethc��ʱҪ��
	CHAR UserName[36];
	DWORD cbUserName = sizeof(UserName);
	CHAR Sid[1024] = { 0 };
	DWORD cbSid = sizeof(Sid);
	CHAR DomainBuffer[128] = { 0 };
	DWORD cbDomainBuffer = sizeof(DomainBuffer);
#pragma warning (disable:26812)
	SID_NAME_USE eUse;
#pragma warning (default:26812)
	PACL Dacl = NULL, OldDacl = NULL;
	EXPLICIT_ACCESS Ea;
	BOOL Ret = FALSE;

	if (AdjustPrivileges(SE_TAKE_OWNERSHIP_NAME) && AdjustPrivileges(SE_RESTORE_NAME))
	{//Ҫ������SE_TAKE_OWNERSHIP��Ȩ
		GetUserNameA(UserName, &cbUserName);
		if (LookupAccountNameA(NULL, UserName, &Sid, &cbSid, DomainBuffer, &cbDomainBuffer, &eUse))
		{
			ZeroMemory(&Ea, sizeof(EXPLICIT_ACCESS));
			wchar_t x[] = L"everyone";
			BuildExplicitAccessWithName(&Ea, x, GENERIC_ALL, GRANT_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
			if (SetEntriesInAclW(1, &Ea, OldDacl, &Dacl) == ERROR_SUCCESS)
			{//ֱ�Ӳ����ƾɵ�ace,��Ȼ�޷�ȥ���ļ��ľܾ�Ȩ
				SetNamedSecurityInfoW(FilePath, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &Sid, NULL, NULL, NULL);
				if (SetNamedSecurityInfoW(FilePath, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &Sid, NULL, Dacl, NULL) == ERROR_SUCCESS)
				{
					Ret = TRUE;
				}
			}

		}
	}
	return Ret;
}
BOOL RunEXE(wchar_t* CmdLine, DWORD flag, STARTUPINFO* si)
{//��CreateProcess����������
	STARTUPINFO s = { 0 };
	if (si == nullptr)si = &s;
	PROCESS_INFORMATION pi = { 0 };
	return CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, flag, NULL, NULL, si, &pi);
}

ATOM MyRegisterClass(HINSTANCE h, WNDPROC proc, LPCWSTR ClassName)
{//��װ����ע��Class����.
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = proc;
	wcex.hInstance = h;
	wcex.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_GUI));//���������֧���Զ��崰��ͼ��
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	wcex.lpszClassName = ClassName;//�Զ���ClassName��WndProc
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//Сͼ��
	return RegisterClassExW(&wcex);
}

BOOL RunWithAdmin(wchar_t* path)//�Թ���Ա�������һ������.
{
	SHELLEXECUTEINFO info = { 0 };
	info.cbSize = sizeof(info);
	info.lpFile = path;
	info.lpVerb = L"runas";
	info.nShow = SW_SHOWNORMAL;
	return ShellExecuteEx(&info);
}

bool MyGetInfo(__out LPSYSTEM_INFO lpSystemInfo)//�õ���������
{
	if (NULL == lpSystemInfo)return false;
	typedef void(WINAPI* LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	HMODULE Handle = GetModuleHandle(L"Kernel32");
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = 0;
	if (Handle != NULL)fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(Handle, "GetNativeSystemInfo");
	if (fnGetNativeSystemInfo != NULL)fnGetNativeSystemInfo(lpSystemInfo); else GetSystemInfo(lpSystemInfo);
	return true;
}

#define ProcessBasicInformation 0  
typedef struct
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}   PROCESS_BASIC_INFORMATION;
typedef LONG(__stdcall* PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);


DWORD GetParentProcessID(DWORD dwProcessId)
{//��ȡ�����̵�PID
	LONG status;
	DWORD dwParentPID = 0;
	HANDLE hProcess;
	PROCESS_BASIC_INFORMATION pbi;

	PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
		GetModuleHandle(L"ntdll"), "NtQueryInformationProcess");

	if (NULL == NtQueryInformationProcess)return 0;
	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
	if (!hProcess)return 0;

	// Retrieve information
	status = NtQueryInformationProcess(hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL);
	// Copy parent Id on success
	if (!status)dwParentPID = pbi.InheritedFromUniqueProcessId;
	CloseHandle(hProcess);
	return dwParentPID;
}

int CaptureImage()
{//��ͼ������C:\SAtemp��
	HDC hdcScreen;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi = { 0 };
	DWORD dwBmpSize;
	HANDLE hDIB;
	CHAR* lpbitmap;
	HANDLE hFile;
	DWORD dwSizeofDIB;
	DWORD dwBytesWritten = 0;
	hdcScreen = GetDC(NULL); // ????DC
	hdcMemDC = CreateCompatibleDC(hdcScreen);
	DEVMODE curDevMod = { 0 };
	curDevMod.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMod);
	int width = curDevMod.dmPelsWidth, height = curDevMod.dmPelsHeight;
	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);

	SelectObject(hdcMemDC, hbmScreen);
	BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
	GetObjectW(hbmScreen, sizeof(BITMAP), &bmpScreen);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char*)GlobalLock(hDIB);

	GetDIBits(hdcScreen, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	hFile = CreateFile(L"C:\\SAtemp\\ScreenShot.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	CloseHandle(hFile);
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	return 0;
}

#pragma warning(disable:4244)
#pragma warning(disable:4312)
void change(void* Src, bool wow)
{//�ļ����knock����
	unsigned int v5, v10;
	BYTE* v6, * v7, v8, * v9, * i;
	HKEY phkResult;//��ֵ�ľ��
	LPCWSTR lpSubKeya;

	phkResult = 0;

	if (wow)RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\WOW6432Node\\TopDomain\\e-Learning Class\\Student", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &phkResult);
	else//����ϵͳλ���򿪼�ֵ���ڵ�Ŀ¼
		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\TopDomain\\e-Learning Class\\Student", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &phkResult);

	v5 = (rand() % 40 + 83) & 0xFFFFFFFC;//����õ����ܺ�����ĳ��ȣ�����ȡ��Ϊ4�ı���
	v6 = (BYTE*)operator new[](v5);//������Ӧ���ڴ�ռ�
	v7 = v6;//v7��ָ��v6��ͷ��ָ��
	if (v5 >> 1)
	{//�ȸ�v6ÿ��λ�϶�д�����ֵ
		lpSubKeya = (LPCWSTR)(v5 >> 1);
		do
		{
			*(WORD*)v7 = rand();
			v7 += 2;
			lpSubKeya = (LPCWSTR)((char*)lpSubKeya - 1);
		} while (lpSubKeya);
	}
	v8 = rand() % (v5 - 68) + 2;//ȡһ����СΪ2 - 57�����ֵ
	*v6 = v8;//v6�ĵ�һλ�����һλ���������ֵ
	v6[v5 - 1] = v8;//�����ֵΪ���ܺ������ֶεĿ�ʼ
	if (Src && wcslen((const wchar_t*)Src))//������벻Ϊ�գ������������ȸ��Ƶ�v8��
		memcpy(&v6[v8], Src, 2 * wcslen((const wchar_t*)Src) + 2);
	else
	{//������Ϊ�գ���v8��v8����һ�ֽڶ���Ϊ��
		v9 = &v6[v8];
		*v9 = 0;
		v9[1] = 0;
	}
	v10 = v5 >> 2;

	for (i = v6; v10; --v10)
	{//��ͷ��β�����ĺ�0x150f0f15���
		*(DWORD*)i ^= 0x150f0f15u;
		i += 4;
	}

	DWORD ret, dwSize = 999, dwType = REG_BINARY; BYTE data[1000];
	ret = RegQueryValueExW(phkResult, L"Knock", 0, &dwType, (LPBYTE)data, &dwSize);//���Զ�ȡ��ֵ
	if (ret == 0)RegSetValueExW(phkResult, L"Knock", 0, 3u, v6, v5);
	ret = RegQueryValueExW(phkResult, L"Knock1", 0, &dwType, (LPBYTE)data, &dwSize);//���Զ�ȡ��ֵ
	if (ret == 0)RegSetValueExW(phkResult, L"Knock1", 0, 3u, v6, v5);
	operator delete[](v6);//Ȼ���赽Knock��

	RegCloseKey(phkResult);
}
#pragma warning(default:4244)
typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
}CLIENT_ID, * PCLIENT_ID;
typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PVOID ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

BOOL ReleaseRes(const wchar_t* strFileName, WORD wResID, const wchar_t* strFileType)
{//�ͷ�ָ����Դ
	if (GetFileAttributes(strFileName) != INVALID_FILE_ATTRIBUTES) { return TRUE; }//��Դ�Ѵ���->�˳�

	DWORD   dwWrite = 0;// ��Դ��С  

	// �����ļ�  
	HANDLE  hFile = CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)return FALSE;//����ʧ��->�˳�

	// ������Դ�ļ��С�������Դ���ڴ桢�õ���Դ��С  
	HRSRC   hrsc = FindResource(NULL, MAKEINTRESOURCE(wResID), strFileType);
	HGLOBAL hG = LoadResource(NULL, hrsc);
	DWORD   dwSize = SizeofResource(NULL, hrsc);

	// д���ļ�  
	WriteFile(hFile, hG, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);
	return TRUE;
}


//�ı������Kprocesshacker����ͨ�ŵ�һ�δ���


BOOL LoadNTDriver(LPCWSTR lpszDriverName, LPCWSTR lpszDriverPath)
{
	wchar_t szDriverImagePath[256];
	//�õ�����������·��
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK = NULL;//NT��������ķ�����

								 //�򿪷�����ƹ�����
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		bRet = FALSE;
		goto BeforeLeave;
	}

	//������������Ӧ�ķ���
	hServiceDDK = CreateService(hServiceMgr,
		lpszDriverName, //�����������ע����е�����  
		lpszDriverName, // ע������������ DisplayName ֵ  
		SERVICE_ALL_ACCESS, // ������������ķ���Ȩ��  
		SERVICE_KERNEL_DRIVER,// ��ʾ���صķ�������������  
		SERVICE_DEMAND_START, // ע������������ Start ֵ  
		SERVICE_ERROR_IGNORE, // ע������������ ErrorControl ֵ  
		szDriverImagePath, // ע������������ ImagePath ֵ  
		NULL, NULL, NULL, NULL, NULL);

	DWORD dwRtn;

	//�жϷ����Ƿ�ʧ��
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			bRet = FALSE;
			goto BeforeLeave;
		}
		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			bRet = FALSE;
			goto BeforeLeave;
		}
	}

	//�����������
	bRet = StartService(hServiceDDK, NULL, NULL);
	if (!bRet)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				bRet = FALSE;
				goto BeforeLeave;
			}
			else
			{
				bRet = TRUE;
				goto BeforeLeave;
			}
		}
	}
	bRet = TRUE;
	//�뿪ǰ�رվ��
BeforeLeave:
	if (hServiceDDK)CloseServiceHandle(hServiceDDK);
	if (hServiceMgr)CloseServiceHandle(hServiceMgr);
	return bRet;
}

//ж����������  
BOOL UnloadNTDriver(LPCWSTR szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK = NULL;//NT��������ķ�����
	SERVICE_STATUS SvrSta;
	//��SCM������
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		bRet = FALSE;
		goto BeforeLeave;
	}
	//����������Ӧ�ķ���
	hServiceDDK = OpenService(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);

	if (hServiceDDK == NULL)
	{
		bRet = FALSE;
		goto BeforeLeave;
	}
	//ֹͣ�����������ֹͣʧ�ܣ�ֻ�������������ܣ��ٶ�̬���ء�  
	ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta);
	//��̬ж����������  
	DeleteService(hServiceDDK);
	bRet = TRUE;
BeforeLeave:
	//�뿪ǰ�رմ򿪵ľ��
	if (hServiceDDK)CloseServiceHandle(hServiceDDK);
	if (hServiceMgr)CloseServiceHandle(hServiceMgr);
	return bRet;
}

typedef _Success_(return >= 0) LONG NTSTATUS;
typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef  DWORD(__stdcall* NTOPENFILE)(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG ShareAccess,
	_In_ ULONG OpenOptions
	);

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	_Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef const UNICODE_STRING* PCUNICODE_STRING;


FORCEINLINE VOID RtlInitUnicodeString(
	_Out_ PUNICODE_STRING DestinationString,
	_In_opt_ const wchar_t* SourceString
)
{
	if (SourceString)
		DestinationString->MaximumLength = (DestinationString->Length = (USHORT)(wcslen(SourceString) * sizeof(WCHAR))) + sizeof(WCHAR);
	else
		DestinationString->MaximumLength = DestinationString->Length = 0;

	DestinationString->Buffer = (PWCH)SourceString;
}

#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; \
    }

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define KPH_DEVICE_TYPE 0x9999
#define KPH_CTL_CODE(x) CTL_CODE(KPH_DEVICE_TYPE, 0x800 + x,3,0)
#define KPH_OPENPROCESS KPH_CTL_CODE(50)
#define KPH_TERMINATEPROCESS KPH_CTL_CODE(55)
typedef VOID(NTAPI* PIO_APC_ROUTINE)(
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	);
NTSTATUS KphpDeviceIoControl(
	_In_ ULONG KphControlCode,
	_In_ PVOID InBuffer,
	_In_ ULONG InBufferLength,
	_In_ HANDLE PhKphHandle
)
{
	DWORD a;
	DeviceIoControl(PhKphHandle, KphControlCode, InBuffer, InBufferLength, nullptr, sizeof(DWORD), &a, nullptr);
	return GetLastError();
}

NTSTATUS KphOpenProcess(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ PCLIENT_ID ClientId,
	_In_ HANDLE PhKphHandle
)
{
	struct
	{
		PHANDLE ProcessHandle;
		ACCESS_MASK DesiredAccess;
		PCLIENT_ID ClientId;
	} input = { ProcessHandle, DesiredAccess, ClientId };

	return KphpDeviceIoControl(
		(ULONG)KPH_OPENPROCESS,
		&input,
		sizeof(input),
		PhKphHandle
	);
}
NTSTATUS PhOpenProcess(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ HANDLE ProcessId,
	_In_ HANDLE PhKphHandle
)
{
	//OBJECT_ATTRIBUTES objectAttributes;
	CLIENT_ID clientId;

	clientId.UniqueProcess = ProcessId;
	clientId.UniqueThread = NULL;
	return KphOpenProcess(
		ProcessHandle,
		DesiredAccess,
		&clientId,
		PhKphHandle
	);
}

NTSTATUS PhTerminateProcess(
	_In_ HANDLE ProcessHandle,
	_In_ NTSTATUS ExitStatus,
	_In_ HANDLE PhKphHandle
)
{
	NTSTATUS status;
	struct
	{
		HANDLE ProcessHandle;
		NTSTATUS ExitStatus;
	} input = { ProcessHandle, ExitStatus };

	status = KphpDeviceIoControl(
		(ULONG)KPH_TERMINATEPROCESS,
		&input,
		sizeof(input),
		PhKphHandle
	);
	return status;
}

NTSTATUS KphConnect(PHANDLE handle)
{
	OBJECT_ATTRIBUTES objectAttributes;
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)
		return FALSE;
	NTOPENFILE myopen = (NTOPENFILE)GetProcAddress(hModule, "NtOpenFile");
	UNICODE_STRING on;
	RtlInitUnicodeString(&on, L"\\Device\\KProcessHacker2");
	InitializeObjectAttributes(
		&objectAttributes,
		&on,
		0x00000040,
		NULL,
		NULL
	);
	IO_STATUS_BLOCK isb;
	return myopen(
		handle,
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,
		&objectAttributes,
		&isb,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0x00000040
	);
}