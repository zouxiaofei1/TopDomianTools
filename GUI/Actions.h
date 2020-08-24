//Ϊ�˼���GUI.cpp����
//һ���ֺ���������Class��ȫ�ֱ���û�й�ϵ��������������
//by zouxiaofei1 2015 - 2020

#pragma once
#include "stdafx.h"
#include "GUI.h"
#pragma warning(disable:4996)
VOID RestartDirect();
VOID LockCursor();
DWORD WINAPI RestartThread(LPVOID pM);
#define SE_SHUTDOWN_PRIVILEGE  0x13

constexpr unsigned int Hash(const wchar_t* str)//��ȡһ���ַ�����"ɢ��ֵ"
{
	constexpr unsigned int seed = 131;
	unsigned int hash = 0;

	while (*str)hash = hash * seed + (*str++);

	return (hash & 0x7FFFFFFF);
}
const unsigned int Hash2(const wchar_t* str)//��ȡһ���ַ�����"ɢ��ֵ"
{
	constexpr unsigned int seed = 131;
	unsigned int hash = 0;

	while (*str)hash = hash * seed + (*str++);

	return (hash & 0x7FFFFFFF);
}

VOID LockCursor()//��ס���
{
	RECT rect;
	rect.bottom = rect.top = GetSystemMetrics(SM_CYSCREEN);
	rect.right = rect.left = GetSystemMetrics(SM_CXSCREEN);//��������������½�
	ClipCursor(&rect);//���ǰ�windows������ctrl+alt+del�ͻ�ָ�
}

__forceinline VOID RestartDirect()//���������������߳�
{
	CreateThread(0, 0, RestartThread, 0, 0, 0);
}

DWORD WINAPI  RestartThread(LPVOID pM)//��������
{//��Ӧֱ�ӵ��ã��������̻߳Ῠһ���
	(pM);
	typedef int(__stdcall* PFN_RtlAdjustPrivilege)(int, BOOL, BOOL, int*);
	typedef int(__stdcall* PFN_ZwShutdownSystem)(int);
	HMODULE hModule = ::LoadLibrary(L"ntdll.dll");
	if (hModule != NULL)
	{//Ѱ��ZwShutdownSystem�ĵ�ַ
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress(hModule, "RtlAdjustPrivilege"); PFN_ZwShutdownSystem
			pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress(hModule, "ZwShutdownSystem");
		if (pfnRtl != NULL && pfnShutdown != NULL)//
		{//����SE_SHUTDOWN_PRIVILEGE��Ȩ��
			int en = 0;//��Ȥ�������Ȩ���ڷǹ���Ա�¾��ܵõ�
			int nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
			if (nRet == 0x0C000007C) nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en);
			nRet = pfnShutdown(1);//SH_SHUTDOWN = 0; //SH_RESTART = 1; //SH_POWEROFF = 2;
		}
	}
	return 0;
}

BOOL MyRemoveDirectory(wchar_t* FilePath)//����RemoveDirectory��bug,�޷�ɾ��ĳЩ���������ַ����ļ���
{//ʹ��AutoDeleteִ����Ϻ󣬿��ܻ��������յ�Ŀ¼�޷�ɾ�������ﳢ�Ե���SHFileOperationɾ������
	size_t len = mywcslen(FilePath);
	FilePath[len + 1] = FilePath[len + 1] = 0;//SHFileOperation��һ��bug,�ļ�Ŀ¼����� 2 ���ַ���������NULL
	SHFILEOPSTRUCT FileOp = { 0 };
	FileOp.fFlags = FOF_NO_UI;
	FileOp.pFrom = FilePath;
	FileOp.pTo = NULL; //һ��Ҫ��NULL
	FileOp.wFunc = FO_DELETE; //ɾ������
	return SHFileOperation(&FileOp) == 0;//�ɹ�����0
}

