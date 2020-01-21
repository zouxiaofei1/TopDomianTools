#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#define _CRT_RAND_S
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS 
#include <time.h>
#include <Windows.h>
#include <WinSvc.h>
#include <vector>
#include <ProfInfo.h>
#include <UserEnv.h>
#include <sddl.h>
#include <Psapi.h>
#define BAD_HANDLE(x) ((INVALID_HANDLE_VALUE == x) || (NULL == x))
typedef struct
{
	DWORD origSessionID;
	HANDLE hUser;
	bool bPreped;
}CleanupInteractive;
#include <WtsApi32.h>
void CleanUpInteractiveProcess(CleanupInteractive* pCI)
{
	SetTokenInformation(pCI->hUser, TokenSessionId, &pCI->origSessionID, sizeof(pCI->origSessionID));


}
bool EnablePrivilege(LPCWSTR privilegeStr, HANDLE hToken /* = NULL */)
{
	TOKEN_PRIVILEGES  tp;         // token privileges
	LUID              luid;
	bool				bCloseToken = false;

	if (NULL == hToken)
	{
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
			return false;
		}
		bCloseToken = true;
	}

	if (!LookupPrivilegeValue(NULL, privilegeStr, &luid))
	{
		if (bCloseToken)
			CloseHandle(hToken);
		return false;
	}

	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Adjust Token privileges
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		if (bCloseToken)
			CloseHandle(hToken);
		return false;
	}
	if (bCloseToken)
		CloseHandle(hToken);

	return true;
}

class Settings
{
	Settings(const Settings&); //not implemented
public:
	Settings()
	{
		bDontLoadProfile = false;
		bRunElevated = false;
		bRunLimited = false;
		remoteCompConnectTimeoutSec = 0;
		bUseSystemAccount = false;
		bShowUIOnWinLogon = false;
		priority = NORMAL_PRIORITY_CLASS;
		hProcess = INVALID_HANDLE_VALUE;
		hUserProfile = INVALID_HANDLE_VALUE; //call UnloadUserProfile when done
		hUser = INVALID_HANDLE_VALUE;
		bDisableFileRedirection = false;
		hStdOut = INVALID_HANDLE_VALUE;
		hStdIn = INVALID_HANDLE_VALUE;
		hStdErr = INVALID_HANDLE_VALUE;
		bInteractive = false;
		processID = 0;
		bNoName = false;
		sessionToInteractWith = (DWORD)-1; //not initialized
	}

	bool bDontLoadProfile;
	DWORD sessionToInteractWith;
	bool bInteractive;
	bool bRunElevated;
	bool bRunLimited;
	wchar_t password[300];
	wchar_t user[300];
	bool bUseSystemAccount;
	wchar_t workingDir[300];
	bool bShowUIOnWinLogon;
	int priority;
	wchar_t app[300];
	wchar_t appArgs[300];
	bool bDisableFileRedirection;

	//NOT TRANSMITTED
	DWORD remoteCompConnectTimeoutSec;
	HANDLE hProcess;
	DWORD processID;
	HANDLE hUserProfile; //call UnloadUserProfile when done
	HANDLE hUser;
	HANDLE hStdOut;
	HANDLE hStdIn;
	HANDLE hStdErr;
	bool bNoName;
};
#pragma warning(disable:4996)
#define MAX_LOADSTRING 100

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "psapi.lib")


wchar_t name[300];
void GetTokenUserSID(HANDLE hToken)
{
	DWORD tmp = 0;
	wchar_t  tokenUserBuf[1024] = { 0 };
	DWORD userTokenSize = 1024;

	TOKEN_USER* userToken = (TOKEN_USER*)tokenUserBuf;

	if (GetTokenInformation(hToken, TokenUser, userToken, userTokenSize, &tmp))
	{
		WCHAR* pSidString = NULL;
		if (ConvertSidToStringSid(userToken->User.Sid, &pSidString))
			wcscpy_s(name, pSidString);
		if (NULL != pSidString)
			LocalFree(pSidString);
	}
}


HANDLE GetLocalSystemProcessToken()
{
	DWORD pids[1024 * 10] = { 0 }, cbNeeded = 0, cProcesses = 0;

	if (!EnumProcesses(pids, sizeof(pids), &cbNeeded))
	{
		return NULL;
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);
	for (DWORD i = 0; i < cProcesses; ++i)
	{
		DWORD gle = 0;
		DWORD dwPid = pids[i];
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
		if (hProcess)
		{
			HANDLE hToken = 0;
			if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_READ | TOKEN_IMPERSONATE | TOKEN_QUERY_SOURCE | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_EXECUTE, &hToken))
			{
				try
				{
					GetTokenUserSID(hToken);

					//const wchar_t arg[] = L"NT AUTHORITY\\";
					//if(0 == _wcsnicmp(name, arg, sizeof(arg)/sizeof(arg[0])-1))

					if (wcscmp(name, L"S-1-5-18") == 0) //Well known SID for Local System
					{
						CloseHandle(hProcess);
						return hToken;
					}
				}
				catch (...)
				{
				}
			}
			else
				gle = GetLastError();
			CloseHandle(hToken);
		}
		else
			gle = GetLastError();
		CloseHandle(hProcess);
	}
	return NULL;
}

