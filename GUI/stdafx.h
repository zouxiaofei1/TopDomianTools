// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//
//ps:�������ͷ�ļ��ᱻԤ���뵽.obj�ļ��ڣ�
//һ���̶����ܼӿ�����ٶ�
//������ʱ��������߰��������
//
//by zouxiaofei1 2015 - 2021

#pragma once
#define WINVER          _WIN32_WINNT_WINXP
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 

#include <winsock2.h>
#include <WS2tcpip.h>
#include <TlHelp32.h>//��������
#include <commdlg.h>//����"ѡ���ļ�"�Ի����ͷ�ļ�
#include <shellapi.h>//���ڽ����ļ���ק
#include <aclapi.h>//ȡ����Ȩ��
#include <winsock.h>//������ȡip��ַ
#include <Shlobj.h>//����"ѡ���ļ���"�Ի����ͷ�ļ�
#include <OleCtl.h>//����jpg

#include <UserEnv.h>//myPaExec�õ�ͷ�ļ�
#include <sddl.h>
#include <Psapi.h>
#include <WtsApi32.h>
#include <iphlpapi.h>