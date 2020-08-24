//����TopDomainTools����Դ������ProcessHacker����
//������ProcessHacker��������ͨ�ŵ�һ�δ���
//https://github.com/processhacker/processhacker
//���ڼ���KPH����,ȡ��ͨ�����Ӳ�������������
//
//ע��:
//���ڱ������������沢����ͨ
//�����ע�͵����ݿ�����һ���Ĵ���
//

#pragma once
#include "stdafx.h"
#include "GUI.h"
#pragma warning(disable:4996)


typedef struct _CLIENT_ID
{//������ProcessHacker�Զ����һ���ṹ��
	HANDLE UniqueProcess;//�����˽��̵�PID��TID
	HANDLE UniqueThread;//������Ҳ��֪����ΪʲôҪ��HANDLE���洢
}CLIENT_ID, * PCLIENT_ID;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;//���������???
	PVOID ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;


BOOL LoadNTDriver(LPCWSTR lpszDriverName, LPCWSTR lpszDriverPath, bool Kernel)
{
	wchar_t szDriverImagePath[MAX_STR];
	//�õ�����������·��
	GetFullPathName(lpszDriverPath, MAX_STR, szDriverImagePath, NULL);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK = NULL;//NT��������ķ�����

								 //�򿪷�����ƹ�����
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)//�򲻿����������ʱ�˳�
	{//(��˵)������360����ʱ����������
		bRet = FALSE;
		goto BeforeLeave;
	}

	//������������Ӧ�ķ���
	hServiceDDK = CreateService(hServiceMgr,
		lpszDriverName, //�����������ע����е�����  
		lpszDriverName, // ע������������ DisplayName ֵ  
		SERVICE_ALL_ACCESS, // ������������ķ���Ȩ��  
		Kernel ? SERVICE_KERNEL_DRIVER : SERVICE_WIN32_OWN_PROCESS,// ��ʾ���صķ��������������Ƿ���
		SERVICE_DEMAND_START, // ע������������ Start ֵ  
		SERVICE_ERROR_IGNORE, // ע������������ ErrorControl ֵ  
		szDriverImagePath, // ע������������ ImagePath ֵ  
		NULL, NULL, NULL, NULL, NULL);//���漸������һ���ò���

	DWORD dwRtn;

	//�жϷ����Ƿ�ʧ��
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{//�����Ϊ���� or �����Ѿ����أ�
			bRet = FALSE;
			goto BeforeLeave;
		}//��ͼ���������
		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			bRet = FALSE;//�򲻿�? ->ʧ���˳�
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
	//ֹͣ�����������ֹͣʧ�ܣ�ֻ���������������ٶ�̬���ء�  
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

typedef _Success_(return >= 0) LONG NTSTATUS;//�����������з���ֵֻҪ���ڵ���0���൱��TRUE
typedef struct _IO_STATUS_BLOCK
{//IO_STATUS_BLOCK,��������Ϊ �������״̬��
	union//һ�����ڽ����������򷵻ص���Ϣ
	{
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef  DWORD(__stdcall* NTOPENFILE)(
	_Out_ PHANDLE FileHandle,//NtOpenFile�������Զ���
	_In_ ACCESS_MASK DesiredAccess,//�����������ﲻ���������ļ�
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,//ͬ���أ�windows�кܶ��������File�ĺ�����ֻ���Խ����ļ�����
	_In_ ULONG ShareAccess,//�����������򿪾�������豸ȡ��ͨ�ŵȵ�
	_In_ ULONG OpenOptions//��CreateFile�ɴ򿪻򴴽��ļ�����I/O�豸�������ؿɷ��ʵľ��:����̨,ͨ����Դ,Ŀ¼,����������,�ļ�,�ʲ�,�ܵ�
	);//	(ժ�԰ٶȰٿ�)

typedef struct _UNICODE_STRING
{//����ͨ���г��õ������ַ���
	USHORT Length;//(��ʵ������������CString,wchar_t���ֳ�������Ҫ��)
	USHORT MaximumLength;
	_Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;//<---���������ʵ����wchar_t* �� LPWSTR
} UNICODE_STRING, * PUNICODE_STRING;//ȴ"��������"�ػ��˸�����
typedef const UNICODE_STRING* PCUNICODE_STRING;
//�����鷳�������ܲ��ҵ������١�Ҫ�Ӵ���ȫ�µ�һ�Ѻ���������ǩ��(���ڸ��˿����߶���)�ѶȽϴ�
//�������Windows���������ż��ߵ�ԭ��֮һ��
//������������ͨ�ŵĺ���

FORCEINLINE VOID RtlInitUnicodeString(
	_Out_ PUNICODE_STRING DestinationString,
	_In_opt_ const wchar_t* SourceString
)//��������wchar[]ת��ΪUNICODE_STRING
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
    }//��OBJECT_ATTRIBUTES�ṹ�帳��ʼֵ

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)//����ͨ���й���ʶ�������Ҫ"����"
#define KPH_DEVICE_TYPE 0x9999//DeviceType,Method,Access����ֵһ�㶼�ǹ̶���
#define KPH_CTL_CODE(x) CTL_CODE(KPH_DEVICE_TYPE, 0x800 + x,3,0)
#define KPH_OPENPROCESS KPH_CTL_CODE(50)//Function������ʶ���ܱ��
#define KPH_TERMINATEPROCESS KPH_CTL_CODE(55)
typedef VOID(NTAPI* PIO_APC_ROUTINE)(
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	);
NTSTATUS KphpDeviceIoControl(//ProcessHacker�Զ����һ������ͨ����Ǻ���
	_In_ ULONG KphControlCode,//ͨ��ʶ����
	_In_ PVOID InBuffer,//�����ָ��
	_In_ ULONG InBufferLength,//���ݳ���
	_In_ HANDLE PhKphHandle//�������
)
{
	DWORD a;
	DeviceIoControl(PhKphHandle, KphControlCode, InBuffer, InBufferLength, nullptr, sizeof(DWORD), &a, nullptr);
	return GetLastError();
}

