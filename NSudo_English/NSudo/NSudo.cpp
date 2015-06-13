//Mouri_Naruto NSudo 2.1 (Build 809)
//(C) CopyRight 2015 Mouri_Naruto

#include "stdafx.h"
#include "NSudo.h"

#define NSudo_Title L"Mouri_Naruto NSudo"
#define NSudo_Version L"2.1 (Build 809)"
#define NSudo_CopyRight L"(C)2015 Mouri_Naruto. All rights reserved."

#define ReturnMessage(lpText) MessageBoxW(NULL, (lpText), NSudo_Title, NULL)
bool GetPrivilege(HANDLE ProcessHandle, LPCWSTR lpName);
bool GetToken(HANDLE TokenHandle, LPCWSTR lpName);
void GetAllTokens(HANDLE TokenHandle);
void About();
void GetSystemPrivilege(LPWSTR szCMDLine);
void GetTIToken(LPWSTR szCMDLine);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	if (!GetPrivilege(GetCurrentProcess(),SE_DEBUG_NAME))
	{
		ReturnMessage(L"Failed to get debug Privilege");
		return -1;
	}
	
	wchar_t szCMDPath[260], szCMDLineSystem[260], szCMDLineTI[260];

	GetSystemWindowsDirectoryW(szCMDPath, 260); //��ȡWindowsĿ¼

	if (GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "ZwWow64ReadVirtualMemory64")) //�ж��Ƿ���64λOS
	{
		wcscat_s(szCMDPath, 260, L"\\SysNative\\");  //64������ʾ��·��
	}
	else
	{
		wcscat_s(szCMDPath, 260, L"\\System32\\");  //32������ʾ��·��
	}

	wcscpy_s(szCMDLineSystem, 260, szCMDPath);
	wcscpy_s(szCMDLineTI, 260, szCMDPath);
	wcscat_s(szCMDLineSystem, 260, L"cmd.exe /K title " NSudo_Title L" - [System] & echo " NSudo_Title  L" " NSudo_Version L" & echo " NSudo_CopyRight);
	wcscat_s(szCMDLineTI, 260, L"cmd.exe /K title " NSudo_Title L" - [System With TrustedInstaller Token] & echo " NSudo_Title  L" " NSudo_Version L" & echo " NSudo_CopyRight);

	if (wcscmp(L"-TiShell", lpCmdLine) == 0)
	{	
		GetTIToken(szCMDLineTI);
		ExitProcess(0);
	}
	else if (wcscmp(L"-TI", lpCmdLine) == 0 || wcscmp(L"-ti", lpCmdLine) == 0 || wcscmp(L"-Ti", lpCmdLine) == 0)
	{
		wchar_t szCMDLine[260];
		GetModuleFileNameW(NULL, szCMDLine, 260);
		wcscat_s(szCMDLine, 260, L" -TiShell");
		GetSystemPrivilege(szCMDLine);
		ExitProcess(0);
	}
	else if (wcscmp(L"-System", lpCmdLine) == 0 || wcscmp(L"-system", lpCmdLine) == 0 || wcscmp(L"-SYSTEM", lpCmdLine) == 0)
	{
		GetSystemPrivilege(szCMDLineSystem);
		ExitProcess(0);
	}
	else if (wcscmp(L"-Help", lpCmdLine) == 0 || wcscmp(L"-help", lpCmdLine) == 0 || wcscmp(L"-HELP", lpCmdLine) == 0)
	{
		About();
		ExitProcess(0);
	}
	else
	{
		int nButtonPressed = 0, nRadioButton = 0;
		TASKDIALOGCONFIG config = { 0 };
		const TASKDIALOG_BUTTON buttons[] = {
			{ 101, L"Run Command Prompt with the selected privileges(&C)" },
			{ 102, L"About and Command Help(&A)" },
		};
		const TASKDIALOG_BUTTON choosebuttons[] = {
			{ 201, L"System With TrustedInstaller Token(&T)" },
			{ 202, L"System(&S)" },
		};

		config.dwFlags = TDF_USE_HICON_MAIN | TDF_USE_HICON_FOOTER | TDF_EXPAND_FOOTER_AREA | TDF_ALLOW_DIALOG_CANCELLATION;
		config.cbSize = sizeof(config);
		config.pszWindowTitle = NSudo_Title L" " NSudo_Version;
		config.cRadioButtons = ARRAYSIZE(choosebuttons);
		config.pRadioButtons = choosebuttons;
		config.pszMainInstruction = L"Please select the authority you need";
		config.hInstance = hInstance;
		config.cButtons = ARRAYSIZE(buttons);
		config.pButtons = buttons;
		config.hMainIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_NSUDO));
		LoadIconWithScaleDown(NULL, IDI_WARNING,GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),&config.hFooterIcon);
		config.pszFooter = L"Warning: If you think BIOS is a new plant-based fuel, NSudo may not be right for you.";

		TaskDialogIndirect(&config, &nButtonPressed, &nRadioButton, NULL);

		switch (nButtonPressed)
		{
		case 101:
			if (nRadioButton == 201)
			{
				wchar_t szCMDLine[260];
				GetModuleFileNameW(NULL, szCMDLine, 260);
				wcscat_s(szCMDLine, 260, L" -TiShell");
				GetSystemPrivilege(szCMDLine);
			}
			else
			{
				GetSystemPrivilege(szCMDLineSystem);
			}
			ExitProcess(0);
		case 102:
			About();
			_tWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
		default:
			break;
		}
	}
	
	return 0;
}

