// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <strsafe.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <Tlhelp32.h>
#include <Shellapi.h>
#include <winsvc.h>

#include<Userenv.h>
#pragma comment(lib,"Userenv.lib")

#include <CommCtrl.h>
#pragma comment(lib,"comctl32.lib")

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

/*
int hr = MessageBoxW(NULL,
L"�Ƿ�����ͨ��NSudo�����������\n"
L"\n"
L"������: cmd.exe /k \n"
L"\n"
L"��ʾ:���������������������õĻ�������ȡ��\n",
L"NSudo 2.2 (Build 861) By Mouri_Naruto",
MB_YESNO | MB_ICONINFORMATION);
*/