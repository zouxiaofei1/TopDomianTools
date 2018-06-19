#pragma once
#include "stdafx.h"
VOID Restart()
{
	const int SE_SHUTDOWN_PRIVILEGE = 0x13;
	typedef int(__stdcall *PFN_RtlAdjustPrivilege)(INT, BOOL, BOOL, INT*);
	typedef int(__stdcall *PFN_ZwShutdownSystem)(INT);
	HMODULE hModule = ::LoadLibrary(_T("ntdll.dll"));
	if (hModule != NULL)
	{
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress(hModule, "RtlAdjustPrivilege"); PFN_ZwShutdownSystem pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress(hModule, "ZwShutdownSystem");
		if (pfnRtl != NULL & pfnShutdown != NULL)
		{
			int en = 0;
			int nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
			if (nRet == 0x0C000007C) nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en); //SH_SHUTDOWN = 0; //SH_RESTART = 1; //SH_POWEROFF = 2;
			nRet = pfnShutdown(1);
		}
	}
}