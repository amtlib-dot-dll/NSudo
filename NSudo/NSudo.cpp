//Mouri_Naruto NSudo 2.2 (Build 861)
//(C) CopyRight 2015 Mouri_Naruto

#include "stdafx.h"
#include "NSudo.h"

#define NSudo_Title L"NSudo 2.2 (Build 861) By Mouri_Naruto"
#define NSudo_HelpText L"NSudo [ѡ��]\n\nѡ��:\n-TI ��SystemȨ��(����TrustedInstaller����)����������ʾ��\n-System ��SystemȨ������������ʾ��\n-Help ���������а���"

#define ReturnMessage(lpText) MessageBoxW(NULL, (lpText), NSudo_Title, NULL)

#define StrLenW(str) sizeof(str) / 2 - 1

void GetSystemPrivilege(LPWSTR szCMDLine);
void GetTIToken(LPWSTR szCMDLine);
void CreateCMDShell(LPCWSTR lpConsoleTitle);

#include "Privilege.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{	
	if (!GetPrivilege(GetCurrentProcess(),SE_DEBUG_NAME))
	{
		ReturnMessage(L"���̵���Ȩ�޻�ȡʧ��");
		return -1;
	}

	//�ڲ������в���
	if (StrCmpNIW(L"-TiShell", lpCmdLine, StrLenW(L"-TiShell")) == 0)
	{
		wchar_t szCMDLine[260];
		GetModuleFileNameW(NULL, szCMDLine, 260);
		wcscat_s(szCMDLine, 260, L" -GetTiShell");
		GetTIToken(szCMDLine);
		ExitProcess(0);
	}
	else if (StrCmpNIW(L"-GetSystemShell", lpCmdLine, StrLenW(L"-GetSystemShell")) == 0)
	{
		CreateCMDShell(L"NSudo - [System]");
	}
	else if (StrCmpNIW(L"-GetTiShell", lpCmdLine, StrLenW(L"-GetTiShell")) == 0)
	{
		CreateCMDShell(L"NSudo - [System With TrustedInstaller Token]");
	}

	//�����������в���
	if (StrCmpNIW(L"-TI", lpCmdLine, StrLenW(L"-TI")) == 0)
	{
		wchar_t szCMDLine[260];
		GetModuleFileNameW(NULL, szCMDLine, 260);
		StringCbCatW(szCMDLine, sizeof(szCMDLine), L" -TiShell");
		GetSystemPrivilege(szCMDLine);
		ExitProcess(0);
	}
	else if (StrCmpNIW(L"-System", lpCmdLine, StrLenW(L"-System")) == 0)
	{
		wchar_t szCMDLine[260];
		GetModuleFileNameW(NULL, szCMDLine, 260);
		StringCbCatW(szCMDLine, sizeof(szCMDLine), L" -GetSystemShell");
		GetSystemPrivilege(szCMDLine);
		ExitProcess(0);
	}
	else if (StrCmpNIW(L"-Help", lpCmdLine, StrLenW(L"-Help")) == 0)
	{
		ReturnMessage(NSudo_HelpText);
		ExitProcess(0);
	}
	else
	{
		int nButtonPressed = 0, nRadioButton = 0;
		TASKDIALOGCONFIG config = { 0 };
		const TASKDIALOG_BUTTON buttons[] = {
			{ 101, L"����ѡȨ������������ʾ��(&C)" },
			{ 102, L"�����а���(&H)" },
		};
		const TASKDIALOG_BUTTON choosebuttons[] = {
			{ 201, L"SystemȨ��(����TrustedInstaller����)(&T)" },
			{ 202, L"��SystemȨ��(&S)" },
		};

		config.dwFlags = TDF_USE_HICON_MAIN | TDF_USE_HICON_FOOTER | TDF_EXPAND_FOOTER_AREA | TDF_ALLOW_DIALOG_CANCELLATION;
		config.cbSize = sizeof(config);
		config.pszWindowTitle = NSudo_Title;
		config.cRadioButtons = ARRAYSIZE(choosebuttons);
		config.pRadioButtons = choosebuttons;
		config.pszMainInstruction = L"��ѡ������Ҫ��Ȩ��";
		config.hInstance = hInstance;
		config.cButtons = ARRAYSIZE(buttons);
		config.pButtons = buttons;
		config.hMainIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_NSUDO));
		LoadIconWithScaleDown(NULL, IDI_WARNING,GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),&config.hFooterIcon);
		config.pszFooter = L"���棺ʹ�ñ�������Ҫ��һ���ļ��������";

		TaskDialogIndirect(&config, &nButtonPressed, &nRadioButton, NULL);

		switch (nButtonPressed)
		{
		case 101:
			if (nRadioButton == 201)
			{
				wchar_t szCMDLine[260];
				GetModuleFileNameW(NULL, szCMDLine, 260);
				StringCbCatW(szCMDLine, sizeof(szCMDLine), L" -TiShell");
				GetSystemPrivilege(szCMDLine);
			}
			else
			{
				wchar_t szCMDLine[260];
				GetModuleFileNameW(NULL, szCMDLine, 260);
				StringCbCatW(szCMDLine, sizeof(szCMDLine), L" -GetSystemShell");
				GetSystemPrivilege(szCMDLine);
			}
			ExitProcess(0);
		case 102:
			ReturnMessage(NSudo_HelpText);
			_tWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
		default:
			break;
		}
	}
	
	return 0;
}

