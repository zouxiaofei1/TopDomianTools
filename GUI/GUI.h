//为了减短GUI.cpp长度
//多数和Class不相关的函数都被放到这里

#pragma once

#include "resource.h"

#define BUTTON_IN(x,y) if(x == Hash(y))
#define Delta 10 //按钮渐变色速度

#define InitBrushs DBlueBrush = CreateSolidBrush(RGB(210, 255, 255));\
	LBlueBrush = CreateSolidBrush(RGB(230, 255, 255));\
	WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));\
	BlueBrush = CreateSolidBrush(RGB(40, 130, 240));\
	green = CreateSolidBrush(RGB(0, 206, 209));\
	grey = CreateSolidBrush(RGB(245, 245, 245));\
	Dgrey = CreateSolidBrush(RGB(230, 230, 230));\
	yellow = CreateSolidBrush(RGB(244, 238, 175));\
	DBlue = CreatePen(PS_SOLID, 1, RGB(210, 255, 255));\
	LBlue = CreatePen(PS_SOLID, 1, RGB(230, 255, 255));\
	YELLOW = CreatePen(PS_SOLID, 1, RGB(0xC1, 0xCD, 0xCD));\
	RED = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));\
	BLACK = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));\
	White = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));\
	GREEN = CreatePen(PS_SOLID, 2, RGB(5, 195, 195));\
	LGREY = CreatePen(PS_SOLID, 1, RGB(115, 115, 115));\
	BLUE = CreatePen(PS_SOLID, 1, RGB(40, 130, 240));

#define MAX_BUTTON 50
#define MAX_CHECK 15
#define MAX_FRAME 15
#define MAX_TEXT 25
#define MAX_EDIT 10
#define MAX_LINE 20
#define MAX_STRING 85
#define MAX_AREA 5

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
}CLIENT_ID, *PCLIENT_ID;
typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PVOID ObjectName;
	ULONG Attritubes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
}OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
typedef  DWORD(__stdcall *NTOPENPROCESS)(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObejectAttributes,
	IN PCLIENT_ID PClient_Id
	);
bool MyOpenProcess(OUT PHANDLE ProcessHandle, DWORD PID)
{
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)
		return FALSE;
	NTOPENPROCESS NtOpenProcess = (NTOPENPROCESS)GetProcAddress(hModule, "NtOpenProcess");
	if (NtOpenProcess == NULL)
		return FALSE;
	CLIENT_ID ClientID;
	ClientID.UniqueProcess = (HANDLE)PID; //UniqueProcess可以接受PID
	ClientID.UniqueThread = 0;
	OBJECT_ATTRIBUTES oa;
	oa.Length = sizeof(oa);
	oa.RootDirectory = 0;
	oa.ObjectName = 0;
	oa.Attritubes = 0;
	oa.SecurityDescriptor = 0;
	oa.SecurityQualityOfService = 0;
	NtOpenProcess(ProcessHandle, PROCESS_ALL_ACCESS, &oa, &ClientID);
	return 0;
}
