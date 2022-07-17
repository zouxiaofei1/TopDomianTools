﻿//为了减短GUI.cpp长度
//一部分和主程序中Class、全局变量没有关系函数被放在这里
//by zouxiaofei1 2015 - 2021

#pragma once
#include "stdafx.h"
#include "GUI.h"
#pragma warning(disable:4996)
VOID RestartDirect();
VOID LockCursor();
DWORD WINAPI RestartThread(LPVOID pM);
#define SE_SHUTDOWN_PRIVILEGE  0x13

bool Findquotations(wchar_t* zxf, wchar_t zxf2[])//命令行调用找到"双引号"
{
	wchar_t tmp0;
	wchar_t* tmp1 = mywcsstr(zxf, L"\"");
	if (tmp1 == 0)return false;
	wchar_t* tmp2 = mywcsstr(tmp1 + 1, L"\"");
	if (tmp2 == 0)return false;
	tmp0 = *tmp2;
	*tmp2 = 0;
	mywcscpy(zxf2, tmp1 + 1);
	*tmp2 = tmp0;
	return true;
}

const unsigned int Hash(const wchar_t* str)//获取一个字符串的"散列值"
{
	const unsigned int seed = 131;
	unsigned int hash = 0;

	while (*str)hash = hash * seed + (*str++);

	return (hash & 0x7FFFFFFF);
}
VOID LockCursor()//锁住鼠标
{
	RECT rect;
	rect.bottom = rect.top = GetSystemMetrics(SM_CYSCREEN);
	rect.right = rect.left = GetSystemMetrics(SM_CXSCREEN);//把鼠标限制在右下角
	ClipCursor(&rect);//但是按windows键或者ctrl+alt+del就会恢复
}

__forceinline VOID RestartDirect()//创建快速重启的线程
{
	CreateThread(0, 0, RestartThread, 0, 0, 0);
}

DWORD WINAPI  RestartThread(LPVOID pM)//快速重启
{//不应直接调用，否则主线程会卡一会儿
	(pM);
	typedef int(__stdcall* PFN_RtlAdjustPrivilege)(int, BOOL, BOOL, int*);
	typedef int(__stdcall* PFN_ZwShutdownSystem)(int);
	HMODULE hModule = ::LoadLibrary(L"ntdll.dll");
	if (hModule != NULL)
	{//寻找ZwShutdownSystem的地址
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress(hModule, "RtlAdjustPrivilege"); PFN_ZwShutdownSystem
			pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress(hModule, "ZwShutdownSystem");
		if (pfnRtl != NULL && pfnShutdown != NULL)//
		{//申请SE_SHUTDOWN_PRIVILEGE提权？
			int en = 0;//有趣的是这个权限在非管理员下就能得到
			int nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
			if (nRet == 0x0C000007C) nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en);
			nRet = pfnShutdown(1);//SH_SHUTDOWN = 0; //SH_RESTART = 1; //SH_POWEROFF = 2;
		}
	}
	return 0;
}

BOOL MyRemoveDirectory(wchar_t* FilePath)//由于RemoveDirectory的bug,无法删除某些含有特殊字符的文件夹
{//使得AutoDelete执行完毕后，可能会残留少许空的目录无法删除。这里尝试调用SHFileOperation删除它们
	size_t len = mywcslen(FilePath);
	FilePath[len + 1] = FilePath[len + 1] = 0;//SHFileOperation有一个bug,文件目录的最后 2 个字符都必须是NULL
	SHFILEOPSTRUCT FileOp;
	myZeroMemory(&FileOp, sizeof(FileOp));
	FileOp.fFlags = FOF_NO_UI;
	FileOp.pFrom = FilePath;
	FileOp.pTo = NULL; //一定要是NULL
	FileOp.wFunc = FO_DELETE; //删除操作
	return SHFileOperation(&FileOp) == 0;//成功返回0
}

BOOL GetVersionEx2(LPOSVERSIONINFOW lpVersionInformation)
{//用内部一点的函数获取系统版本
	HMODULE hNtDll = GetModuleHandleW(L"NTDLL");

	typedef int(__stdcall* tRtlGetVersion)(PRTL_OSVERSIONINFOW);
	tRtlGetVersion pRtlGetVersion = nullptr;
	if (hNtDll)pRtlGetVersion = (tRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
	if (pRtlGetVersion)return pRtlGetVersion((PRTL_OSVERSIONINFOW)lpVersionInformation) >= 0;
	return FALSE;
}

#pragma warning(disable:28159)//编译器建议我们使用IsWindows7OrGreater()这类的函数
//但是这种函数只支持win7及以上的系统
//所以我们就不听取这个建议了= =


BOOL GetOSDisplayString(wchar_t* pszOS)
{//获取系统版本的函数
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
#ifndef _WIN64
	myZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));//GetVersionEx用来判断版本效果一般不好