void About()
{
	ReturnMessage(NSudo_Title  L" " NSudo_Version L"\n" NSudo_CopyRight L"\n\n"
		L"NSudo [Options]\n\nOptions:\n[ -TI ] Run Command Prompt from System With TrustedInstaller Token\n[ -System ] Run Command Prompt from System\n[ -Help ] Show Help");
}

void GetSystemPrivilege(LPWSTR szCMDLine)
{
	DWORD dwUserSessionId; //�û��ỰID
	DWORD dwWinLogonPID = -1; //winlogon.exe��ProcessID

	if ((dwUserSessionId = WTSGetActiveConsoleSessionId()) == 0xFFFFFFFF) //����û��ỰID
	{
		ReturnMessage( L"Failed to get user session");
		return;
	}

	PROCESSENTRY32W ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //����CreateToolhelp32Snapshot�������н���
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		ReturnMessage(L"Failed to call CreateToolhelp32Snapshot");
		return;
	}

	if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
	{
		do
		{
			if (wcscmp(L"winlogon.exe", ProcessEntry.szExeFile) == 0) //Ѱ��winlogon����
			{
				DWORD dwSessionID;
				if (ProcessIdToSessionId(ProcessEntry.th32ProcessID, &dwSessionID)) //��ȡwinlogon�ĻỰID
				{
					if (dwSessionID != dwUserSessionId) continue; //�ж��Ƿ��ǵ�ǰ�û�ID
					dwWinLogonPID = ProcessEntry.th32ProcessID;
					break;
				}
			}
		} while (Process32NextW(hSnapshot, &ProcessEntry));
	}
	CloseHandle(hSnapshot);

	if (dwWinLogonPID == -1)
	{
		ReturnMessage(L"Failed to get winlogon.exe PID");
		return;
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwWinLogonPID);
	if (hProc != NULL)
	{
		HANDLE hToken, hDupToken;
		if (OpenProcessToken(hProc, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken))
		{
			if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hDupToken))
			{
				LPVOID lpEnv; //������
				if (CreateEnvironmentBlock(&lpEnv, hToken, 1))
				{
					GetAllTokens(hDupToken);
					
					STARTUPINFOW StartupInfo = { 0 };
					PROCESS_INFORMATION ProcessInfo = { 0 };
					StartupInfo.lpDesktop = L"WinSta0\\Default";
					if (!CreateProcessWithTokenW(
						hDupToken,
						LOGON_WITH_PROFILE,
						NULL,
						szCMDLine,
						CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
						lpEnv,
						NULL,
						&StartupInfo,
						&ProcessInfo))
					{
						if (!CreateProcessAsUserW(hDupToken,
							NULL,
							szCMDLine,
							NULL,
							NULL,
							NULL,
							CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
							lpEnv,
							NULL,
							&StartupInfo,
							&ProcessInfo))
						{
							ReturnMessage(L"Failed to Create the process");
						}
					}
					DestroyEnvironmentBlock(lpEnv);
				}
				else ReturnMessage(L"Failed to Call CreateEnvironmentBlock of winlogon.exe");
				CloseHandle(hDupToken);
			}
			else ReturnMessage(L"Failed to Call DuplicateTokenEx of winlogon.exe");
			CloseHandle(hToken);
		}
		else ReturnMessage(L"Failed to Call OpenProcessToken of winlogon.exe");
		CloseHandle(hProc);
	}
	else ReturnMessage(L"Failed to Call OpenProcess of winlogon.exe");
}