BOOL GetVersionEx2(LPOSVERSIONINFOW lpVersionInformation)
{//���ڲ�һ��ĺ�����ȡϵͳ�汾
	HMODULE hNtDll = GetModuleHandleW(L"NTDLL");

	typedef int(__stdcall* tRtlGetVersion)(PRTL_OSVERSIONINFOW);
	tRtlGetVersion pRtlGetVersion = nullptr;
	if (hNtDll)pRtlGetVersion = (tRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
	if (pRtlGetVersion)return pRtlGetVersion((PRTL_OSVERSIONINFOW)lpVersionInformation) >= 0;
	return FALSE;
}

#pragma warning(disable:28159)//��������������ʹ��IsWindows7OrGreater()����ĺ���
//�������ֺ���ֻ֧��win7�����ϵ�ϵͳ
//�������ǾͲ���ȡ���������= =


BOOL GetOSDisplayString(wchar_t* pszOS)
{//��ȡϵͳ�汾�ĺ���
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));//GetVersionEx�����жϰ汾Ч��һ�㲻��
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);//�����������ж�һ̨win10����
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);//�õ��Ľ������6.2(9200)
	if (!bOsVersionInfoEx) return FALSE;//������ȷ��ʾ��ϸ�汾��

	wchar_t tmp[MAX_NUM] = { 0 };//(��������win7��ǰ��ϵͳ��GetVersionExû������)
	myitow(osvi.dwMajorVersion, tmp, MAX_NUM);//��汾��
	mywcscpy(pszOS, tmp); mywcscat(pszOS, L".");
	myitow(osvi.dwMinorVersion, tmp, MAX_NUM);//С�汾��
	mywcscat(pszOS, tmp); mywcscat(pszOS, L".");
	myitow(osvi.dwBuildNumber, tmp, MAX_NUM);//build
	mywcscat(pszOS, tmp);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion >= 6)
	{//win7���Ժ���ʹ��RtlGetVersion����
		OSVERSIONINFOEXW ovi;
		ZeroMemory(&ovi, sizeof(OSVERSIONINFOEXW));
		if (!GetVersionEx2((LPOSVERSIONINFOW)&ovi)) return FALSE;
		osvi.dwMajorVersion = ovi.dwMajorVersion;
		osvi.dwMinorVersion = ovi.dwMinorVersion;
		osvi.dwBuildNumber = ovi.dwBuildNumber;
		myitow(osvi.dwMajorVersion, tmp, MAX_NUM);
		mywcscpy(pszOS, tmp); mywcscat(pszOS, L".");
		myitow(osvi.dwMinorVersion, tmp, MAX_NUM);//ƴ�Ӱ汾��
		mywcscat(pszOS, tmp); mywcscat(pszOS, L".");
		myitow(osvi.dwBuildNumber, tmp, MAX_NUM);
		mywcscat(pszOS, tmp);
	}
	return true;
}
#pragma warning(default:28159)//�ָ�����

bool EnablePrivilege(LPCWSTR privilegeStr, HANDLE hToken);

__forceinline void CheckIP(wchar_t* a)//ȡ������ip��ַ  
{
	WSADATA wsaData = {0};
	char name[155] = {0};
	char* ip;
	wchar_t wip[20] = { 0 };
	PHOSTENT hostinfo;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{//����ԭ��̫���������ٿ�
		if (gethostname(name, sizeof(name)) == 0)
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
				MultiByteToWideChar(CP_ACP, 0, ip, -1, wip, 20);
			}//����
		WSACleanup();
	}
	mywcscat(a, wip);
}

