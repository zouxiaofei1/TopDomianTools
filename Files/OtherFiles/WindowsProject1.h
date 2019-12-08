#pragma once

#include "resource.h"
#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#define _CRT_RAND_S
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#include <atlstr.h>
#include <time.h>

#ifdef _DEBUG
#define VERIFY(x) { BOOL b = x; _ASSERT(b); }
#else
#define VERIFY(x) x
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

//#include <afx.h>
//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#ifndef _AFX_NO_OLE_SUPPORT
//#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
//#endif
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>                     // MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
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
		DWORD gle = GetLastError();
		if (bCloseToken)
			CloseHandle(hToken);
		_ASSERT(0);
		return false;
	}
	if (bCloseToken)
		CloseHandle(hToken);

	return true;
}

class FileInfo
{
public:
	FileInfo() { bCopyFile = false; fileVersionMS = 0; fileVersionLS = 0; fileLastWrite.dwHighDateTime = 0; fileLastWrite.dwLowDateTime = 0; }
	FileInfo(const FileInfo& in) { *this = in; }
	FileInfo& operator=(const FileInfo& in)
	{
		filenameOnly = in.filenameOnly;
		fullFilePath = in.fullFilePath;
		bCopyFile = in.bCopyFile;
		fileVersionMS = in.fileVersionMS;
		fileVersionLS = in.fileVersionLS;
		fileLastWrite.dwHighDateTime = in.fileLastWrite.dwHighDateTime;
		fileLastWrite.dwLowDateTime = in.fileLastWrite.dwLowDateTime;
		return *this;
	}

	CString filenameOnly;
	CString fullFilePath; //not serialized to remote server
	FILETIME fileLastWrite;
	DWORD fileVersionMS;
	DWORD fileVersionLS;
	bool bCopyFile; //not serialized to remote server
};


class RemMsg
{
	RemMsg(const RemMsg&); //not implemented
	RemMsg& operator=(const RemMsg&); //not implemented

	DWORD GetUniqueID();

public:
	RemMsg() { m_msgID = 0; m_pBuff = NULL; m_expectedLen = 0; m_uniqueProcessID = GetUniqueID(); }
	RemMsg(WORD msgID)
	{
		m_msgID = msgID;
		m_bResetReadItr = true;
		m_pBuff = NULL;
		m_expectedLen = 0;
		m_uniqueProcessID = GetUniqueID();
	}
	virtual ~RemMsg()
	{
		delete[] m_pBuff;
	}

	const BYTE* GetDataToSend(DWORD& totalLen);
	void SetFromReceivedData(BYTE* pData, DWORD dataLen); //returns whether all data received or not

	RemMsg& operator<<(LPCWSTR s);
	RemMsg& operator<<(bool b);
	RemMsg& operator<<(DWORD d);
	RemMsg& operator<<(__int64 d);
	RemMsg& operator<<(FILETIME d);

	RemMsg& operator>>(CString& s);
	RemMsg& operator>>(bool& b);
	RemMsg& operator>>(DWORD& d);
	RemMsg& operator>>(__int64& d);
	RemMsg& operator>>(FILETIME& d);

	WORD				m_msgID;
	std::vector<BYTE>	m_payload;
	std::vector<BYTE>::iterator m_readItr;
	bool				m_bResetReadItr;
	DWORD				m_expectedLen;
	DWORD				m_uniqueProcessID; //helps remote PAExecs differentiate from possibly multiple clients talking to it

private:
	BYTE* m_pBuff;
};


class Settings
{
	Settings(const Settings&); //not implemented
	Settings& operator=(const Settings&); //not implemented
public:
	Settings()
	{
		bCopyFiles = false;
		bForceCopy = false;
		bCopyIfNewerOrHigherVer = false;
		bDontWaitForTerminate = false;
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
		bODS = false;
		hStdOut = INVALID_HANDLE_VALUE;
		hStdIn = INVALID_HANDLE_VALUE;
		hStdErr = INVALID_HANDLE_VALUE;
		bNeedToDetachFromAdmin = false;
		bNeedToDetachFromIPC = false;
		bNeedToDeleteServiceFile = false;
		bNeedToDeleteService = false;
		bInteractive = false;
		processID = 0;
		bNoDelete = false;
		timeoutSeconds = 0;
		bNoName = false;
		sessionToInteractWith = (DWORD)-1; //not initialized
	}