void GetTIToken(LPWSTR szCMDLine)
{
	SC_HANDLE hSC = OpenSCManagerW(NULL, NULL, GENERIC_EXECUTE);
	if (hSC != NULL)
	{
		SC_HANDLE hSvc = OpenServiceW(hSC, L"TrustedInstaller", SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
		if (hSvc != NULL)
		{
			SERVICE_STATUS status;
			if (QueryServiceStatus(hSvc, &status))
			{
				if (status.dwCurrentState == SERVICE_STOPPED)
				{
					// ��������
					if (StartServiceW(hSvc, NULL, NULL) == FALSE)
					{
						ReturnMessage(L"Failed to Start TrustedInstaller Service");
					}
					// �ȴ���������
					while (::QueryServiceStatus(hSvc, &status) == TRUE)
					{
						Sleep(status.dwWaitHint);
						if (status.dwCurrentState == SERVICE_RUNNING)
						{
							break;
						}
					}
				}
			}
			CloseServiceHandle(hSvc);
		}
		CloseServiceHandle(hSC);
	}

	DWORD dwTIPID = -1; //winlogon.exe��ProcessID

	PROCESSENTRY32W ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //����CreateToolhelp32Snapshot�������н���
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		ReturnMessage(L"Failed to call CreateToolhelp32Snapshot");
		return;
	}

	if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
	{
		do
		{
			if (wcscmp(L"TrustedInstaller.exe", ProcessEntry.szExeFile) == 0) //Ѱ��winlogon����
			{
				dwTIPID = ProcessEntry.th32ProcessID;
				break;
			}
		} while (Process32NextW(hSnapshot, &ProcessEntry));
	}
	CloseHandle(hSnapshot);

	if (dwTIPID == -1)
	{
		ReturnMessage(L"Failed to get TrustedInstaller.exe PID");
		return;
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwTIPID);
	if (hProc != NULL)
	{
		HANDLE hToken, hDupToken;
		if (OpenProcessToken(hProc, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken))
		{
			if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hDupToken))
			{
				LPVOID lpEnv; //������
				if (CreateEnvironmentBlock(&lpEnv, hToken, 1))
				{
					GetAllTokens(hDupToken);
					
					STARTUPINFOW StartupInfo = { 0 };
					PROCESS_INFORMATION ProcessInfo = { 0 };
					StartupInfo.lpDesktop = L"WinSta0\\Default";
					if (!CreateProcessWithTokenW(
						hDupToken,
						LOGON_WITH_PROFILE,
						NULL,
						szCMDLine,
						CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
						lpEnv,
						NULL,
						&StartupInfo,
						&ProcessInfo))
					{
						if (!CreateProcessAsUserW(hDupToken,
							NULL,
							szCMDLine,
							NULL,
							NULL,
							NULL,
							CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
							lpEnv,
							NULL,
							&StartupInfo,
							&ProcessInfo))
						{
							ReturnMessage(L"Failed to Create the process");
						}
					}
					DestroyEnvironmentBlock(lpEnv);
				}
				else ReturnMessage(L"Failed to Call CreateEnvironmentBlock of TrustedInstaller.exe");
				CloseHandle(hDupToken);
			}
			else ReturnMessage(L"Failed to Call DuplicateTokenEx of TrustedInstaller.exe");
			CloseHandle(hToken);
		}
		else ReturnMessage(L"Failed to Call OpenProcessToken of TrustedInstaller.exe");
		CloseHandle(hProc);
	}
	else ReturnMessage(L"Failed to Call OpenProcess of TrustedInstaller.exe");
}

