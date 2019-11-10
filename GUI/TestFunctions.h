//为了减短GUI.cpp长度
//一部分调试用函数、测试中的函数将被放在这里

#pragma once
#include "stdafx.h"

void charTowchar(const char *chr, wchar_t *wchar, int size);
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
