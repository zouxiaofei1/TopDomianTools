//这是TopDomainTools工程源代码中ProcessHacker部分
//精简自ProcessHacker中与驱动通信的一段代码
//https://github.com/processhacker/processhacker
//用于加载KPH驱动,取得通信连接并用来结束进程
//
//注意:
//由于本人在驱动方面并不精通
//下面的注释等内容可能有一定的错误。
//

#pragma once
#include "stdafx.h"
#include "GUI.h"
#pragma warning(disable:4996)


typedef struct _CLIENT_ID
{//这是由ProcessHacker自定义的一个结构体
	HANDLE UniqueProcess;//保存了进程的PID和TID
	HANDLE UniqueThread;//不过我也不知道他为什么要用HANDLE来存储
}CLIENT_ID, * PCLIENT_ID;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;//对象的属性???
	PVOID ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;


BOOL LoadNTDriver(LPCWSTR lpszDriverName, LPCWSTR lpszDriverPath, bool Kernel)
{
	wchar_t szDriverImagePath[MAX_STR];
	//得到完整的驱动路径
	GetFullPathName(lpszDriverPath, MAX_STR, szDriverImagePath, NULL);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄

								 //打开服务控制管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)//打不开服务管理器时退出
	{//(据说)操作被360拦截时常常会这样
		bRet = FALSE;
		goto BeforeLeave;
	}

	//创建驱动所对应的服务
	hServiceDDK = CreateService(hServiceMgr,
		lpszDriverName, //驱动程序的在注册表中的名字  
		lpszDriverName, // 注册表驱动程序的 DisplayName 值  
		SERVICE_ALL_ACCESS, // 加载驱动程序的访问权限  
		Kernel ? SERVICE_KERNEL_DRIVER : SERVICE_WIN32_OWN_PROCESS,// 表示加载的服务是驱动程序还是服务
		SERVICE_DEMAND_START, // 注册表驱动程序的 Start 值  
		SERVICE_ERROR_IGNORE, // 注册表驱动程序的 ErrorControl 值  
		szDriverImagePath, // 注册表驱动程序的 ImagePath 值  
		NULL, NULL, NULL, NULL, NULL);//后面几个参数一般用不到

	DWORD dwRtn;

	//判断服务是否失败
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{//如果认为服务 or 驱动已经加载，
			bRet = FALSE;
			goto BeforeLeave;
		}//试图打开这个驱动
		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			bRet = FALSE;//打不开? ->失败退出
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
	//停止驱动程序，如果停止失败，只有重新启动才能再动态加载。  
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

typedef _Success_(return >= 0) LONG NTSTATUS;//在驱动控制中返回值只要大于等于0就相当于TRUE
typedef struct _IO_STATUS_BLOCK
{//IO_STATUS_BLOCK,中文译名为 输入输出状态块
	union//一般用于接收驱动程序返回的信息
	{
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef  DWORD(__stdcall* NTOPENFILE)(
	_Out_ PHANDLE FileHandle,//NtOpenFile函数的自定义
	_In_ ACCESS_MASK DesiredAccess,//不过我们这里不用它来打开文件
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,//同样地，windows中很多名字里带File的函数不只可以进行文件操作
	_In_ ULONG ShareAccess,//还可以用来打开句柄，和设备取得通信等等
	_In_ ULONG OpenOptions//如CreateFile可打开或创建文件或者I/O设备，并返回可访问的句柄:控制台,通信资源,目录,磁盘驱动器,文件,邮槽,管道
	);//	(摘自百度百科)

typedef struct _UNICODE_STRING
{//驱动通信中常用的特殊字符串
	USHORT Length;//(其实看起来并不比CString,wchar_t这种常用类型要好)
	USHORT MaximumLength;
	_Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;//<---这个玩意其实就是wchar_t* 或 LPWSTR
} UNICODE_STRING, * PUNICODE_STRING;//却"故作高深"地换了个名字
typedef const UNICODE_STRING* PCUNICODE_STRING;
//调试麻烦、网上能查找的资料少、要接触到全新的一堆函数、数字签名(对于个人开发者而言)难度较大
//这可能是Windows驱动开发门槛高的原因之一吧
//下面是与驱动通信的函数

FORCEINLINE VOID RtlInitUnicodeString(
	_Out_ PUNICODE_STRING DestinationString,
	_In_opt_ const wchar_t* SourceString
)//将常见的wchar[]转换为UNICODE_STRING
{
	if (SourceString)
		DestinationString->MaximumLength = (DestinationString->Length = (USHORT)(mywcslen(SourceString) * sizeof(WCHAR))) + sizeof(WCHAR);
	else
		DestinationString->MaximumLength = DestinationString->Length = 0;

	DestinationString->Buffer = (PWCH)SourceString;
}

#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; \
    }//给OBJECT_ATTRIBUTES结构体赋初始值

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)//驱动通信中构造识别码的重要"函数"
#define KPH_DEVICE_TYPE 0x9999//DeviceType,Method,Access三个值一般都是固定的
#define KPH_CTL_CODE(x) CTL_CODE(KPH_DEVICE_TYPE, 0x800 + x,3,0)
#define KPH_OPENPROCESS KPH_CTL_CODE(50)//Function用来标识功能编号
#define KPH_TERMINATEPROCESS KPH_CTL_CODE(55)
typedef VOID(NTAPI* PIO_APC_ROUTINE)(
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	);
NTSTATUS KphpDeviceIoControl(//ProcessHacker自定义的一个驱动通信外壳函数
	_In_ ULONG KphControlCode,//通信识别码
	_In_ PVOID InBuffer,//传入的指针
	_In_ ULONG InBufferLength,//数据长度
	_In_ HANDLE PhKphHandle//驱动句柄
)
{
	DWORD a;
	DeviceIoControl(PhKphHandle, KphControlCode, InBuffer, InBufferLength, nullptr, sizeof(DWORD), &a, nullptr);
	return GetLastError();
}

NTSTATUS KphOpenProcess(//使用KProcessHacker来取得对一个进程的控制权
	/*_Out_*/ PHANDLE ProcessHandle,//返回进程句柄
	_In_ ACCESS_MASK DesiredAccess,//想取得的权限
	_In_ PCLIENT_ID ClientId,//进程的PID,TID
	_In_ HANDLE PhKphHandle//驱动句柄
)
{
	struct
	{//由于DeviceIoControl只能传入一个指针，
		PHANDLE ProcessHandle2;//想要传入多个数据时只能把他们"打包"成一个结构体
		ACCESS_MASK DesiredAccess;
		PCLIENT_ID ClientId;		//这也造成了一个问题:
	} input = { ProcessHandle, DesiredAccess, ClientId };//32位程序和64位程序中很多类型所占字节时不同的
	//打包成结构体后，32位程序如果想和64位驱动通信，很可能会出现 因长度不同而导致的错误
	return KphpDeviceIoControl(//因此TopDomainTools中专门定制了一个64位程序Win64KPHcaller，
		(ULONG)KPH_OPENPROCESS,//使得32位的TopDomainTools也能间接地和64位驱动通信
		&input,
		sizeof(input),
		PhKphHandle
	);
}
NTSTATUS PhOpenProcess(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ HANDLE ProcessId,
	_In_ HANDLE PhKphHandle
)//KphOpenProcess的外壳函数
{//主要负责将PID转换为CLIENT_ID结构体
	CLIENT_ID clientId;

	clientId.UniqueProcess = ProcessId;
	clientId.UniqueThread = NULL;
	return KphOpenProcess(
		ProcessHandle,
		DesiredAccess,
		&clientId,
		PhKphHandle
	);
}

NTSTATUS PhTerminateProcess(//用于结束一个已经被"Open"过的进程
	_In_ HANDLE ProcessHandle,//已在KphOpenProcess中获取的进程句柄
	_In_ NTSTATUS ExitStatus,//随便填
	_In_ HANDLE PhKphHandle//驱动句柄
)
{
	NTSTATUS status;
	struct//基本和KphOpenProcess同理
	{
		HANDLE ProcessHandle;
		NTSTATUS ExitStatus;
	} input = { ProcessHandle, ExitStatus };

	status = KphpDeviceIoControl(
		(ULONG)KPH_TERMINATEPROCESS,
		&input,//注意不同操作中KphpDeviceIoControl传进去的操作码不同
		sizeof(input),
		PhKphHandle
	);
	return status;
}

NTSTATUS KphConnect(PHANDLE handle)//和KProcessHacker驱动取得通信的函数
{//在取得通信前先需要加载这个取得
	OBJECT_ATTRIBUTES objectAttributes;
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)
		return FALSE;
	NTOPENFILE myopen = (NTOPENFILE)GetProcAddress(hModule, "NtOpenFile");
	UNICODE_STRING on;//用NtOpenFile来通过驱动名打开句柄
	RtlInitUnicodeString(&on, L"\\Device\\KProcessHacker2");
	InitializeObjectAttributes(//InitializeObjectAttributes中传入的驱动名不是简单的KProcessHacker2，
		&objectAttributes,//而是\\Device\\KProcessHacker2
		&on,//类似地，驱动中 文件路径也和平常的DOS路径有所不同
		0x00000040,//向驱动中传路径往往要加"\\??\\"
		NULL,
		NULL
	);
	IO_STATUS_BLOCK isb;
	return myopen(
		handle,
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,
		&objectAttributes,
		&isb,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0x00000040
	);
}