#endif
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);//例如用它来判断一台win10电脑
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);//得到的结果总是6.2(9200)
	if (!bOsVersionInfoEx) return FALSE;//不能正确显示详细版本号

	wchar_t tmp[MAX_NUM];//(不过对于win7以前的系统用GetVersionEx没有问题)
	myZeroMemory(tmp, sizeof(wchar_t) * 10);
	myitow(osvi.dwMajorVersion, tmp, MAX_NUM);//大版本号
	mywcscpy(pszOS, tmp); mywcscat(pszOS, L".");
	myitow(osvi.dwMinorVersion, tmp, MAX_NUM);//小版本号
	mywcscat(pszOS, tmp); mywcscat(pszOS, L".");
	myitow(osvi.dwBuildNumber, tmp, MAX_NUM);//build
	mywcscat(pszOS, tmp);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion >= 6)
	{//win7及以后则使用RtlGetVersion更好
		OSVERSIONINFOEXW ovi;
#ifndef _WIN64
		myZeroMemory(&ovi, sizeof(OSVERSIONINFOEXW));
#endif
		ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!GetVersionEx2((LPOSVERSIONINFOW)&ovi)) return FALSE;
		osvi.dwMajorVersion = ovi.dwMajorVersion;
		osvi.dwMinorVersion = ovi.dwMinorVersion;
		osvi.dwBuildNumber = ovi.dwBuildNumber;
		myitow(osvi.dwMajorVersion, tmp, MAX_NUM);
		mywcscpy(pszOS, tmp); mywcscat(pszOS, L".");
		myitow(osvi.dwMinorVersion, tmp, MAX_NUM);//拼接版本号
		mywcscat(pszOS, tmp); mywcscat(pszOS, L".");
		myitow(osvi.dwBuildNumber, tmp, MAX_NUM);
		mywcscat(pszOS, tmp);
	}
	return true;
}
#pragma warning(default:28159)//恢复警告

bool EnablePrivilege(LPCWSTR privilegeStr, HANDLE hToken);

void CheckIP(wchar_t* a)//取本机的ip地址  
{
	WSADATA wsaData;
#ifndef _WIN64
	myZeroMemory(&wsaData, sizeof(WSADATA));
#endif

	char name[156], * ip;
#ifndef _WIN64
	myZeroMemory(name, sizeof(char) * 156);
#else
	* name = 0;
#endif

	wchar_t wip[20];
	myZeroMemory(wip, sizeof(wchar_t) * 20);
	PHOSTENT hostinfo;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{//具体原理不太清楚，详见百科
		if (gethostname(name, sizeof(name)) == 0)
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
				MultiByteToWideChar(CP_ACP, 0, ip, -1, wip, 20);
			}//清理
		WSACleanup();
	}
	mywcscat(a, wip);
}

BOOL TakeOwner(LPCWSTR FilePath)
{//获取指定文件的所有权，在删除sethc等时要用
	CHAR UserName[36];
	//s(0);
	DWORD cbUserName = sizeof(char) * 36;
	//s(1);
	CHAR Sid[1024];
	myZeroMemory(Sid, sizeof(char) * 1024);
	DWORD cbSid = sizeof(char) * 1024;
	CHAR DomainBuffer[128];
	myZeroMemory(DomainBuffer, sizeof(char) * 128);
	DWORD cbDomainBuffer = sizeof(char) * 128;
	SID_NAME_USE eUse;
	wchar_t tmp[9];
	mywcscpy(tmp, L"everyone");
	PACL Dacl = NULL, OldDacl = NULL;
	EXPLICIT_ACCESS Ea;
	BOOL Ret = FALSE;

	if (EnablePrivilege(SE_TAKE_OWNERSHIP_NAME, 0) && EnablePrivilege(SE_RESTORE_NAME, 0))
	{//要先申请SE_TAKE_OWNERSHIP特权
		GetUserNameA(UserName, &cbUserName);
		if (LookupAccountNameA(NULL, UserName, &Sid, &cbSid, DomainBuffer, &cbDomainBuffer, &eUse))
		{
			myZeroMemory(&Ea, sizeof(EXPLICIT_ACCESS));

			BuildExplicitAccessWithName(&Ea, tmp, GENERIC_ALL, GRANT_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
			if (SetEntriesInAclW(1, &Ea, OldDacl, &Dacl) == ERROR_SUCCESS)
			{//直接不复制旧的ace,不然无法去除文件的拒绝权
				SetNamedSecurityInfoW((LPWSTR)FilePath, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &Sid, NULL, NULL, NULL);
				if (SetNamedSecurityInfoW((LPWSTR)FilePath, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &Sid, NULL, Dacl, NULL) == ERROR_SUCCESS)
					Ret = TRUE;
			}

		}
	}
	return Ret;
}
BOOL RunEXE(wchar_t* CmdLine, DWORD flag, STARTUPINFO* si)
{//用CreateProcess来创建进程
	STARTUPINFO s;
	myZeroMemory(&s, sizeof(STARTUPINFO));
	if (si == nullptr)si = &s;
	PROCESS_INFORMATION pi;
	//#ifndef _WIN64
	myZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	//#endif
	const BOOL f = CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, flag, NULL, NULL, si, &pi);
	if (pi.hProcess)CloseHandle(pi.hProcess);
	if (pi.hThread)CloseHandle(pi.hThread);
	return f;
}