	void Serialize(RemMsg& msg, bool bSave)
	{
#define CUR_SERIALIZE_VERSION 1
		if (bSave)
		{
			msg << (DWORD)CUR_SERIALIZE_VERSION;
			msg << (DWORD)allowedProcessors.size();
			for (std::vector<WORD>::iterator itr = allowedProcessors.begin(); allowedProcessors.end() != itr; itr++)
				msg << (DWORD)*itr;
			msg << bCopyFiles;
			msg << bForceCopy;
			msg << bCopyIfNewerOrHigherVer;
			msg << bDontWaitForTerminate;
			msg << bDontLoadProfile;
			msg << sessionToInteractWith;
			msg << bInteractive;
			msg << bRunElevated;
			msg << bRunLimited;
			msg << password;
			msg << user;
			msg << bUseSystemAccount;
			msg << workingDir;
			msg << bShowUIOnWinLogon;
			msg << (DWORD)priority;
			msg << app;
			msg << appArgs;
			msg << bDisableFileRedirection;
			msg << bODS;
			msg << remoteLogPath;
			msg << bNoDelete;
			msg << srcDir;
			msg << destDir;
			msg << (DWORD)srcFileInfos.size();
			for (std::vector<FileInfo>::iterator fItr = srcFileInfos.begin(); srcFileInfos.end() != fItr; fItr++)
			{
				msg << (*fItr).filenameOnly;
				msg << (*fItr).fileLastWrite;
				msg << (*fItr).fileVersionLS;
				msg << (*fItr).fileVersionMS;
				//bCopyFiles is NOT sent
				//fullFilePath is NOT sent
			}
			msg << (DWORD)destFileInfos.size();
			for (std::vector<FileInfo>::iterator fItr = destFileInfos.begin(); destFileInfos.end() != fItr; fItr++)
			{
				msg << (*fItr).filenameOnly;
				msg << (*fItr).fileLastWrite;
				msg << (*fItr).fileVersionLS;
				msg << (*fItr).fileVersionMS;
				//bCopyFiles is NOT sent
				//fullFilePath is NOT sent
			}
			msg << timeoutSeconds;
		}
		else
		{
			allowedProcessors.clear();

			DWORD version = 0;
			msg >> version;

			DWORD num = 0;
			msg >> num;
			for (DWORD i = 0; i < num; i++)
			{
				DWORD proc = 0;
				msg >> proc;
				allowedProcessors.push_back((WORD)proc);
			}
			msg >> bCopyFiles;
			msg >> bForceCopy;
			msg >> bCopyIfNewerOrHigherVer;
			msg >> bDontWaitForTerminate;
			msg >> bDontLoadProfile;
			msg >> sessionToInteractWith;
			msg >> bInteractive;
			msg >> bRunElevated;
			msg >> bRunLimited;
			msg >> password;
			msg >> user;
			msg >> bUseSystemAccount;
			msg >> workingDir;
			msg >> bShowUIOnWinLogon;
			msg >> (DWORD&)priority;
			msg >> app;
			msg >> appArgs;
			msg >> bDisableFileRedirection;
			msg >> bODS;
			msg >> remoteLogPath;
			msg >> bNoDelete;
			msg >> srcDir;
			msg >> destDir;

			msg >> num;
			for (DWORD i = 0; i < num; i++)
			{
				FileInfo fi;
				msg >> fi.filenameOnly;
				msg >> fi.fileLastWrite;
				msg >> fi.fileVersionLS;
				msg >> fi.fileVersionMS;
				fi.bCopyFile = false; //not known whether it will be copied yet
				//bCopyFiles is NOT sent
				//fullFilePath is NOT sent
				srcFileInfos.push_back(fi);
			}

			msg >> num;
			for (DWORD i = 0; i < num; i++)
			{
				FileInfo fi;
				msg >> fi.filenameOnly;
				msg >> fi.fileLastWrite;
				msg >> fi.fileVersionLS;
				msg >> fi.fileVersionMS;
				fi.bCopyFile = false; //not known whether it will be copied yet
				//bCopyFiles is NOT sent
				//fullFilePath is NOT sent
				destFileInfos.push_back(fi);
			}
			msg >> timeoutSeconds;
		}
	}

	bool ResolveFilePaths();

	std::vector<WORD> allowedProcessors; //empty means any
	bool bCopyFiles;
	bool bForceCopy;
	bool bCopyIfNewerOrHigherVer;
	bool bDontWaitForTerminate;
	bool bDontLoadProfile;
	DWORD sessionToInteractWith;
	bool bInteractive;
	bool bRunElevated;
	bool bRunLimited;
	CString password;
	CString user;
	bool bUseSystemAccount;
	CString workingDir;
	bool bShowUIOnWinLogon;
	int priority;
	CString app;
	CString appArgs;
	bool bDisableFileRedirection;
	bool bODS;
	CString remoteLogPath;
	bool bNoDelete;
	CString srcDir;
	CString destDir;
	std::vector<FileInfo> srcFileInfos; //index 0 will be source of app, but may not match app if -csrc or -clist are being used
	std::vector<FileInfo> destFileInfos; //index 0 will be app
	DWORD timeoutSeconds;

	//NOT TRANSMITTED
	DWORD remoteCompConnectTimeoutSec;
	std::vector<CString> computerList; //run locally if empty
	HANDLE hProcess;
	DWORD processID;
	HANDLE hUserProfile; //call UnloadUserProfile when done
	HANDLE hUser;
	CString localLogPath;
	HANDLE hStdOut;
	HANDLE hStdIn;
	HANDLE hStdErr;
	bool bNeedToDetachFromAdmin;
	bool bNeedToDetachFromIPC;
	bool bNeedToDeleteServiceFile;
	bool bNeedToDeleteService;
	bool bNoName;
	CString serviceName;
};