//��ȡ����Ȩ��(�����ڽ���)
bool GetPrivilege(HANDLE ProcessHandle,LPCWSTR lpName)
{
	bool bRet = false;
	HANDLE hCurrentProcessToken;
	if (OpenProcessToken(ProcessHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hCurrentProcessToken))
	{
		if (GetToken(hCurrentProcessToken, lpName)) bRet = true;
		CloseHandle(hCurrentProcessToken);
	}
	return bRet;
}

//��ȡ����Ȩ��(����������)
bool GetToken(HANDLE TokenHandle, LPCWSTR lpName)
{
	bool bRet = false;
	if (TokenHandle != INVALID_HANDLE_VALUE)
	{
		LUID Luid;
		if (LookupPrivilegeValueW(NULL, lpName, &Luid))
		{
			TOKEN_PRIVILEGES TokenPrivileges;

			TokenPrivileges.PrivilegeCount = 1;
			TokenPrivileges.Privileges[0].Luid = Luid;
			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if (AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL)) bRet = true;
		}
	}
	return bRet;
}

//һ����ȡȫ��Ȩ������(����������)
void GetAllTokens(HANDLE TokenHandle)
{
	GetToken(TokenHandle,SE_CREATE_TOKEN_NAME);
	GetToken(TokenHandle, SE_ASSIGNPRIMARYTOKEN_NAME);
	GetToken(TokenHandle, SE_LOCK_MEMORY_NAME);
	GetToken(TokenHandle, SE_INCREASE_QUOTA_NAME);
	GetToken(TokenHandle, SE_UNSOLICITED_INPUT_NAME);
	GetToken(TokenHandle, SE_MACHINE_ACCOUNT_NAME);
	GetToken(TokenHandle, SE_TCB_NAME);
	GetToken(TokenHandle, SE_SECURITY_NAME);
	GetToken(TokenHandle, SE_TAKE_OWNERSHIP_NAME);
	GetToken(TokenHandle, SE_LOAD_DRIVER_NAME);
	GetToken(TokenHandle, SE_SYSTEM_PROFILE_NAME);
	GetToken(TokenHandle, SE_SYSTEMTIME_NAME);
	GetToken(TokenHandle, SE_PROF_SINGLE_PROCESS_NAME);
	GetToken(TokenHandle, SE_INC_BASE_PRIORITY_NAME);
	GetToken(TokenHandle, SE_CREATE_PAGEFILE_NAME);
	GetToken(TokenHandle, SE_CREATE_PERMANENT_NAME);
	GetToken(TokenHandle, SE_BACKUP_NAME);
	GetToken(TokenHandle, SE_RESTORE_NAME);
	GetToken(TokenHandle, SE_SHUTDOWN_NAME);
	GetToken(TokenHandle, SE_DEBUG_NAME);
	GetToken(TokenHandle, SE_AUDIT_NAME);
	GetToken(TokenHandle, SE_SYSTEM_ENVIRONMENT_NAME);
	GetToken(TokenHandle, SE_CHANGE_NOTIFY_NAME);
	GetToken(TokenHandle, SE_REMOTE_SHUTDOWN_NAME);
	GetToken(TokenHandle, SE_UNDOCK_NAME);
	GetToken(TokenHandle, SE_SYNC_AGENT_NAME);
	GetToken(TokenHandle, SE_ENABLE_DELEGATION_NAME);
	GetToken(TokenHandle, SE_MANAGE_VOLUME_NAME);
	GetToken(TokenHandle, SE_IMPERSONATE_NAME);
	GetToken(TokenHandle, SE_CREATE_GLOBAL_NAME);
	GetToken(TokenHandle, SE_TRUSTED_CREDMAN_ACCESS_NAME);
	GetToken(TokenHandle, SE_RELABEL_NAME);
	GetToken(TokenHandle, SE_INC_WORKING_SET_NAME);
	GetToken(TokenHandle, SE_TIME_ZONE_NAME);
	GetToken(TokenHandle, SE_CREATE_SYMBOLIC_LINK_NAME);
}