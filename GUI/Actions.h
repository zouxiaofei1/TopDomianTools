//为了减短GUI.cpp长度
//一部分和主程序中Class、全局变量没有关系函数被放在这里

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

VOID LockCursor()//锁住鼠标
{
	RECT rect;
	rect.bottom = rect.top = GetSystemMetrics(SM_CYSCREEN);
	rect.right = rect.left = GetSystemMetrics(SM_CXSCREEN);//把鼠标限制在右下角
	ClipCursor(&rect);//但是按windows键或者ctrl+alt+del就会恢复
}

__forceinline VOID RestartDirect()
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

BOOL GetOSDisplayString(wchar_t* pszOS)
{//获取系统版本的函数
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));//据说GetVersionEx用来判断版本效果不好
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);//用它来判断一台win10电脑
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);//得到的结果一般是6.2(9200)
	if (!bOsVersionInfoEx) return FALSE;//不能正确显示详细版本号

	wchar_t tmp[101];//对于win7以前的系统用GetVersionEx没有问题
	_itow_s(osvi.dwMajorVersion, tmp, 10);//大版本号
	wcscpy(pszOS, tmp); wcscat(pszOS, L".");
	_itow_s(osvi.dwMinorVersion, tmp, 10);//小版本号
	wcscat(pszOS, tmp); wcscat(pszOS, L".");
	_itow_s(osvi.dwBuildNumber, tmp, 10);//build
	wcscat(pszOS, tmp);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion >= 6)
	{//win7及以后
		OSVERSIONINFOEXW ovi;
		ZeroMemory(&ovi, sizeof(OSVERSIONINFOEXW));
		if (!GetVersionEx2((LPOSVERSIONINFOW)&ovi)) return FALSE;
		osvi.dwMajorVersion = ovi.dwMajorVersion;
		osvi.dwMinorVersion = ovi.dwMinorVersion;
		osvi.dwBuildNumber = ovi.dwBuildNumber;
		_itow_s(osvi.dwMajorVersion, tmp, 10);
		wcscpy(pszOS, tmp); wcscat(pszOS, L".");
		_itow_s(osvi.dwMinorVersion, tmp, 10);//拼接版本号
		wcscat(pszOS, tmp); wcscat(pszOS, L".");
		_itow_s(osvi.dwBuildNumber, tmp, 10);
		wcscat(pszOS, tmp);
	}
	return true;
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
wchar_t* wip;
__forceinline wchar_t* CheckIP()//取本机的ip地址  
{
	WSADATA wsaData;
	char name[155];
	char* ip;
	wip=new wchar_t[60];
	PHOSTENT hostinfo;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{//具体原理不太清楚，详见百科
		if (gethostname(name, sizeof(name)) == 0)
			if ((hostinfo = gethostbyname(name)) != NULL)
			{//wip存ip地址字符串
				ip = inet_ntoa(*(struct in_addr*) * hostinfo->h_addr_list);
				charTowchar(ip, wip, sizeof(wip) * 60);
			}//清理
		WSACleanup();
	}
	return wip;
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
	BOOL Ret = FALSE;

	if (AdjustPrivileges(SE_TAKE_OWNERSHIP_NAME) && AdjustPrivileges(SE_RESTORE_NAME))
	{//要先申请SE_TAKE_OWNERSHIP特权
		GetUserNameA(UserName, &cbUserName);
		if (LookupAccountNameA(NULL, UserName, &Sid, &cbSid, DomainBuffer, &cbDomainBuffer, &eUse))
		{
			ZeroMemory(&Ea, sizeof(EXPLICIT_ACCESS));
			wchar_t x[] = L"everyone";
			BuildExplicitAccessWithName(&Ea, x, GENERIC_ALL, GRANT_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
			if (SetEntriesInAclW(1, &Ea, OldDacl, &Dacl) == ERROR_SUCCESS)
			{//直接不复制旧的ace,不然无法去除文件的拒绝权
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
{//用CreateProcess来创建进程
	STARTUPINFO s = { 0 };
	if (si == nullptr)si = &s;
	PROCESS_INFORMATION pi = { 0 };
	return CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, flag, NULL, NULL, si, &pi);
}

ATOM MyRegisterClass(HINSTANCE h, WNDPROC proc, LPCWSTR ClassName)
{//封装过的注册Class函数.
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = proc;
	wcex.hInstance = h;
	wcex.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_GUI));//这个函数不支持自定义窗体图标
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	wcex.lpszClassName = ClassName;//自定义ClassName和WndProc
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标
	return RegisterClassExW(&wcex);
}

BOOL RunWithAdmin(wchar_t* path)//以管理员身份运行一个程序.
{
	SHELLEXECUTEINFO info = { 0 };
	info.cbSize = sizeof(info);
	info.lpFile = path;
	info.lpVerb = L"runas";
	info.nShow = SW_SHOWNORMAL;
	return ShellExecuteEx(&info);
}

bool MyGetInfo(__out LPSYSTEM_INFO lpSystemInfo)//得到环境变量
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
#pragma warning(disable:4312)
void change(void* Src, bool wow)
{//改极域的knock密码
	unsigned int v5, v10;
	BYTE* v6, * v7, v8, * v9, * i;
	HKEY phkResult;//键值的句柄
	LPCWSTR lpSubKeya;

	phkResult = 0;

	if (wow)RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\WOW6432Node\\TopDomain\\e-Learning Class\\Student", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &phkResult);
	else//根据系统位数打开键值所在的目录
		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\TopDomain\\e-Learning Class\\Student", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &phkResult);

	v5 = (rand() % 40 + 83) & 0xFFFFFFFC;//随机得到加密后密码的长度，向下取整为4的倍数
	v6 = (BYTE*)operator new[](v5);//申请相应的内存空间
	v7 = v6;//v7是指向v6开头的指针
	if (v5 >> 1)
	{//先给v6每个位上都写上随机值
		lpSubKeya = (LPCWSTR)(v5 >> 1);
		do
		{
			*(WORD*)v7 = rand();
			v7 += 2;
			lpSubKeya = (LPCWSTR)((char*)lpSubKeya - 1);
		} while (lpSubKeya);
	}
	v8 = rand() % (v5 - 68) + 2;//取一个大小为2 - 57的随机值
	*v6 = v8;//v6的第一位和最后一位都标上这个值
	v6[v5 - 1] = v8;//以这个值为加密后密码字段的开始
	if (Src && wcslen((const wchar_t*)Src))//如果密码不为空，将密码明文先复制到v8处
		memcpy(&v6[v8], Src, 2 * wcslen((const wchar_t*)Src) + 2);
	else
	{//若密码为空，则将v8和v8的下一字节都设为空
		v9 = &v6[v8];
		*v9 = 0;
		v9[1] = 0;
	}
	v10 = v5 >> 2;

	for (i = v6; v10; --v10)
	{//从头到尾把密文和0x150f0f15异或
		*(DWORD*)i ^= 0x150f0f15u;
		i += 4;
	}

	DWORD ret, dwSize = 999, dwType = REG_BINARY; BYTE data[1000];
	ret = RegQueryValueExW(phkResult, L"Knock", 0, &dwType, (LPBYTE)data, &dwSize);//尝试读取键值
	if (ret == 0)RegSetValueExW(phkResult, L"Knock", 0, 3u, v6, v5);
	ret = RegQueryValueExW(phkResult, L"Knock1", 0, &dwType, (LPBYTE)data, &dwSize);//尝试读取键值
	if (ret == 0)RegSetValueExW(phkResult, L"Knock1", 0, 3u, v6, v5);
	operator delete[](v6);//然后设到Knock中

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


BOOL LoadNTDriver(LPCWSTR lpszDriverName, LPCWSTR lpszDriverPath)
{
	wchar_t szDriverImagePath[256];
	//得到完整的驱动路径
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄

								 //打开服务控制管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		bRet = FALSE;
		goto BeforeLeave;
	}

	//创建驱动所对应的服务
	hServiceDDK = CreateService(hServiceMgr,
		lpszDriverName, //驱动程序的在注册表中的名字  
		lpszDriverName, // 注册表驱动程序的 DisplayName 值  
		SERVICE_ALL_ACCESS, // 加载驱动程序的访问权限  
		SERVICE_KERNEL_DRIVER,// 表示加载的服务是驱动程序  
		SERVICE_DEMAND_START, // 注册表驱动程序的 Start 值  
		SERVICE_ERROR_IGNORE, // 注册表驱动程序的 ErrorControl 值  
		szDriverImagePath, // 注册表驱动程序的 ImagePath 值  
		NULL, NULL, NULL, NULL, NULL);

	DWORD dwRtn;

	//判断服务是否失败
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

	//开启此项服务
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
	//离开前关闭句柄
BeforeLeave:
	if (hServiceDDK)CloseServiceHandle(hServiceDDK);
	if (hServiceMgr)CloseServiceHandle(hServiceMgr);
	return bRet;
}

//卸载驱动程序  
BOOL UnloadNTDriver(LPCWSTR szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄
	SERVICE_STATUS SvrSta;
	//打开SCM管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		bRet = FALSE;
		goto BeforeLeave;
	}
	//打开驱动所对应的服务
	hServiceDDK = OpenService(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);

	if (hServiceDDK == NULL)
	{
		bRet = FALSE;
		goto BeforeLeave;
	}
	//停止驱动程序，如果停止失败，只有重新启动才能，再动态加载。  
	ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta);
	//动态卸载驱动程序。  
	DeleteService(hServiceDDK);
	bRet = TRUE;
BeforeLeave:
	//离开前关闭打开的句柄
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