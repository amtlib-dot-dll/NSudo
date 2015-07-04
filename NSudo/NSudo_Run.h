void NSudo_Run(HWND hDlg , LPCWSTR szUser, LPCWSTR szPrivilege, LPCWSTR szMandatory, LPCWSTR szCMDLine)
{
	if (_wcsicmp(L"", szCMDLine) == 0)
	{
		MessageBoxW(hDlg, L"请在文本框输入你要执行的命令或者选择下拉框里面的常用命令", L"NSudo 3.0 Release", MB_ICONEXCLAMATION);
	}
	else
	{
		wchar_t szBuffer[260];

		GetSystemDirectoryW(szBuffer, 260); //获取System32目录
		
		if (_wcsicmp(L"命令提示符", szCMDLine) == 0)
		{
			wcscat_s(szBuffer, 260, L"\\cmd.exe");
		}
		else if (_wcsicmp(L"PowerShell", szCMDLine) == 0)
		{
			wcscat_s(szBuffer, 260, L"\\WindowsPowerShell\\v1.0\\powershell.exe");
		}
		else if (_wcsicmp(L"PowerShell ISE", szCMDLine) == 0)
		{
			wcscat_s(szBuffer, 260, L"\\WindowsPowerShell\\v1.0\\powershell_ise.exe");
		}
		else
		{
			wcscpy_s(szBuffer, 260, szCMDLine);
		}

		HANDLE hUserToken = INVALID_HANDLE_VALUE;
		if (_wcsicmp(NSudo_Text_CU, szUser) == 0)
		{
			NSudoGetCurrentUserToken(&hUserToken);
		}
		else if (_wcsicmp(NSudo_Text_CUP, szUser) == 0)
		{
			NSudoGetCurrentProcessToken(&hUserToken);
		}
		else
		{
			if (_wcsicmp(NSudo_Text_Sys, szUser) == 0)
			{
				NSudoGetSystemToken(&hUserToken);
			}
			else if (_wcsicmp(NSudo_Text_TI, szUser) == 0)
			{
				NSudoGetTrustedInstallerToken(&hUserToken);
			}
		}

		if (_wcsicmp(NSudo_Text_EnableAll, szPrivilege) == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, true);
		}
		else if (_wcsicmp(NSudo_Text_DisableAll, szPrivilege) == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, false);
		}

		if (_wcsicmp(NSudo_Text_Low, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-4096");
		}
		else if (_wcsicmp(NSudo_Text_Medium, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-8192");
		}
		else if (_wcsicmp(NSudo_Text_High, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-12288");
		}

		if (NSudoImpersonateSystemToken())
		{
			if (!NSudoCreateProcess(hUserToken, NULL, szBuffer))
			{
				ReturnMessage(L"进程创建失败");
			}
			RevertToSelf();
		}

		CloseHandle(hUserToken);
	}
}