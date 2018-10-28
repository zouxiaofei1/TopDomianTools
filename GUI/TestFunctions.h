#pragma once
#include "stdafx.h"
#ifndef _WIN64
#define Bitstr L"32Bit Update 183"
#else
#define Bitstr L"64Bit Update 183"
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

//
//void wcharTochar(const wchar_t *wchar, char *chr, int length)
//{
//	WideCharToMultiByte(CP_ACP, 0, wchar, -1, chr, length, NULL, NULL);
//}
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
#define CreateStrs \
	Main.CreateString(L"安装成功，请按Scroll Lock键查看效果！", L"OneOK");\
	Main.CreateString(L"安装失败\n文件不存在", L"OneFail");\
	Main.CreateString(L"卸载成功", L"unQSOK");\
	Main.CreateString(L"提示", L"Info");\
	Main.CreateString(L"一键安装", L"setQS");\
	Main.CreateString(L"卸载", L"unQS");\
	Main.CreateString(L"启动失败", L"StartFail");\
	Main.CreateString(L"Sethc删除失败！权限不足", L"DSR3Fail");\
	Main.CreateString(L"Sethc删除失败！\n可能因为权限不足/驱动文件不存在", L"DSR0Fail");\
	Main.CreateString(L"Sethc复制失败！\n权限不足", L"CSFail");\
	Main.CreateString(L"Sethc复制失败！\n文件不存在", L"NoSethc");\
	Main.CreateString(L"NTSD复制失败！\n权限不足", L"CNTSDFail");\
	Main.CreateString(L"NTSD复制失败！\n文件不存在", L"NoNTSD");\
	Main.CreateString(L"打开键值失败!\n可能因为没有足够权限或极域未安装", L"ACFail");\
	Main.CreateString(L"修改键值失败!\n(未知错误)", L"ACUKE");\
	Main.CreateString(L"修改成功", L"ACOK");\
	Main.CreateString(L"出错了...可能是因为键值不存在\n按确定强制读取", L"VPFail");\
	Main.CreateString(L"出错了...\n按确定强制读取", L"VPUKE");\
	Main.CreateString(L"密码为空或键值不存在", L"VPNULL");\
	Main.CreateString(L"删除文件", L"Tdelete");\
	Main.CreateString(L"复制文件", L"Tcopy");\
	Main.CreateString(L"按快捷键Ctrl+B切换", L"Tctrl+b");\
	Main.CreateString(L"极域不同版本密码格式不一样", L"Tcp1");\
	Main.CreateString(L"并不一定能修改成功", L"Tcp2");\
	Main.CreateString(L"作者：minecraft cxy villager", L"Tcoder");\
	Main.CreateString(L"版本：C++ v1.8.4 测试版", L"Tver");\
	Main.CreateString(Bitstr, L"Tver2");\
	Main.CreateString(L"CopyLeft© SA软件 2015 - 2018", L"_Tleft");\
	Main.CreateString(L"© SA软件 2015 - 2018", L"Tleft");\
	Main.CreateString(L"版权 - 不存在的", L"Tleft2");\
	Main.CreateString(L"极域破解v1.8.4 不是管理员", L"Tmain");\
	Main.CreateString(L"极域破解v1.8.4", L"Tmain2");\
	Main.CreateString(L"这不是文件 / 文件夹！", L"TINotF");\
	Main.CreateString(L"请启动360！360可能会报加载驱动，请放行！\n如果执行后蓝屏，请把dump送至作者邮箱", L"360Start");\
	Main.CreateString(L"加载驱动失败!\n可能是因为权限不足/操作被360拦截/文件不存在", L"360Fail");\
	Main.CreateString(L"当前非管理员模式，蓝屏可能无效", L"BSODAsk");\
	Main.CreateString(L"密码为:", L"pswdis");\
	Main.CreateString(L"极域电子教室 - 已连接至教师端", L"tnd");\
	CatchWnd.CreateString(L"剩余 ", L"Timer1");\
	CatchWnd.CreateString(L" 秒", L"Timer2");\
	CatchWnd.CreateString(NULL, L"back");\
	Main.CreateString(L"explorer.exe", L"E_runinVD");\
	Main.CreateString(L"输入端口", L"E_ApplyCh");\
	Main.CreateString(L"输入密码", L"E_CP");\
	Main.CreateString(L"浏览文件/文件夹", L"E_View");\
	Main.CreateString(L"StudentMain", L"E_TDname");\
	Main.CreateString(L"确定要把密码改成", L"CPAsk1");\
	Main.CreateString(L"么？", L"CPAsk2");\
	Main.CreateString(L"下载成功", L"Loaded");\
	Main.CreateString(L"正在下载", L"Loading");\
	Main.CreateString(L"- 不可用 ", L"Useless");\
	Main.CreateString(L"- 可用 ", L"Usable");\
	Main.CreateString(L"- 推荐 ", L"Rec");\
	Main.CreateString(L"按 Ctrl+P 组合键可显示/隐藏此窗口", L"Ttip1");\
	Main.CreateString(L"一般情况下建议使用“一键安装”或“虚拟桌面”", L"Ttip2");\
	Main.CreateString(L"系统位数:", L"Tbit");\
	Main.CreateString(L"系统版本:", L"Twinver");\
	Main.CreateString(L"cmd状态:", L"Tcmd");\
	Main.CreateString(L"极域版本:", L"TTDv");\
	Main.CreateString(L"IP地址:", L"TIP");\
	Main.CreateString(L"存在", L"TcmdOK");\
	Main.CreateString(L"不存在", L"TcmdNO");\
	Main.CreateString(L"打游戏", L"Games");\
	Main.CreateString(L"停止", L"Gamee");\
	CatchWnd.CreateString(L"已经吃掉了 ", L"Eat1");\
	CatchWnd.CreateString(L" 个窗口", L"Eat2");

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
