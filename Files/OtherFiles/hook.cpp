// HOOK.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "HOOK.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void myfunction()
{
	//MessageBox(0, 0, 0, 0);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;//创建进程快照
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return;
	while (Process32Next(hSnapShot, &pe))
	{
		//MessageBox(0, pe.szExeFile, 0, 0);
		if (
			(pe.szExeFile[0]=='S'|| pe.szExeFile[0] == 's')
			&& pe.szExeFile[1] == 't'&&
			pe.szExeFile[2] == 'u'
			)
		{
			DebugActiveProcess(pe.th32ProcessID);
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = 0;
			hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
			TerminateProcess(hProcess, 1);//否则使用普通的OpenProcess和TerminateProcess
			CloseHandle(hProcess);
		}
	}

}
int main()
{
	hInst = GetModuleHandle(NULL);
    MyRegisterClass(hInst);

    // 执行应用程序初始化:
    if (!InitInstance(hInst))return FALSE;

   
    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        
    }

	return 0;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {0};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HOOK);
    wcex.lpszClassName  =L"hook";

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(L"hook", L"",0,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   RegisterHotKey(hWnd, 1, NULL, VK_SCROLL);
   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_HOTKEY:
	{
		MessageBox(0, 0, 0, 0);
		myfunction();
		char a[260];
		GetModuleFileNameA(NULL, a, 260);
		WinExec(a, 0);
		ExitProcess(0);
	}
   
    }
    return 0;
}


