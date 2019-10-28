#include <Windows.h>

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
}CLIENT_ID, * PCLIENT_ID;
typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PVOID ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;
typedef _Success_(return >= 0) LONG NTSTATUS;
typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef  DWORD(__stdcall* NTOPENFILE)(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG ShareAccess,
	_In_ ULONG OpenOptions
	);

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	_Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef const UNICODE_STRING* PCUNICODE_STRING;

int mywcslen(const wchar_t* wstr)
{
	int i = 0;
	while (*wstr != L'\0')
	{
		i++;
		wstr++;
	}
	return i;
}
FORCEINLINE VOID RtlInitUnicodeString(
	_Out_ PUNICODE_STRING DestinationString,
	_In_opt_ const wchar_t* SourceString
)
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
    }

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define KPH_DEVICE_TYPE 0x9999
#define KPH_CTL_CODE(x) CTL_CODE(KPH_DEVICE_TYPE, 0x800 + x,3,0)
#define KPH_OPENPROCESS KPH_CTL_CODE(50)
#define KPH_TERMINATEPROCESS KPH_CTL_CODE(55)
typedef VOID(NTAPI* PIO_APC_ROUTINE)(
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	);
NTSTATUS KphpDeviceIoControl(
	_In_ ULONG KphControlCode,
	_In_ PVOID InBuffer,
	_In_ ULONG InBufferLength,
	_In_ HANDLE PhKphHandle
)
{
	DWORD a;
	DeviceIoControl(PhKphHandle, KphControlCode, InBuffer, InBufferLength, nullptr, sizeof(DWORD), &a, nullptr);
	return GetLastError();
}

NTSTATUS KphOpenProcess(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ PCLIENT_ID ClientId,
	_In_ HANDLE PhKphHandle
)
{
	struct
	{
		PHANDLE ProcessHandle;
		ACCESS_MASK DesiredAccess;
		PCLIENT_ID ClientId;
	} input = { ProcessHandle, DesiredAccess, ClientId };

	return KphpDeviceIoControl(
		(ULONG)KPH_OPENPROCESS,
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
)
{
	//OBJECT_ATTRIBUTES objectAttributes;
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

NTSTATUS PhTerminateProcess(
	_In_ HANDLE ProcessHandle,
	_In_ NTSTATUS ExitStatus,
	_In_ HANDLE PhKphHandle
)
{
	NTSTATUS status;
	struct
	{
		HANDLE ProcessHandle;
		NTSTATUS ExitStatus;
	} input = { ProcessHandle, ExitStatus };

	status = KphpDeviceIoControl(
		(ULONG)KPH_TERMINATEPROCESS,
		&input,
		sizeof(input),
		PhKphHandle
	);
	return status;
}

NTSTATUS KphConnect(PHANDLE handle)
{
	OBJECT_ATTRIBUTES objectAttributes;
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)
		return FALSE;
	NTOPENFILE myopen = (NTOPENFILE)GetProcAddress(hModule, "NtOpenFile");
	UNICODE_STRING on;
	RtlInitUnicodeString(&on, L"\\Device\\KProcessHacker2");
	InitializeObjectAttributes(
		&objectAttributes,
		&on,
		0x00000040,
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
int myfunction()
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,//程序入口点
//	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	wchar_t a[401], * b = GetCommandLine(); int c = 0, num = 0;
	while (*b != 0)
	{
		a[c] = *b;
		c++;
		if (*b == ' ')c = 0;
		b++;
	}
	a[c] = 0;
	for (int i = 0; i < c; ++i)num = num * 10 + a[i] - 48;
	//wchar_t d[34];
	//_itow_s(num, d, 10);
	//MessageBox(0, d, 0, 0);
	HANDLE PhKphHandle = 0;
	//BOOL ConnectSuccess = FALSE;//尝试连接KProcessHacker
	if (num == 0)return 0;
	if (KphConnect(&PhKphHandle) >= 0)
	{
		DWORD dwProcessID = num;
		HANDLE hProcess = 0;
		PhOpenProcess(&hProcess, 1, (HANDLE)dwProcessID, PhKphHandle);
		PhTerminateProcess(hProcess, 1, PhKphHandle);
	}

	return 0;
}