NTSTATUS KphOpenProcess(//ʹ��KProcessHacker��ȡ�ö�һ�����̵Ŀ���Ȩ
	/*_Out_*/ PHANDLE ProcessHandle,//���ؽ��̾��
	_In_ ACCESS_MASK DesiredAccess,//��ȡ�õ�Ȩ��
	_In_ PCLIENT_ID ClientId,//���̵�PID,TID
	_In_ HANDLE PhKphHandle//�������
)
{
	struct
	{//����DeviceIoControlֻ�ܴ���һ��ָ�룬
		PHANDLE ProcessHandle2;//��Ҫ����������ʱֻ�ܰ�����"���"��һ���ṹ��
		ACCESS_MASK DesiredAccess;
		PCLIENT_ID ClientId;		//��Ҳ�����һ������:
	} input = { ProcessHandle, DesiredAccess, ClientId };//32λ�����64λ�����кܶ�������ռ�ֽ�ʱ��ͬ��
	//����ɽṹ���32λ����������64λ����ͨ�ţ��ܿ��ܻ���� �򳤶Ȳ�ͬ�����µĴ���
	return KphpDeviceIoControl(//���TopDomainTools��ר�Ŷ�����һ��64λ����Win64KPHcaller��
		(ULONG)KPH_OPENPROCESS,//ʹ��32λ��TopDomainToolsҲ�ܼ�ӵغ�64λ����ͨ��
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
)//KphOpenProcess����Ǻ���
{//��Ҫ����PIDת��ΪCLIENT_ID�ṹ��
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

NTSTATUS PhTerminateProcess(//���ڽ���һ���Ѿ���"Open"���Ľ���
	_In_ HANDLE ProcessHandle,//����KphOpenProcess�л�ȡ�Ľ��̾��
	_In_ NTSTATUS ExitStatus,//�����
	_In_ HANDLE PhKphHandle//�������
)
{
	NTSTATUS status;
	struct//������KphOpenProcessͬ��
	{
		HANDLE ProcessHandle;
		NTSTATUS ExitStatus;
	} input = { ProcessHandle, ExitStatus };

	status = KphpDeviceIoControl(
		(ULONG)KPH_TERMINATEPROCESS,
		&input,//ע�ⲻͬ������KphpDeviceIoControl����ȥ�Ĳ����벻ͬ
		sizeof(input),
		PhKphHandle
	);
	return status;
}

NTSTATUS KphConnect(PHANDLE handle)//��KProcessHacker����ȡ��ͨ�ŵĺ���
{//��ȡ��ͨ��ǰ����Ҫ�������ȡ��
	OBJECT_ATTRIBUTES objectAttributes;
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)
		return FALSE;
	NTOPENFILE myopen = (NTOPENFILE)GetProcAddress(hModule, "NtOpenFile");
	UNICODE_STRING on;//��NtOpenFile��ͨ���������򿪾��
	RtlInitUnicodeString(&on, L"\\Device\\KProcessHacker2");
	InitializeObjectAttributes(//InitializeObjectAttributes�д�������������Ǽ򵥵�KProcessHacker2��
		&objectAttributes,//����\\Device\\KProcessHacker2
		&on,//���Ƶأ������� �ļ�·��Ҳ��ƽ����DOS·��������ͬ
		0x00000040,//�������д�·������Ҫ��"\\??\\"
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