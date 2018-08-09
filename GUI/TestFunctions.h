#pragma once
#include "stdafx.h"

void charTowchar(const char *chr, wchar_t *wchar, int size);
void wcharTochar(const wchar_t *wchar, char *chr, int length);
void s(int cathy)
{
	wchar_t Cathytmp[34];
	_itow_s(cathy, Cathytmp, 10);
	MessageBox(NULL, Cathytmp, L"", NULL);
}
void s(LPCSTR cathy)
{
	MessageBoxA(NULL, cathy, "", NULL);
}

void charTowchar(const char *chr, wchar_t *wchar, int size)
{
	MultiByteToWideChar(CP_ACP, 0, chr, strlen(chr) + 1, wchar, size / sizeof(wchar[0]));
}


void wcharTochar(const wchar_t *wchar, char *chr, int length)
{
	WideCharToMultiByte(CP_ACP, 0, wchar, -1, chr, length, NULL, NULL);
}

int __cdecl sub_430CD0(void *Src, LPCWSTR lpSubKey, LPCWSTR lpValueName)
{
	const WCHAR *v3;
	unsigned int v4, v5, v10, v12;
	BYTE *v6, *v7, v8, *v9, *v11, *i;
	const WCHAR *v14;
	LSTATUS v15;
	int result;
	HKEY phkResult;
	LPCWSTR lpSubKeya;
	v3 = lpSubKey;
	phkResult = 0;
	if (!lpSubKey)
		v3 = L"Software\\TopDomain\\e-Learning Class\\Student";
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, v3, 0, 0x20006u, &phkResult))
		goto LABEL_25;
	v4 = _time64(0);
	srand(v4);
	v5 = (rand() % 40 + 83) & 0xFFFFFFFC;
	v6 = (BYTE *)operator new[](v5);
	v7 = v6;
	if (v5 >> 1)
	{
		lpSubKeya = (LPCWSTR)(v5 >> 1);
		do
		{
			*(WORD *)v7 = rand();
			v7 += 2;
			lpSubKeya = (LPCWSTR)((char *)lpSubKeya - 1);
		} while (lpSubKeya);
	}
	v8 = rand() % (v5 - 68) + 2;
	*v6 = v8;
	v6[v5 - 1] = v8;
	if (Src && wcslen((const wchar_t *)Src))
		memcpy(&v6[v8], Src, 2 * wcslen((const wchar_t *)Src) + 2);
	else
	{
		v9 = &v6[v8];
		*v9 = 0;
		v9[1] = 0;
	}
	v10 = v5 >> 2;
	v11 = v6;
	if (v5 >> 2)
	{
		v12 = v5 >> 2;
		do
		{
			*(DWORD *)v11 ^= 0x454C4350u;
			v11 += 4;
			--v12;
		} while (v12);
	}
	for (i = v6; v10; --v10)
	{
		*(DWORD *)i ^= 0x50434C45u;
		i += 4;
	}
	v14 = lpValueName;
	if (!lpValueName)
		v14 = L"Knock1";
	v15 = RegSetValueExW(phkResult, v14, 0, 3u, v6, v5);
	operator delete[](v6);
	if (v15)
	{
	LABEL_25:
		if (phkResult)RegCloseKey(phkResult);
		result = 0;
	}
	else
	{
		RegCloseKey(phkResult);
		result = 1;
	}
	return result;
}

BOOL LoadNTDriver(LPCWSTR lpszDriverName,LPCWSTR lpszDriverPath)

{
	wchar_t szDriverImagePath[256];
	//得到完整的驱动路径
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);
	//s(szDriverImagePath);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄

								 //打开服务控制管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		//s(9);
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
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	DWORD dwRtn;

	//判断服务是否失败
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			//s(99);
			bRet = FALSE;
			goto BeforeLeave;
		}
		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			//s(999);
			bRet = FALSE;
			goto BeforeLeave;
		}
	}

	//开启此项服务
	bRet = StartService(hServiceDDK, NULL, NULL);
	if (!bRet)
	{
		DWORD dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			//s(9999);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				//s(99999);
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

unsigned int Hash(const wchar_t *str)
{
	unsigned int seed = 131;
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}