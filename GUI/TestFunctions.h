//为了减短GUI.cpp长度
//一部分调试用函数、测试中的函数将被放在这里

#pragma once
#include "stdafx.h"

void charTowchar(const char* chr, wchar_t* wchar, int size);
void s(LPCWSTR a)//调试用MessageBox
{
	MessageBox(NULL, a, L"", NULL);
}
void s(int a)
{
	wchar_t tmp[34];
	_itow_s(a, tmp, 10);
	MessageBox(NULL, tmp, L"", NULL);
}

bool Findquotations(wchar_t* zxf, wchar_t zxf2[])//命令行调用找到"双引号"
{
	__try
	{
		wchar_t tmp0;
		wchar_t* tmp1 = wcsstr(zxf, L"\"");
		wchar_t* tmp2 = wcsstr(tmp1 + 1, L"\"");
		if (tmp1 == 0 || tmp2 == 0)return false;
		tmp0 = *tmp2;
		*tmp2 = 0;
		wcscpy(zxf2, tmp1 + 1);
		*tmp2 = tmp0;
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

void LoadPicture(const wchar_t* lpFilePath, HDC hdc, int startx, int starty, double DPI)
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
		free(pBuffer);
		return;
	}

	// 堆内存上锁   
	GlobalUnlock(pBuffer);

	// 在全局存储器的堆中分配一个流对象   
	if (CreateStreamOnHGlobal(pBuffer, true, &pstream) != S_OK)
	{
		CloseHandle(FileHandle);
		free(pBuffer);
		return;
	}

	// 创建一个新的图像并初始化   
	if (!SUCCEEDED(OleLoadPicture(pstream, SizeL, true, IID_IPicture, (void**)&pPic)))return;
	// 获取句柄   
//  HWND hwnd=GetDlgItem(MainHwnd,PID_PICTURE);   
	// 创建画布   
	//HDC hdc = GetDC(hwnd);
	// 高和宽   
	long hmWidth;
	long hmHeight;
	// 从IPicture中获取高度与宽度   
	pPic->get_Width(&hmWidth);
	pPic->get_Height(&hmHeight);
	//s(hmWidth);
	////    
	int nWidth = MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), 2540);
	int nHeight = MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), 2540);
	//s(nWidth);
	////    
	////SetWindowPos(hwnd, HWND_TOP, 0,0,nWidth/2,nHeight/2,SWP_FRAMECHANGED);   
	//if (nWidth < 600 && nHeight < 600)
	//	//MoveWindow(hwnd, (600-nWidth)/2,(600-nHeight)/2, nWidth, nHeight, FALSE);   
	//	SetWindowPos(hwnd, HWND_TOP, (600 - nWidth) / 2, (600 - nHeight) / 2, nWidth, nHeight, SWP_FRAMECHANGED);
	//else
	//{
	//	int large = nWidth > nHeight ? nWidth : nHeight;
	//	float k = 600;
	//	float x = k / (large * 1.0);
	//	int newx = (int)(x * nWidth);
	//	int newy = (int)(x * nHeight);
	//	SetWindowPos(hwnd, HWND_BOTTOM, 1, 50, newx, newy, SWP_FRAMECHANGED);
	//}
	//RECT rc;
	//GetClientRect(hwnd, &rc);
	pPic->Render(hdc, (int)(startx * DPI), (int)(starty * DPI), (int)(DPI * nWidth), (int)(DPI * nHeight), 0, hmHeight, hmWidth, -hmHeight, NULL);

	//  TransparentBlt(0,0, 0, rc.right-rc.left, rc.bottom-rc.top, hDC, 0, 0,  nWidth, nHeight, RGB(255,   255,   255));   
		// 释放空间   
	pPic->Release();
	pstream->Release();
	//ReleaseDC(hwnd, hdc);
	CloseHandle(FileHandle);
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
