#pragma once
#include "stdafx.h"
#ifndef _WIN64
#define Bitstr L"32Bit Update 167"
#else
#define Bitstr L"64Bit Update 167"
#endif

void charTowchar(const char *chr, wchar_t *wchar, int size);
void wcharTochar(const wchar_t *wchar, char *chr, int length);
void s(int cathy)
{
	wchar_t Cathytmp[34];
	_itow_s(cathy, Cathytmp, 10);
	MessageBox(NULL, Cathytmp, L"", NULL);
}
void out(int cathy)
{
	wchar_t Cathytmp[34];
	_itow_s(cathy, Cathytmp, 10);
	wcscat_s(Cathytmp, L"\n");
	OutputDebugString(Cathytmp);
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
#pragma warning(disable:4244)
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
	v4 = (unsigned int)_time64(0);
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
#pragma warning(default:4244)
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
#define CreateStrs \
	Main.CreateString(L"��װ�ɹ����밴Scroll Lock���鿴Ч����", L"OneOK");\
	Main.CreateString(L"��װʧ��\n�ļ�������", L"OneFail");\
	Main.CreateString(L"ж�سɹ�", L"unQSOK");\
	Main.CreateString(L"��ʾ", L"Info");\
	Main.CreateString(L"һ����װ", L"setQS");\
	Main.CreateString(L"ж��", L"unQS");\
	Main.CreateString(L"����ʧ��", L"StartFail");\
	Main.CreateString(L"Sethcɾ��ʧ�ܣ�Ȩ�޲���", L"DSR3Fail");\
	Main.CreateString(L"Sethcɾ��ʧ�ܣ�\n������ΪȨ�޲���/�����ļ�������", L"DSR0Fail");\
	Main.CreateString(L"Sethc����ʧ�ܣ�\nȨ�޲���", L"CSFail");\
	Main.CreateString(L"Sethc����ʧ�ܣ�\n�ļ�������", L"NoSethc");\
	Main.CreateString(L"NTSD����ʧ�ܣ�\nȨ�޲���", L"CNTSDFail");\
	Main.CreateString(L"NTSD����ʧ�ܣ�\n�ļ�������", L"NoNTSD");\
	Main.CreateString(L"�򿪼�ֵʧ��!\n������Ϊû���㹻Ȩ�޻���δ��װ", L"ACFail");\
	Main.CreateString(L"�޸ļ�ֵʧ��!\n(δ֪����)", L"ACUKE");\
	Main.CreateString(L"�޸ĳɹ�", L"ACOK");\
	Main.CreateString(L"������...��������Ϊ��ֵ������\n��ȷ��ǿ�ƶ�ȡ", L"VPFail");\
	Main.CreateString(L"������...\n��ȷ��ǿ�ƶ�ȡ", L"VPUKE");\
	Main.CreateString(L"����Ϊ�ջ��ֵ������", L"VPNULL");\
	Main.CreateString(L"ɾ���ļ�", L"Tdelete");\
	Main.CreateString(L"�����ļ�", L"Tcopy");\
	Main.CreateString(L"����ݼ�Ctrl+B�л�", L"Tctrl+b");\
	Main.CreateString(L"����ͬ�汾�����ʽ��һ��", L"Tcp1");\
	Main.CreateString(L"����һ�����޸ĳɹ�", L"Tcp2");\
	Main.CreateString(L"���ߣ�minecraft cxy villager", L"Tcoder");\
	Main.CreateString(L"�汾��C++ v1.8.4 ���԰�", L"Tver");\
	Main.CreateString(Bitstr, L"Tver2");\
	Main.CreateString(L"CopyLeft(?) SA��� 2015 - 2018", L"_Tleft");\
	Main.CreateString(L"SA��� 2015 - 2018", L"Tleft");\
	Main.CreateString(L"��Ȩ - �����ڵ�", L"Tleft2");\
	Main.CreateString(L"�����ƽ�v1.8.4 ���ǹ���Ա", L"Tmain");\
	Main.CreateString(L"�����ƽ�v1.8.4", L"Tmain2");\
	Main.CreateString(L"�ⲻ���ļ� / �ļ��У�", L"TINotF");\
	Main.CreateString(L"������360��360���ܻᱨ��������������У�\n���ִ�к����������dump������������", L"360Start");\
	Main.CreateString(L"��������ʧ��!\n��������ΪȨ�޲���/������360����/�ļ�������", L"360Fail");\
	Main.CreateString(L"��ǰ�ǹ���Աģʽ������������Ч", L"BSODAsk");\
	Main.CreateString(L"����Ϊ:", L"pswdis");\
	Main.CreateString(L"������ӽ��� - ����������ʦ��", L"tnd");\
	CatchWnd.CreateString(L"ʣ�� ", L"Timer1");\
	CatchWnd.CreateString(L" ��", L"Timer2");\
	CatchWnd.CreateString(NULL, L"back");\
	Main.CreateString(L"explorer.exe", L"E_runinVD");\
	Main.CreateString(L"����˿�", L"E_ApplyCh");\
	Main.CreateString(L"��������", L"E_CP");\
	Main.CreateString(L"����ļ�/�ļ���", L"E_View");\
	Main.CreateString(L"StudentMain", L"E_TDname");\
	Main.CreateString(L"ȷ��Ҫ������ĳ�", L"CPAsk1");\
	Main.CreateString(L"ô��", L"CPAsk2");\
	Main.CreateString(L"���سɹ�", L"Loaded");\
	Main.CreateString(L"��������", L"Loading");\
	Main.CreateString(L"- ������ ", L"Useless");\
	Main.CreateString(L"- ���� ", L"Usable");\
	Main.CreateString(L"- �Ƽ� ", L"Rec");\
	Main.CreateString(L"�� Ctrl+P ��ϼ�����ʾ/���ش˴���", L"Ttip1");\
	Main.CreateString(L"һ������½���ʹ�á�һ����װ�����������桱", L"Ttip2");\
	Main.CreateString(L"ϵͳλ��:", L"Tbit");\
	Main.CreateString(L"ϵͳ�汾:", L"Twinver");\
	Main.CreateString(L"cmd״̬:", L"Tcmd");\
	Main.CreateString(L"����汾:", L"TTDv");\
	Main.CreateString(L"IP��ַ:", L"TIP");\
	Main.CreateString(L"����", L"TcmdOK");\
	Main.CreateString(L"������", L"TcmdNO");\
	CatchWnd.CreateString(L"�Ѿ��Ե��� ", L"Eat1");\
	CatchWnd.CreateString(L" ������", L"Eat2");

bool Findquotations(wchar_t* zxf, wchar_t zxf2[])
{
	wchar_t tmp0;
	wchar_t *tmp1 = wcsstr(zxf, L"\"");
	wchar_t *tmp2 = wcsstr(tmp1 + 1, L"\"");
	if (tmp1 == 0 || tmp2 == 0)return false;
	tmp0 = *tmp2;
	*tmp2 = 0;
	wcscpy(zxf2, tmp1 + 1);
	*tmp2 = tmp0;
	return true;
}