ATOM MyRegisterClass(HINSTANCE h, WNDPROC proc, LPCWSTR ClassName, unsigned int style)
{//封装过的注册Class函数.
	WNDCLASSEXW wcex;
	myZeroMemory(&wcex, sizeof(WNDCLASSEXW));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = style;
	wcex.lpfnWndProc = proc;
	wcex.hInstance = h;
	wcex.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_GUI));//这个函数不支持自定义窗体图标
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = ClassName;//自定义ClassName和WndProc
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标
	return RegisterClassExW(&wcex);
}

BOOL RunWithAdmin(wchar_t* path)//以管理员身份运行一个程序.
{
	SHELLEXECUTEINFO info;
	myZeroMemory(&info, sizeof(SHELLEXECUTEINFO));
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

int CaptureImage(const wchar_t *FilePath)//截图并放在指定文件夹内
{
	HDC hdcScreen, hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;
	myZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
	DWORD dwBmpSize;
	HANDLE hDIB;
	CHAR* lpbitmap;
	HANDLE hFile;
	DWORD dwSizeofDIB, dwBytesWritten = 0;
	hdcScreen = GetDC(NULL); // 获取桌面DC
	hdcMemDC = CreateCompatibleDC(hdcScreen);
	DEVMODE curDevMod;
	myZeroMemory(&curDevMod, sizeof(DEVMODE));
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
	hFile = CreateFile(FilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
	HMODULE huser = LoadLibrary(L"user32.dll");//显示界面时需要关闭系统的DPI缩放
	if (huser)SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
	if (SetProcessDPIAware != NULL)SetProcessDPIAware();
}
#pragma warning(disable:4244)
#pragma warning(disable:4312)
int myrand();
void change(void* Src, bool wow)
{//2016版极域的Knock密码
	unsigned int v5, v10;
	BYTE* v6, * v7, v8, * v9, * i;
	HKEY phkResult;//键值的句柄
	LPCWSTR lpSubKeya;

	phkResult = 0;

	if (wow)RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\WOW6432Node\\TopDomain\\e-Learning Class\\Student", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &phkResult);
	else//根据系统位数打开键值所在的目录
		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\TopDomain\\e-Learning Class\\Student", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &phkResult);

	//这里介绍2016、2017版Knock密码的加密方式
	//
	//极域在2016年对加密方式进行了较大的更改。
	//比如，密码键值被换了一个目录，换到了TopDomain\e-Learning Class\Student目录存储
	//还有就是，密码的加密强度变大了一些(不过还是不大)
	//这段代码是2018年用IDA反编译得来的，所以变量名和风格会有些奇怪
	//
	v5 = (myrand() % 40 + 83) & 0xFFFFFFFC;//首先，随机得到加密后密码的长度，向下取整为4的倍数
	v6 = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, v5);//申请相应的内存空间
	if (v6 == 0)return;//密文储存在v6中
	v7 = v6;//v7是指向v6开头的指针
	if (v5 >> 1)
	{//先给v6每个位上都写上随机值
		lpSubKeya = (LPCWSTR)(v5 >> 1);
		do
		{
			*(WORD*)v7 = myrand();
			v7 += 2;
			lpSubKeya = (LPCWSTR)((char*)lpSubKeya - 1);
		} while (lpSubKeya);
	}

	v8 = myrand() % (v5 - 68) + 2;//取一个大小为2 - 57的随机值
	*v6 = v8;//v6的第一位和最后一位都标上这个值
	v6[v5 - 1] = v8;//以这个值为密码字段的开始，将这个值记为v8
	if (Src && mywcslen((const wchar_t*)Src))//如果密码不为空，将密码明文先复制到v8处
		mymemcpy(&v6[v8], Src, 2 * mywcslen((const wchar_t*)Src) + 2);
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

	DWORD ret, dwSize = 999, dwType = REG_BINARY; BYTE data[1000];//尝试打开键值，然后将v6设到Knock和Knock1中
	ret = RegQueryValueExW(phkResult, L"Knock", 0, &dwType, (LPBYTE)data, &dwSize);

	if (ret == 0)RegSetValueExW(phkResult, L"Knock", 0, 3u, v6, v5);//总体来说，极域的Knock密码对于大多数学生而言非常有迷惑性----
	ret = RegQueryValueExW(phkResult, L"Knock1", 0, &dwType, (LPBYTE)data, &dwSize);//同样地，对极域的代理商似乎也有迷惑性
	if (ret == 0)RegSetValueExW(phkResult, L"Knock1", 0, 3u, v6, v5);//所以他们给程序加了后门:mythware_super_password，无视限制的超级密码
	HeapFree(GetProcessHeap(), 0, v6);//于是非常有迷惑性的密码就这样没用了。
	RegCloseKey(phkResult);
}
#pragma warning(default:4244)

BOOL ReleaseRes(const wchar_t* strFileName, WORD wResID, const wchar_t* strFileType)//释放指定资源
{
	if (GetFileAttributes(strFileName) != INVALID_FILE_ATTRIBUTES) { return TRUE; }//资源已存在->退出

	DWORD   dwWrite = 0;// 资源大小  

	// 创建文件  
	HANDLE  hFile = CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)return FALSE;//创建失败->退出

	// 查找资源文件中、加载资源到内存、得到资源大小  
	HRSRC   hrsc = FindResource(NULL, MAKEINTRESOURCE(wResID), strFileType);
	if (!hrsc)return FALSE;
	HGLOBAL hG = LoadResource(NULL, hrsc);
	const DWORD dwSize = SizeofResource(NULL, hrsc);

	// 写入文件  
	WriteFile(hFile, hG, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);
	return TRUE;
}

