//为了减短GUI.cpp长度
//一部分和主程序关系稍小的，有实际作用的函数被放在这里

#pragma once
#include "stdafx.h"
//因某种原因被注释掉的lib
//#pragma comment(lib, "User32.lib")
//#pragma comment(lib, "Advapi32.lib")
void s(int cathy);
void s(LPCWSTR cathy);
VOID GetInfo(__out LPSYSTEM_INFO lpSystemInfo);
#pragma warning(disable:4996)
VOID RestartDirect();
VOID LockCursor();
DWORD WINAPI RestartThread(LPVOID pM);

VOID LockCursor()//锁住鼠标
{
	RECT rect;
	rect.bottom = rect.top = GetSystemMetrics(SM_CYSCREEN);
	rect.right = rect.left = GetSystemMetrics(SM_CXSCREEN);//把鼠标限制在右下角
	ClipCursor(&rect);//但是按windows键或者ctrl+alt+del就会恢复
}
VOID Restart()//瞬间重启
{
	HDESK VirtualDesk = CreateDesktop(L"GUI", NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);
	SwitchDesktop(VirtualDesk);
	LockCursor();
	RestartDirect();
}
inline VOID RestartDirect()
{//创建线程
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
	{//寻找ZwShutdownSystem的地址
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress(hModule, "RtlAdjustPrivilege"); PFN_ZwShutdownSystem
			pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress(hModule, "ZwShutdownSystem");
		if (pfnRtl != NULL && pfnShutdown != NULL)//
		{//申请SE_SHUTDOWN_PRIVILEGE提权？
			int en = 0;//有趣的是这个权限在非管理员下就能得到
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
{//用内部一点的函数获取系统版本
	HMODULE hNtDll = GetModuleHandleW(L"NTDLL");

	typedef int(__stdcall* tRtlGetVersion)(PRTL_OSVERSIONINFOW);
	tRtlGetVersion pRtlGetVersion = nullptr;
	if (hNtDll)pRtlGetVersion = (tRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
	if (pRtlGetVersion)return pRtlGetVersion((PRTL_OSVERSIONINFOW)lpVersionInformation) >= 0;
	return FALSE;
}

BOOL AdjustPrivileges(const wchar_t* lpName)
{//输入权限名，获取指定的权限
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



BOOL TakeOwner(LPWSTR FilePath)
{//获取指定文件的所有权，在删除sethc等时要用
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
	PSECURITY_DESCRIPTOR Sd = NULL;
	BOOL Ret = FALSE;

	if (AdjustPrivileges(SE_TAKE_OWNERSHIP_NAME) && AdjustPrivileges(SE_RESTORE_NAME))
	{//要先申请SE_TAKE_OWNERSHIP特权
		GetUserNameA(UserName, &cbUserName);
		if (LookupAccountNameA(NULL, UserName, &Sid, &cbSid, DomainBuffer, &cbDomainBuffer, &eUse))
		{

			ZeroMemory(&Ea, sizeof(EXPLICIT_ACCESS));
			GetNamedSecurityInfoW(FilePath, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &OldDacl, NULL, &Sd);
			wchar_t x[] = L"everyone";
			BuildExplicitAccessWithName(&Ea, x, GENERIC_ALL, GRANT_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
			if (SetEntriesInAclW(1, &Ea, OldDacl, &Dacl) == ERROR_SUCCESS)
			{
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
bool RunEXE(wchar_t*CmdLine, DWORD flag, STARTUPINFO* si)
{//用CreateProcess来创建进程
	STARTUPINFO s = { 0 };
	if (si == nullptr)si = &s;
	PROCESS_INFORMATION pi = { 0 };
	return CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, flag, NULL, NULL, si, &pi);
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
{//获取父进程的PID
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
{//截图并放在C:\SAtemp内
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

void change(void* Src, bool wow)
{//改极域的knock密码
	unsigned int v5, v10;
	BYTE* v6, * v7, v8, * v9, * i;
	HKEY phkResult;
	LPCWSTR lpSubKeya;

	phkResult = 0;

	if (wow)RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\WOW6432Node\\TopDomain\\e-Learning Class\\Student", 0, 0x20006u, &phkResult);
	else
		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\TopDomain\\e-Learning Class\\Student", 0, 0x20006u, &phkResult);

	v5 = (rand() % 40 + 83) & 0xFFFFFFFC;
	v6 = (BYTE*)operator new[](v5);
	v7 = v6;
	if (v5 >> 1)
	{
		lpSubKeya = (LPCWSTR)(v5 >> 1);
		do
		{
			*(WORD*)v7 = rand();
			v7 += 2;
			lpSubKeya = (LPCWSTR)((char*)lpSubKeya - 1);
		} while (lpSubKeya);
	}
	v8 = rand() % (v5 - 68) + 2;
	*v6 = v8;
	v6[v5 - 1] = v8;
	if (Src && wcslen((const wchar_t*)Src))
		memcpy(&v6[v8], Src, 2 * wcslen((const wchar_t*)Src) + 2);
	else
	{
		v9 = &v6[v8];
		*v9 = 0;
		v9[1] = 0;
	}
	v10 = v5 >> 2;

	for (i = v6; v10; --v10)
	{
		*(DWORD*)i ^= 0x150f0f15u;
		i += 4;
	}

	RegSetValueExW(phkResult, L"Knock", 0, 3u, v6, v5);
	RegSetValueExW(phkResult, L"Knock1", 0, 3u, v6, v5);
	operator delete[](v6);

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
{//释放指定资源
	if (GetFileAttributes(strFileName) != INVALID_FILE_ATTRIBUTES) { return TRUE; }//资源已存在->退出

	DWORD   dwWrite = 0;// 资源大小  

	// 创建文件  
	HANDLE  hFile = CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)return FALSE;//创建失败->退出

	// 查找资源文件中、加载资源到内存、得到资源大小  
	HRSRC   hrsc = FindResource(NULL, MAKEINTRESOURCE(wResID), strFileType);
	HGLOBAL hG = LoadResource(NULL, hrsc);
	DWORD   dwSize = SizeofResource(NULL, hrsc);

	// 写入文件  
	WriteFile(hFile, hG, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);
	return TRUE;
}

//改编过的与Kprocesshacker驱动通信的一段代码
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