// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#define WINVER          _WIN32_WINNT_WINXP
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
#define _CRT_SECURE_NO_WARNINGS
//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 
#include <hash_map>
#include <TlHelp32.h>//��������
#include <commdlg.h>//����"ѡ���ļ�"�Ի����ͷ�ļ�
#include <shellapi.h>//���ڽ����ļ���ק
#include <ctime>//ʱ��
#include <aclapi.h>//ȡ����Ȩ��
#include <winsock.h>//������ȡip��ַ
#include <WinBase.h>//����һ���api
#include <Shlobj.h>//����"ѡ���ļ���"�Ի����ͷ�ļ�
#include <map>//mapӳ��
#include <OleCtl.h>//����jpg

#include <UserEnv.h>//myPaExec�õ�ͷ�ļ�
#include <sddl.h>
#include <Psapi.h>
#include <WtsApi32.h>