void GetSystemPrivilege(LPWSTR szCMDLine)
{
	DWORD dwUserSessionId; //�û��ỰID
	DWORD dwWinLogonPID = -1; //winlogon.exe��ProcessID

	if ((dwUserSessionId = WTSGetActiveConsoleSessionId()) == 0xFFFFFFFF) //����û��ỰID
	{
		ReturnMessage( L"����û��ỰIDʧ��");
		return;
	}

	PROCESSENTRY32W ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //����CreateToolhelp32Snapshot�������н���
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		ReturnMessage(L"CreateToolhelp32Snapshot����ʧ��");
		return;
	}

	if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
	{
		do
		{
			if (StrCmpNIW(L"winlogon.exe", ProcessEntry.szExeFile, StrLenW(L"winlogon.exe")) == 0) //Ѱ��winlogon����
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
		ReturnMessage(L"winlogon.exe����PID��ȡʧ��");
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
							ReturnMessage(L"���̴���ʧ��");
						}
					}
					DestroyEnvironmentBlock(lpEnv);
				}
				else ReturnMessage(L"winlogon.exe���̻����鴴��ʧ��");
				CloseHandle(hDupToken);
			}
			else ReturnMessage(L"winlogon.exe���̾�����Ƹ���ʧ��");
			CloseHandle(hToken);
		}
		else ReturnMessage(L"winlogon.exe���̾�����ƴ�ʧ��");
		CloseHandle(hProc);
	}
	else ReturnMessage(L"winlogon.exe���̾����ʧ��");
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
						ReturnMessage(L"TrustedInstaller��������ʧ��");
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
		ReturnMessage(L"CreateToolhelp32Snapshot����ʧ��");
		return;
	}

	if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
	{
		do
		{
			if (StrCmpNIW(L"TrustedInstaller.exe", ProcessEntry.szExeFile, StrLenW(L"TrustedInstaller.exe")) == 0) //Ѱ��TrustedInstaller����
			{
				dwTIPID = ProcessEntry.th32ProcessID;
				break;
			}
		} while (Process32NextW(hSnapshot, &ProcessEntry));
	}
	CloseHandle(hSnapshot);

	if (dwTIPID == -1)
	{
		ReturnMessage(L"TrustedInstaller.exe����PID��ȡʧ��");
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
							ReturnMessage(L"���̴���ʧ��");
						}
					}
					DestroyEnvironmentBlock(lpEnv);
				}
				else ReturnMessage(L"TrustedInstaller.exe���̻����鴴��ʧ��");
				CloseHandle(hDupToken);
			}
			else ReturnMessage(L"TrustedInstaller.exe���̾�����Ƹ���ʧ��");
			CloseHandle(hToken);
		}
		else ReturnMessage(L"TrustedInstaller.exe���̾�����ƴ�ʧ��");
		CloseHandle(hProc);
	}
	else ReturnMessage(L"TrustedInstaller.exe���̾����ʧ��");
}



//����������ʾ������
void CreateCMDShell(LPCWSTR lpConsoleTitle)
{
	AllocConsole(); //��������̨

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ����̨��� 

	HICON hIcon = LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_NSUDO));//��ȡͼ����
	SendMessageW(GetConsoleWindow(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);//���ÿ���̨ͼ��

	SetConsoleTitleW(lpConsoleTitle); // ���ô��ڱ��� 

	COORD size = { 80, 300 };
	SetConsoleScreenBufferSize(hOut, size); // ���û�������С 

	wchar_t szCMDPath[260];

	GetSystemWindowsDirectoryW(szCMDPath, 260); //��ȡWindowsĿ¼

	if (GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "ZwWow64ReadVirtualMemory64")) //�ж��Ƿ���64λOS
	{
		StringCbCatW(szCMDPath, sizeof(szCMDPath), L"\\SysNative\\cmd.exe"); //64������ʾ��·��
	}
	else
	{
		StringCbCatW(szCMDPath, sizeof(szCMDPath), L"\\System32\\cmd.exe"); //32������ʾ��·��
	}

	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOW si = { 0 };
	si.hStdOutput = hOut;
	CreateProcessW(NULL, szCMDPath, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi); //����CMD

	WaitForSingleObject(pi.hProcess, INFINITE); //�ȴ����̽���

	FreeConsole();

	ExitProcess(0);//�˳�����
}