BOOL TakeOwner(LPCWSTR FilePath)
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

	if (EnablePrivilege(SE_TAKE_OWNERSHIP_NAME, 0) && EnablePrivilege(SE_RESTORE_NAME, 0))
	{//Ҫ������SE_TAKE_OWNERSHIP��Ȩ
		GetUserNameA(UserName, &cbUserName);
		if (LookupAccountNameA(NULL, UserName, &Sid, &cbSid, DomainBuffer, &cbDomainBuffer, &eUse))
		{
			ZeroMemory(&Ea, sizeof(EXPLICIT_ACCESS));
			wchar_t tmp[] = L"everyone";
			BuildExplicitAccessWithName(&Ea, tmp, GENERIC_ALL, GRANT_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
			if (SetEntriesInAclW(1, &Ea, OldDacl, &Dacl) == ERROR_SUCCESS)
			{//ֱ�Ӳ����ƾɵ�ace,��Ȼ�޷�ȥ���ļ��ľܾ�Ȩ
				SetNamedSecurityInfoW((LPWSTR)FilePath, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &Sid, NULL, NULL, NULL);
				if (SetNamedSecurityInfoW((LPWSTR)FilePath, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &Sid, NULL, Dacl, NULL) == ERROR_SUCCESS)
					Ret = TRUE;
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
	const bool f = CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, flag, NULL, NULL, si, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return f;
}

ATOM MyRegisterClass(HINSTANCE h, WNDPROC proc, LPCWSTR ClassName, unsigned int style)
{//��װ����ע��Class����.
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = style;
	wcex.lpfnWndProc = proc;
	wcex.hInstance = h;
	wcex.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_GUI));//���������֧���Զ��崰��ͼ��
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
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
	LONG status = 0;
	DWORD dwParentPID = 0;
	HANDLE hProcess = 0;
	PROCESS_BASIC_INFORMATION pbi;

	HMODULE hm = GetModuleHandle(L"ntdll");
	PROCNTQSIP NtQueryInformationProcess = 0;
	if (hm)NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(hm, "NtQueryInformationProcess");

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
	HDC hdcScreen, hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi = { 0 };
	DWORD dwBmpSize;
	HANDLE hDIB;
	CHAR* lpbitmap;
	HANDLE hFile;
	DWORD dwSizeofDIB, dwBytesWritten = 0;
	hdcScreen = GetDC(NULL); // ��ȡ����DC
	hdcMemDC = CreateCompatibleDC(hdcScreen);
	DEVMODE curDevMod = { 0 };
	curDevMod.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMod);
	const int width = curDevMod.dmPelsWidth, height = curDevMod.dmPelsHeight;
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
	if (!hDIB)return 0;
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

__forceinline void SetProcessAware()
{
	typedef DWORD(CALLBACK* SEtProcessDPIAware)(void);
	SEtProcessDPIAware SetProcessDPIAware = 0;
	HMODULE huser = LoadLibrary(L"user32.dll");//��ʾ����ʱ��Ҫ�ر�ϵͳ��DPI����
	if (huser)SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
	if (SetProcessDPIAware != NULL)SetProcessDPIAware();
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
	if (Src && mywcslen((const wchar_t*)Src))//������벻Ϊ�գ������������ȸ��Ƶ�v8��
		mymemcpy(&v6[v8], Src, 2 * mywcslen((const wchar_t*)Src) + 2);
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
	if (!hrsc)return FALSE;
	HGLOBAL hG = LoadResource(NULL, hrsc);
	const DWORD dwSize = SizeofResource(NULL, hrsc);

	// д���ļ�  
	WriteFile(hFile, hG, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);
	return TRUE;
}

void LoadPicture(const wchar_t* lpFilePath, HDC hdc, int startx, int starty, float DPI)
{
	// �ļ����   
	HANDLE FileHandle;
	// ��λ���ݡ���λ����   
	DWORD SizeH, SizeL, ReadCount;

	IStream* pstream = NULL;
	IPicture* pPic = NULL;
	// �Զ��ķ�ʽ��ͼ��   
	FileHandle = CreateFile(lpFilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL
	);
	// ��ʧ��   
	if (FileHandle == INVALID_HANDLE_VALUE)
		return;
	// ��ȡͼ���ļ���С   
	SizeL = GetFileSize(FileHandle, &SizeH);
	// Ϊͼ���ļ�   
	// ����һ�����ƶ���ȫ�ֵĶ��ڴ�   
	HGLOBAL pBuffer = GlobalAlloc(GMEM_MOVEABLE, SizeL);
	// ������ڴ�ʧ��   
	if (pBuffer == NULL)
	{
		CloseHandle(FileHandle);
		return;
	}
	// ָ����ڴ��ָ��ת��Ϊͨ�õ�ָ�������൱�ڽ���   
	LPVOID pDes = GlobalLock(pBuffer);

	// �������ݽ����ڴ�   
	if (ReadFile(FileHandle, pDes, SizeL, &ReadCount, NULL) == 0)
	{
		CloseHandle(FileHandle);
		GlobalUnlock(pBuffer);
		DeleteObject(pBuffer);
		return;
	}

	// ���ڴ�����   
	GlobalUnlock(pBuffer);

	// ��ȫ�ִ洢���Ķ��з���һ��������   
	if (CreateStreamOnHGlobal(pBuffer, true, &pstream) != S_OK)
	{
		CloseHandle(FileHandle);
		DeleteObject(pBuffer);
		return;
	}

	// ����һ���µ�ͼ�񲢳�ʼ��   
	if (!SUCCEEDED(OleLoadPicture(pstream, SizeL, true, IID_IPicture, (void**)&pPic)))return;
	long hmWidth;
	long hmHeight;
	// ��IPicture�л�ȡ�߶�����   
	pPic->get_Width(&hmWidth);
	pPic->get_Height(&hmHeight);
	const int nWidth = MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), 2540);
	const int nHeight = MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), 2540);

	pPic->Render(hdc, (int)(startx * DPI), (int)(starty * DPI), (int)(DPI * nWidth), (int)(DPI * nHeight), 0, hmHeight, hmWidth, -hmHeight, NULL);

	pPic->Release();
	pstream->Release();
	//ReleaseDC(hwnd, hdc);
	CloseHandle(FileHandle);
}

void ReleaseLanguageFiles(const wchar_t* Path, int tag, wchar_t* str)//tag �� str �ɲ���
{//��tag��1��2ʱ�����������ļ�·��������str��
	wchar_t LanguagePath[MAX_PATH] = { 0 };//���ͷ��Դ���������Ӣ�������ļ�
	mywcscpy(LanguagePath, Path);
	mywcscat(LanguagePath, L"language\\");//����languageĿ¼
	CreateDirectory(LanguagePath, NULL);
	mywcscat(LanguagePath, L"Chinese.ini");
	ReleaseRes(LanguagePath, FILE_CHN, L"JPG");
	if (tag == 1)mywcscpy(str, LanguagePath);
	mywcscpy(LanguagePath, Path);
	mywcscat(LanguagePath, L"language\\EnglishANSI.ini");
	ReleaseRes(LanguagePath, FILE_ENG, L"JPG");
	if (tag == 2)mywcscpy(str, LanguagePath);
}