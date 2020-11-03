// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//by zouxiaofei1 2015 - 2020

#pragma once
#define WINVER          _WIN32_WINNT_WINXP
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>


// TODO: 在此处引用程序需要的其他头文件
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 
#include <TlHelp32.h>//遍历进程
#include <commdlg.h>//包括"选择文件"对话框的头文件
#include <shellapi.h>//用于接收文件拖拽
#include <aclapi.h>//取得特权用
#include <winsock.h>//用来获取ip地址
#include <Shlobj.h>//包括"选择文件夹"对话框的头文件
#include <OleCtl.h>//绘制jpg

#include <UserEnv.h>//myPaExec用的头文件
#include <sddl.h>
#include <Psapi.h>
#include <WtsApi32.h>

//#include "mystd.h"//自定义的字符串操作头文件