#include <Windows.h>
#include <TlHelp32.h>

int myfunction()
{
	//MessageBox(0, 0, 0, 0);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;//创建进程快照
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
		//MessageBox(0, pe.szExeFile, 0, 0);
		if (
			(pe.szExeFile[0]=='S'|| pe.szExeFile[0] == 's')
			&& pe.szExeFile[1] == 't'&&
			pe.szExeFile[2] == 'u'
			)
		{
			DebugActiveProcess(pe.th32ProcessID);
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = 0;
			hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
			TerminateProcess(hProcess, 1);//否则使用普通的OpenProcess和TerminateProcess
			CloseHandle(hProcess);
		}
	}
	return 0;

}