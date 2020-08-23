//��GetAsyncKeyState�����Զ���Ŀ�ݼ�ϵͳ
//���Ǻ��ȶ����д��ڲ��Խ׶�
//by zouxiaofei1 2015 - 2020

#pragma once
#include "stdafx.h"
#include "GUI.h"
#pragma warning(disable:4996)
#define MAX_HOTKEY 45
//void s();
//void s(int a);

struct MYHOTKEY
{
	HWND hWnd;
	int id, ms, vk, delay;
	BOOL Enabled;
}Hotkey[MAX_HOTKEY];
#define HOTKEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
void CALLBACK HotKeyTimer(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	(hWnd); (nMsg); (nTimerid); (dwTime);

	for (int i = 1; i < MAX_HOTKEY; ++i)
	{
		--Hotkey[i].delay;
		if (Hotkey[i].Enabled && Hotkey[i].delay <= 0)
		{
			if (HOTKEY_DOWN(Hotkey[i].vk))
			{
				if (((Hotkey[i].ms & MOD_CONTROL)>>1) !=(HOTKEY_DOWN(VK_CONTROL)))continue;
				if (((Hotkey[i].ms & MOD_SHIFT)>>2) != (HOTKEY_DOWN(VK_SHIFT)))continue;
				if ((Hotkey[i].ms & MOD_ALT) != (HOTKEY_DOWN(VK_MENU)))continue;
				SendMessage(Hotkey[i].hWnd, WM_HOTKEY, (WPARAM)Hotkey[i].id, 0);
				
				if (Hotkey[i].delay < 0)Hotkey[i].delay = 12; else Hotkey[i].delay = 2;
			}
		}
	}
}
void InitHotKey()
{
	SetTimer(0, 0, 16, (TIMERPROC)HotKeyTimer);
}

BOOL AutoRegisterHotKey(HWND hwnd, int id, UINT Ms, UINT Vk)
{//�Զ�ע���ȼ�
	if (id >= MAX_HOTKEY) {  return false; }
	Hotkey[id].id = id;
	Hotkey[id].Enabled = true;
	Hotkey[id].ms = Ms;
	Hotkey[id].vk = Vk;
	Hotkey[id].hWnd = hwnd;
	return true;
}
void AutoUnregisterHotKey(HWND hwnd, int id)
{
	(hwnd);
	Hotkey[id].Enabled = false;
}