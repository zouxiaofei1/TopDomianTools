#pragma once
#include "stdafx.h"

void s(int cathy)
{
	wchar_t Cathytmp[34];
	_itow_s(cathy, Cathytmp, 10);
	MessageBox(NULL, Cathytmp, L"", NULL);
}
void s(LPCSTR cathy)
{
	MessageBoxA(NULL, cathy, "", NULL);
}