void Duplicate(HANDLE& h)
{
	HANDLE hDupe = NULL;
	if (DuplicateTokenEx(h, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hDupe))
	{
		CloseHandle(h);
		h = hDupe;
		hDupe = NULL;
	}
}

typedef DWORD(WINAPI* WTSGetActiveConsoleSessionIdProc)(void);

DWORD GetInteractiveSessionID()
{
	// Get the active session ID.
	DWORD   SessionId = 0;
	PWTS_SESSION_INFO pSessionInfo;
	DWORD   Count = 0;

	if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &Count))
	{
		for (DWORD i = 0; i < Count; i++)
		{
			if (pSessionInfo[i].State == WTSActive)	//Here is
			{
				SessionId = pSessionInfo[i].SessionId;
			}
		}
		WTSFreeMemory(pSessionInfo);
	}

	if (0 == SessionId)
	{
		static WTSGetActiveConsoleSessionIdProc pWTSGetActiveConsoleSessionId = NULL;
		if (NULL == pWTSGetActiveConsoleSessionId)
		{
			HMODULE hMod = LoadLibrary(L"Kernel32.dll"); //GLOK
			if (NULL != hMod)
			{
				pWTSGetActiveConsoleSessionId = (WTSGetActiveConsoleSessionIdProc)GetProcAddress(hMod, "WTSGetActiveConsoleSessionId");
			}
		}

		if (NULL != pWTSGetActiveConsoleSessionId) //not supported on Win2K
			SessionId = pWTSGetActiveConsoleSessionId(); //we fall back on this if needed since it apparently doesn't always work

	}

	return SessionId;
}
wchar_t a[] = L"WinSta0\\Default", b2[] = L"winsta0\\Winlogon";
BOOL PrepForInteractiveProcess(Settings& settings, CleanupInteractive* pCI, DWORD sessionID)
{
	pCI->bPreped = true;
	//settings.hUser is set as the -u user, Local System (from -s) or as the account the user originally launched PAExec with

	//figure out which session we need to go into
	Duplicate(settings.hUser);
	pCI->hUser = settings.hUser;

	DWORD targetSessionID = sessionID;

	if ((DWORD)-1 == settings.sessionToInteractWith)
	{
		targetSessionID = GetInteractiveSessionID();
	}

	//if(FALSE == WTSQueryUserToken(targetSessionID, &settings.hUser))
	//	Log(L"Failed to get user from session ", GetLastError());

	//Duplicate(settings.hUser, __FILE__, __LINE__);

	DWORD returnedLen = 0;
	GetTokenInformation(settings.hUser, TokenSessionId, &pCI->origSessionID, sizeof(pCI->origSessionID), &returnedLen);

	EnablePrivilege(SE_TCB_NAME, settings.hUser);

	SetTokenInformation(settings.hUser, TokenSessionId, &targetSessionID, sizeof(targetSessionID));
	return TRUE;

}

void GetUserDomain(LPCWSTR userIn, wchar_t* user, wchar_t* domain)
{
	//run as specified user
	wchar_t tmp[300]; wcscpy_s(tmp, userIn);
	LPCWSTR userStr = NULL, domainStr = NULL;
	if (NULL != wcschr(userIn, L'@'))
		userStr = userIn; //leave domain as NULL
	else
	{
		if (NULL != wcschr(userIn, L'\\'))
		{
			domainStr = wcstok(tmp, L"\\");
			userStr = wcstok(NULL, L"\\");
		}
		else
		{
			//no domain given
			userStr = userIn;
			domainStr = L".";
		}
	}
	wcscpy(user, userStr);
	wcscpy(domain, domainStr);
}



