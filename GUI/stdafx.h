// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#define WINVER          _WIN32_WINNT_WINXP
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#include <locale.h>//�������������
#include <unordered_map>//mapӳ��
#include <TlHelp32.h>//��������
#include <commdlg.h>//����"ѡ���ļ�"�Ի����ͷ�ļ�
#include <shellapi.h>//���ڽ����ļ���ק
#include <ctime>//ʱ��
#include <aclapi.h>//ȡ����Ȩ��
#include <winsock.h>//��������ȡip��ַ
#include <WinBase.h>//����һ���api
#include <stack>//ջ
#include <Shlobj.h>//����"ѡ���ļ���"�Ի����ͷ�ļ�
#include <atlimage.h>//����.jpg