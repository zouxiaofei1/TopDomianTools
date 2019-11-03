// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#define WINVER          _WIN32_WINNT_WINXP
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 在此处引用程序需要的其他头文件

#include <locale.h>//设置语言区域等
#include <unordered_map>//map映射
#include <TlHelp32.h>//遍历进程
#include <commdlg.h>//包括"选择文件"对话框的头文件
#include <shellapi.h>//用于接收文件拖拽
#include <ctime>//时间
#include <aclapi.h>//取得特权用
#include <winsock.h>//用他来获取ip地址
#include <WinBase.h>//包括一大堆api
#include <stack>//栈
#include <Shlobj.h>//包括"选择文件夹"对话框的头文件
#include <atlimage.h>//绘制.jpg