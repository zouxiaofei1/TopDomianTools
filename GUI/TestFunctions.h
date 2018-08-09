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
	//�õ�����������·��
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);
	//s(szDriverImagePath);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK = NULL;//NT��������ķ�����

								 //�򿪷�����ƹ�����
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		//s(9);
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
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	DWORD dwRtn;

	//�жϷ����Ƿ�ʧ��
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

	//�����������
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