//NSudo 3.0
//NSudo Team. All rights reserved.

#include "stdafx.h"

#define NSudoAPI_Export __declspec(dllexport)

extern "C"
{
	//获取一个进程的PID
	NSudoAPI_Export DWORD NSudoGetProcessID(LPCWSTR lpProcessName, bool bUnderCurrentSessionID)
	{
		DWORD dwPID = -1 /*进程ID*/, dwUserSessionId = 0 /*会话ID*/;
		if ((dwUserSessionId = WTSGetActiveConsoleSessionId()) != 0xFFFFFFFF)
		{
			PWTS_PROCESS_INFOW pWTSProcessInfo = NULL;
			DWORD dwProcessCount = 0;

			if (WTSEnumerateProcessesW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pWTSProcessInfo, &dwProcessCount))
			{
				for (DWORD i = 0; i < dwProcessCount; i++)
				{
					if (_wcsicmp(lpProcessName, pWTSProcessInfo[i].pProcessName) == 0) //寻找winlogon进程
					{
						if (bUnderCurrentSessionID && pWTSProcessInfo[i].SessionId != dwUserSessionId) continue; //判断是否是当前用户ID
						dwPID = pWTSProcessInfo[i].ProcessId;
						break;
					}
				}
				WTSFreeMemory(pWTSProcessInfo);
			}
		}
		return dwPID;
	}

	//根据令牌创建进程（对CreateProcess和CreateEnvironmentBlock的封装，可能需要SE_ASSIGNPRIMARYTOKEN_NAME特权）
	NSudoAPI_Export bool NSudoCreateProcess(HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine)
	{
		bool bRet = false;

		STARTUPINFOW StartupInfo = { 0 };
		PROCESS_INFORMATION ProcessInfo = { 0 };
		StartupInfo.lpDesktop = L"WinSta0\\Default";

		if (CreateProcessWithTokenW(hToken,
			LOGON_WITH_PROFILE,
			lpApplicationName,
			lpCommandLine,
			CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
			NULL,
			NULL,
			&StartupInfo,
			&ProcessInfo))
		{
			bRet = true;
		}

		return bRet;
	}

	wchar_t *PrivilegeList[] = {
		SE_CREATE_TOKEN_NAME,
		SE_ASSIGNPRIMARYTOKEN_NAME,
		SE_LOCK_MEMORY_NAME,
		SE_INCREASE_QUOTA_NAME,
		SE_UNSOLICITED_INPUT_NAME,
		SE_MACHINE_ACCOUNT_NAME,
		SE_TCB_NAME,
		SE_SECURITY_NAME,
		SE_TAKE_OWNERSHIP_NAME,
		SE_LOAD_DRIVER_NAME,
		SE_SYSTEM_PROFILE_NAME,
		SE_SYSTEMTIME_NAME,
		SE_PROF_SINGLE_PROCESS_NAME,
		SE_INC_BASE_PRIORITY_NAME,
		SE_CREATE_PAGEFILE_NAME,
		SE_CREATE_PERMANENT_NAME,
		SE_BACKUP_NAME,
		SE_RESTORE_NAME,
		SE_SHUTDOWN_NAME,
		SE_DEBUG_NAME,
		SE_AUDIT_NAME,
		SE_SYSTEM_ENVIRONMENT_NAME,
		SE_CHANGE_NOTIFY_NAME,
		SE_REMOTE_SHUTDOWN_NAME,
		SE_UNDOCK_NAME,
		SE_SYNC_AGENT_NAME,
		SE_ENABLE_DELEGATION_NAME,
		SE_MANAGE_VOLUME_NAME,
		SE_IMPERSONATE_NAME,
		SE_CREATE_GLOBAL_NAME,
		SE_TRUSTED_CREDMAN_ACCESS_NAME,
		SE_RELABEL_NAME,
		SE_INC_WORKING_SET_NAME,
		SE_TIME_ZONE_NAME,
		SE_CREATE_SYMBOLIC_LINK_NAME };

	//设置令牌权限
	NSudoAPI_Export bool SetTokenPrivilege(HANDLE TokenHandle, LPCWSTR lpName, bool bEnable)
	{
		TOKEN_PRIVILEGES TP;

		TP.PrivilegeCount = 1;
		TP.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : NULL;

		if (LookupPrivilegeValueW(NULL, lpName, &TP.Privileges[0].Luid))
		{
			if (AdjustTokenPrivileges(TokenHandle, FALSE, &TP, sizeof(TP), NULL, NULL)) return true;
		}

		return false;
	}

	//设置当前进程令牌权限
	NSudoAPI_Export bool SetCurrentProcessPrivilege(LPCWSTR lpName, bool bEnable)
	{
		bool bRet = false;
		HANDLE hCurrentProcessToken;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hCurrentProcessToken))
		{
			if (SetTokenPrivilege(hCurrentProcessToken, lpName, bEnable)) bRet = true;
			CloseHandle(hCurrentProcessToken);
		}
		return bRet;
	}

	//一键开启或关闭所有特权
	NSudoAPI_Export void NSudoAdjustAllTokenPrivileges(HANDLE TokenHandle, bool bEnable)
	{
		for (int i = 0; i < sizeof(PrivilegeList) / sizeof(wchar_t*); i++)
		{
			SetTokenPrivilege(TokenHandle, PrivilegeList[i], bEnable);
		}
	}

	//获取System权限令牌(需要SE_DEBUG_NAME特权)
	NSudoAPI_Export bool NSudoGetSystemToken(PHANDLE hNewToken)
	{
		bool bRet = false;

		//获取当前会话ID下的winlogon的PID
		DWORD dwWinLogonPID = NSudoGetProcessID(L"winlogon.exe", true);
		if (dwWinLogonPID != -1)
		{
			HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwWinLogonPID);
			if (hProc != NULL)
			{
				HANDLE hToken;
				if (OpenProcessToken(hProc, TOKEN_DUPLICATE, &hToken))
				{
					if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, hNewToken))
					{
						bRet = true;
					}
					CloseHandle(hToken);
				}
				CloseHandle(hProc);
			}
		}
		return bRet;
	}

	//模拟当前进程的权限令牌为System权限(如果想取消模拟，可以调用RevertToSelf)
	NSudoAPI_Export bool NSudoImpersonateSystemToken()
	{
		bool bRet = false;

		HANDLE hSystemToken;
		if (NSudoGetSystemToken(&hSystemToken))
		{
			NSudoAdjustAllTokenPrivileges(hSystemToken, true);

			if (ImpersonateLoggedOnUser(hSystemToken)) bRet = true;
			CloseHandle(hSystemToken);
		}
		return bRet;
	}

	//获取TrustedInstaller权限令牌(需要SE_DEBUG_NAME特权)
	NSudoAPI_Export bool NSudoGetTrustedInstallerToken(PHANDLE hNewToken)
	{
		bool bRet = false;

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
						// 启动服务
						if (StartServiceW(hSvc, NULL, NULL))
						{
							// 等待服务启动
							while (::QueryServiceStatus(hSvc, &status) == TRUE)
							{
								Sleep(status.dwWaitHint);
								if (status.dwCurrentState == SERVICE_RUNNING) break;
							}
						}
					}
				}
				CloseServiceHandle(hSvc);
			}
			CloseServiceHandle(hSC);
		}

		if (NSudoImpersonateSystemToken())
		{
			//获取当前会话ID下的TrustedInstaller的PID
			DWORD dwTIPID = NSudoGetProcessID(L"TrustedInstaller.exe", false);

			if (dwTIPID != -1)
			{
				HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwTIPID);
				if (hProc != NULL)
				{
					HANDLE hToken;
					if (OpenProcessToken(hProc, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken))
					{
						if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, hNewToken))
						{
							bRet = true;
						}
						CloseHandle(hToken);
					}
					CloseHandle(hProc);
				}
			}

			RevertToSelf();
		}

		return bRet;
	}

	//获取当前用户的令牌(需要SE_DEBUG_NAME特权)
	NSudoAPI_Export bool NSudoGetCurrentUserToken(PHANDLE hNewToken)
	{
		bool bRet = false;

		if (NSudoImpersonateSystemToken())
		{
			DWORD dwUserSessionId = WTSGetActiveConsoleSessionId();
			if (0xFFFFFFFF != dwUserSessionId)
			{
				if (WTSQueryUserToken(dwUserSessionId, hNewToken)) bRet = true;
			}

			RevertToSelf();
		}

		return bRet;
	}

	//设置令牌完整性
	NSudoAPI_Export bool SetTokenIntegrity(HANDLE hToken, LPCWSTR szIntegritySID)
	{
		bool bRet = false;
		PSID pIntegritySid = NULL; // PSID 结构

		if (ConvertStringSidToSidW(szIntegritySID, &pIntegritySid)) //获取低完整性 SID 的 PSID 结构
		{
			TOKEN_MANDATORY_LABEL TokenMandatoryLabel = { 0 };  //令牌完整性结构

			TokenMandatoryLabel.Label.Attributes = SE_GROUP_INTEGRITY;
			TokenMandatoryLabel.Label.Sid = pIntegritySid;

			DWORD TokenInformationLength = sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(pIntegritySid);

			//设置进程完整性级别
			if (SetTokenInformation(hToken, TokenIntegrityLevel, &TokenMandatoryLabel, TokenInformationLength))
			{
				bRet = true; // 函数执行成功
			}
			LocalFree(pIntegritySid); // 释放 pIntegritySid 结构
		}

		return bRet;
	}

	//获取当前进程的令牌
	NSudoAPI_Export bool NSudoGetCurrentProcessToken(PHANDLE hNewToken)
	{
		bool bRet = false;

		HANDLE hToken;
		if (OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED, &hToken)) //打开当前进程令牌
		{
			if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, hNewToken))
			{
				bRet = true;
			}
			CloseHandle(hToken); // 关闭 hToken 句柄
		}

		return bRet;
	}
}