bool StartProcess(Settings& settings)
{
	settings.hUser = GetLocalSystemProcessToken();
	//
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	if (!BAD_HANDLE(settings.hStdErr))
	{
		si.hStdError = settings.hStdErr;//useless
		si.hStdInput = settings.hStdIn;
		si.hStdOutput = settings.hStdOut;
		si.dwFlags |= STARTF_USESTDHANDLES;
	}

	wchar_t path[300];
	wcscpy_s(path, settings.app);
	if (wcslen(settings.appArgs) != 0)
	{
		wcscat_s(path, L" ");
		wcscat_s(path, settings.appArgs);
	}

	LPCWSTR startingDir = NULL;
	if (wcslen(settings.workingDir) != 0)
		startingDir = settings.workingDir;

	DWORD launchGLE = 0;

	CleanupInteractive ci = { 0 };

	if (settings.bInteractive || settings.bShowUIOnWinLogon)
	{
		PrepForInteractiveProcess(settings, &ci, settings.sessionToInteractWith);

		if (NULL == si.lpDesktop)
			si.lpDesktop = a;
		if (settings.bShowUIOnWinLogon)
			si.lpDesktop = b2;
	}

	DWORD dwFlags = CREATE_SUSPENDED | CREATE_NEW_CONSOLE;

	LPVOID pEnvironment = NULL;
	CreateEnvironmentBlock(&pEnvironment, settings.hUser, TRUE);
	if (NULL != pEnvironment)
		dwFlags |= CREATE_UNICODE_ENVIRONMENT;

	wchar_t user[301], domain[301];
	GetUserDomain(settings.user, user, domain);

	BOOL bLaunched = FALSE;
	
	if (settings.bUseSystemAccount)
	{
		EnablePrivilege(SE_IMPERSONATE_NAME, 0);
		ImpersonateLoggedOnUser(settings.hUser);
		EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, 0);
		EnablePrivilege(SE_INCREASE_QUOTA_NAME, 0);
		bLaunched = CreateProcessAsUser(settings.hUser, NULL, path, NULL, NULL, TRUE, dwFlags, pEnvironment, startingDir, &si, &pi);
		launchGLE = GetLastError();

		RevertToSelf();
	}
	else
	{
		if (wcslen(settings.user) != 0) //launching as a specific user
		{
			if (false == settings.bRunLimited)
			{
				bLaunched = CreateProcessWithLogonW(user, (wcslen(domain) == 0) ? NULL : domain, settings.password, settings.bDontLoadProfile ? 0 : LOGON_WITH_PROFILE, NULL, path, dwFlags, pEnvironment, startingDir, &si, &pi);
				launchGLE = GetLastError();
			}
			else
				bLaunched = FALSE; //force to run with CreateProcessAsUser so rights can be limited

			//CreateProcessWithLogonW can't be called from LocalSystem on Win2003 and earlier, so LogonUser/CreateProcessAsUser must be used. Might as well try for everyone
			if ((FALSE == bLaunched) && !BAD_HANDLE(settings.hUser))
			{
				EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, 0);
				EnablePrivilege(SE_INCREASE_QUOTA_NAME, 0);
				EnablePrivilege(SE_IMPERSONATE_NAME, 0);
				ImpersonateLoggedOnUser(settings.hUser);
				bLaunched = CreateProcessAsUser(settings.hUser, NULL, path, NULL, NULL, TRUE, CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE, pEnvironment, startingDir, &si, &pi);
				if (0 == GetLastError())
					launchGLE = 0; //mark as successful, otherwise return our original error
				RevertToSelf();
			}
		}
		else
		{
			EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, 0);
			EnablePrivilege(SE_INCREASE_QUOTA_NAME, 0);
			EnablePrivilege(SE_IMPERSONATE_NAME, 0);

			if (NULL != settings.hUser)
				bLaunched = CreateProcessAsUser(settings.hUser, NULL, path, NULL, NULL, TRUE, dwFlags, pEnvironment, startingDir, &si, &pi);
			if (FALSE == bLaunched)
				bLaunched = CreateProcess(NULL, path, NULL, NULL, TRUE, dwFlags, pEnvironment, startingDir, &si, &pi);
			launchGLE = GetLastError();
		}
	}
	//
	if (bLaunched)
	{
		settings.hProcess = pi.hProcess;
		settings.processID = pi.dwProcessId;

		SetPriorityClass(pi.hProcess, settings.priority);
		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
	}

	if (ci.bPreped)
		CleanUpInteractiveProcess(&ci);


	if (NULL != pEnvironment)
		DestroyEnvironmentBlock(pEnvironment);
	pEnvironment = NULL;

	if (!BAD_HANDLE(settings.hUser))
	{
		CloseHandle(settings.hUser);
		settings.hUser = NULL;
	}

	return bLaunched ? true : false;
}

void myPAExec(bool cmd)
{
	Settings set;
	if (cmd)
	{
		wcscpy_s(set.app, L"\"C:\\windows\\system32\\cmd.exe\"");
	}
	else
	{
		wcscpy_s(set.app, L"\"");
		wchar_t tmpstr[300]; GetModuleFileName(NULL, tmpstr, 300);
		wcscat_s(set.app, tmpstr);
		wcscat_s(set.app, L"\"");
		wcscpy_s(set.appArgs, L"-top");
		set.bShowUIOnWinLogon = true;
	}
	set.bInteractive = true;
	set.bUseSystemAccount = true;
	set.sessionToInteractWith = (DWORD)-1;
	wcscpy_s(set.user, L"SYSTEM");
	StartProcess(set);
	return;
}