// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Wtsapi32.lib")
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

CString GetTokenUserSID(HANDLE hToken)
{
	DWORD tmp = 0;
	CString userName;
	DWORD sidNameSize = 64;
	std::vector<WCHAR> sidName;
	sidName.resize(sidNameSize);

	DWORD sidDomainSize = 64;
	std::vector<WCHAR> sidDomain;
	sidDomain.resize(sidNameSize);

	DWORD userTokenSize = 1024;
	std::vector<WCHAR> tokenUserBuf;
	tokenUserBuf.resize(userTokenSize);

	TOKEN_USER* userToken = (TOKEN_USER*)&tokenUserBuf.front();

	if (GetTokenInformation(hToken, TokenUser, userToken, userTokenSize, &tmp))
	{
		WCHAR* pSidString = NULL;
		if (ConvertSidToStringSid(userToken->User.Sid, &pSidString))
			userName = pSidString;
		if (NULL != pSidString)
			LocalFree(pSidString);
	}
	else
		_ASSERT(0);

	return userName;
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
					CString name = GetTokenUserSID(hToken);

					//const wchar_t arg[] = L"NT AUTHORITY\\";
					//if(0 == _wcsnicmp(name, arg, sizeof(arg)/sizeof(arg[0])-1))

					if (name == L"S-1-5-18") //Well known SID for Local System
					{
						CloseHandle(hProcess);
						return hToken;
					}
				}
				catch (...)
				{
					_ASSERT(0);
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
CString StrFormat(LPCTSTR pFormat, ...) //returns a formatted CString.  Inspired by .NET's String.Format
{
	CString result;
	DWORD size = max(4096, (DWORD)_tcslen(pFormat) + 4096);

	_ASSERT(NULL == wcsstr(pFormat, L"{0}")); //StrFormat2 should have been used?? GLOK

	try
	{
		while (true)
		{
			va_list pArg;
			va_start(pArg, pFormat);
			int res = _vsntprintf_s(result.GetBuffer(size), size, _TRUNCATE, pFormat, pArg);
			va_end(pArg);
			if (res >= 0)
			{
				result.ReleaseBuffer();
				return result;
			}
			else
			{
				result.ReleaseBuffer(1);
				size += 8192;
				if (size > (12 * 1024 * 1024))
				{
					_ASSERT(0);
					CString s = L"<error - string too long -- "; //GLOK
					s += pFormat;
					s += L">"; //GLOK
					return s;
				}
			}
		}
	}
	catch (...)
	{
		_ASSERT(0);
		CString res = L"<string format exception: ["; //GLOK
		if (NULL != pFormat)
			res += pFormat;
		else
			res += L"(null)"; //GLOK
		res += L"]>"; //GLOK
		res.Replace(L'%', L'{'); //so no further formatting is attempted GLOK
		return res;
	}
}
void Duplicate(HANDLE& h, LPCSTR file, int line)
{
	HANDLE hDupe = NULL;
	if (DuplicateTokenEx(h, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hDupe))
	{
		CloseHandle(h);
		h = hDupe;
		hDupe = NULL;
	}
	else
	{
		DWORD gle = GetLastError();
		_ASSERT(0);
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
wchar_t a[] = L"WinSta0\\Default", b2 []= L"winsta0\\Winlogon";
BOOL PrepForInteractiveProcess(Settings& settings, CleanupInteractive* pCI, DWORD sessionID)
{
	pCI->bPreped = true;
	//settings.hUser is set as the -u user, Local System (from -s) or as the account the user originally launched PAExec with

	//figure out which session we need to go into
	Duplicate(settings.hUser, __FILE__, __LINE__);
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

void GetUserDomain(LPCWSTR userIn, CString& user, CString& domain)
{
	//run as specified user
	CString tmp = userIn;
	LPCWSTR userStr = NULL, domainStr = NULL;
	if (NULL != wcschr(userIn, L'@'))
		userStr = userIn; //leave domain as NULL
	else
	{
		if (NULL != wcschr(userIn, L'\\'))
		{
			domainStr = wcstok(tmp.LockBuffer(), L"\\");
			userStr = wcstok(NULL, L"\\");
		}
		else
		{
			//no domain given
			userStr = userIn;
			domainStr = L".";
		}
	}
	user = userStr;
	domain = domainStr;
}



bool StartProcess(Settings& settings, HANDLE hCmdPipe)
{
	//Launching as one of:
	//1. System Account
	//2. Specified account (or limited account)
	//3. As current process

	DWORD gle = 0;

	BOOL bLoadedProfile = FALSE;
	//PROFILEINFO profile = { 0 };
	//profile.dwSize = sizeof(profile);
	//profile.lpUserName = (LPWSTR)(LPCWSTR)settings.user;
	//profile.dwFlags = PI_NOUI;
	settings.hUser = GetLocalSystemProcessToken();
	//if (false == GetUserHandle(settings, bLoadedProfile, profile, hCmdPipe))
		//return false;

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

	CString path = StrFormat(L"\"%s\"", settings.app);
	if (FALSE == settings.appArgs.IsEmpty())
	{
		path += L" ";
		path += settings.appArgs;
	}

	LPCWSTR startingDir = NULL;
	if (FALSE == settings.workingDir.IsEmpty())
		startingDir = settings.workingDir;

	DWORD launchGLE = 0;

	CleanupInteractive ci = { 0 };

	if (settings.bInteractive || settings.bShowUIOnWinLogon)
	{
		BOOL b = PrepForInteractiveProcess(settings, &ci, settings.sessionToInteractWith);
		
		if (NULL == si.lpDesktop)
			si.lpDesktop = a;
		if (settings.bShowUIOnWinLogon)
			si.lpDesktop = b2;
		//Log(StrFormat(L"Using desktop: %s", si.lpDesktop), false);
		//http://blogs.msdn.com/b/winsdk/archive/2009/07/14/launching-an-interactive-process-from-windows-service-in-windows-vista-and-later.aspx
		//indicates desktop names are case sensitive
	}

	DWORD dwFlags = CREATE_SUSPENDED | CREATE_NEW_CONSOLE;

	LPVOID pEnvironment = NULL;
	VERIFY(CreateEnvironmentBlock(&pEnvironment, settings.hUser, TRUE));
	if (NULL != pEnvironment)
		dwFlags |= CREATE_UNICODE_ENVIRONMENT;

	CString user, domain;
	GetUserDomain(settings.user, user, domain);

	BOOL bLaunched = FALSE;

	if (settings.bUseSystemAccount)
	{
		EnablePrivilege(SE_IMPERSONATE_NAME,0);
		BOOL bImpersonated = ImpersonateLoggedOnUser(settings.hUser);
		if (FALSE == bImpersonated)
		{
			_ASSERT(bImpersonated);
		}
		EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME,0);
		EnablePrivilege(SE_INCREASE_QUOTA_NAME,0);
		bLaunched = CreateProcessAsUser(settings.hUser, NULL, path.LockBuffer(), NULL, NULL, TRUE, dwFlags, pEnvironment, startingDir, &si, &pi);
		launchGLE = GetLastError();
		path.UnlockBuffer();

		RevertToSelf();
	}
	else
	{
		if (FALSE == settings.user.IsEmpty()) //launching as a specific user
		{
			if (false == settings.bRunLimited)
			{
				bLaunched = CreateProcessWithLogonW(user, domain.IsEmpty() ? NULL : domain, settings.password, settings.bDontLoadProfile ? 0 : LOGON_WITH_PROFILE, NULL, path.LockBuffer(), dwFlags, pEnvironment, startingDir, &si, &pi);
				launchGLE = GetLastError();
				path.UnlockBuffer();
			}
			else
				bLaunched = FALSE; //force to run with CreateProcessAsUser so rights can be limited

			//CreateProcessWithLogonW can't be called from LocalSystem on Win2003 and earlier, so LogonUser/CreateProcessAsUser must be used. Might as well try for everyone
			if ((FALSE == bLaunched) && !BAD_HANDLE(settings.hUser))
			{
				EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME,0);
				EnablePrivilege(SE_INCREASE_QUOTA_NAME,0);
				EnablePrivilege(SE_IMPERSONATE_NAME,0);
				BOOL bImpersonated = ImpersonateLoggedOnUser(settings.hUser);
				if (FALSE == bImpersonated)
				{
					_ASSERT(bImpersonated);
				}

				bLaunched = CreateProcessAsUser(settings.hUser, NULL, path.LockBuffer(), NULL, NULL, TRUE, CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE, pEnvironment, startingDir, &si, &pi);
				if (0 == GetLastError())
					launchGLE = 0; //mark as successful, otherwise return our original error
				path.UnlockBuffer();
				RevertToSelf();
			}
		}
		else
		{
			EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME,0);
			EnablePrivilege(SE_INCREASE_QUOTA_NAME,0);
			EnablePrivilege(SE_IMPERSONATE_NAME,0);

			if (NULL != settings.hUser)
				bLaunched = CreateProcessAsUser(settings.hUser, NULL, path.LockBuffer(), NULL, NULL, TRUE, dwFlags, pEnvironment, startingDir, &si, &pi);
			if (FALSE == bLaunched)
				bLaunched = CreateProcess(NULL, path.LockBuffer(), NULL, NULL, TRUE, dwFlags, pEnvironment, startingDir, &si, &pi);
			launchGLE = GetLastError();


			path.UnlockBuffer();
		}
	}

	if (bLaunched)
	{
		settings.hProcess = pi.hProcess;
		settings.processID = pi.dwProcessId;

		if (false == settings.allowedProcessors.empty())
		{
			DWORD sysMask = 0, procMask = 0;
			VERIFY(GetProcessAffinityMask(pi.hProcess, &procMask, &sysMask));
			procMask = 0;
			for (std::vector<WORD>::iterator itr = settings.allowedProcessors.begin(); settings.allowedProcessors.end() != itr; itr++)
			{
				DWORD bit = 1;
				bit = bit << (*itr - 1);
				procMask |= bit & sysMask;
			}
			VERIFY(SetProcessAffinityMask(pi.hProcess, procMask));
		}

		VERIFY(SetPriorityClass(pi.hProcess, settings.priority));
		ResumeThread(pi.hThread);
		VERIFY(CloseHandle(pi.hThread));
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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	Settings set;
	set.app = L"C:\\windows\\system32\\cmd.exe";
	set.bInteractive = true;
	set.sessionToInteractWith = (DWORD)-1;
	//set.bShowUIOnWinLogon = true;
	set.user = L"SYSTEM";
	StartProcess(set, 0);
	return 0;
}