void LoadPicture(const wchar_t* lpFilePath, HDC hdc, int startx, int starty, float DPI)
{
	// 文件句柄   
	HANDLE FileHandle;
	// 高位数据、低位数据   
	DWORD SizeH, SizeL, ReadCount;

	IStream* pstream = NULL;
	IPicture* pPic = NULL;
	// 以读的方式打开图像   
	FileHandle = CreateFile(lpFilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL
	);
	// 打开失败   
	if (FileHandle == INVALID_HANDLE_VALUE)
		return;
	// 获取图像文件大小   
	SizeL = GetFileSize(FileHandle, &SizeH);
	// 为图像文件   
	// 分配一个可移动的全局的堆内存   
	HGLOBAL pBuffer = GlobalAlloc(GMEM_MOVEABLE, SizeL);
	// 分配对内存失败   
	if (pBuffer == NULL)
	{
		CloseHandle(FileHandle);
		return;
	}
	// 指向堆内存的指针转换为通用的指针类型相当于解锁   
	LPVOID pDes = GlobalLock(pBuffer);

	// 读入数据进堆内存   
	if (ReadFile(FileHandle, pDes, SizeL, &ReadCount, NULL) == 0)
	{
		CloseHandle(FileHandle);
		GlobalUnlock(pBuffer);
		DeleteObject(pBuffer);
		return;
	}

	// 堆内存上锁   
	GlobalUnlock(pBuffer);

	// 在全局存储器的堆中分配一个流对象   
	if (CreateStreamOnHGlobal(pBuffer, true, &pstream) != S_OK)
	{
		CloseHandle(FileHandle);
		DeleteObject(pBuffer);
		return;
	}

	// 创建一个新的图像并初始化   
	if (!SUCCEEDED(OleLoadPicture(pstream, SizeL, true, IID_IPicture, (void**)&pPic)))return;
	long hmWidth;
	long hmHeight;
	// 从IPicture中获取高度与宽度   
	pPic->get_Width(&hmWidth);
	pPic->get_Height(&hmHeight);
	const int nWidth = MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), 2540);
	const int nHeight = MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), 2540);

	pPic->Render(hdc, (int)(startx * DPI), (int)(starty * DPI), (int)(DPI * nWidth), (int)(DPI * nHeight), 0, hmHeight, hmWidth, -hmHeight, nullptr);

	pPic->Release();
	pstream->Release();
	//ReleaseDC(hwnd, hdc);
	CloseHandle(FileHandle);
}

void ReleaseLanguageFiles(const wchar_t* Path, int tag, wchar_t* str)//tag 和 str 可不填
{//当tag填1或2时函数将语言文件路径保存在str中
	wchar_t LanguagePath[MAX_PATH];//先释放自带的两个中英文语言文件
	mywcscpy(LanguagePath, Path);
	mywcscat(LanguagePath, L"language\\");//创建language目录
	CreateDirectory(LanguagePath, NULL);
	mywcscat(LanguagePath, L"Chinese.ini");
	ReleaseRes(LanguagePath, FILE_CHN, L"JPG");
	if (tag == 1)mywcscpy(str, LanguagePath);
	mywcscpy(LanguagePath, Path);
	mywcscat(LanguagePath, L"language\\EnglishANSI.ini");
	ReleaseRes(LanguagePath, FILE_ENG, L"JPG");
	if (tag == 2)mywcscpy(str, LanguagePath);
}