#pragma once

struct LanguageResource
{
	wchar_t* NSudo_Text_Default;

	wchar_t* NSudo_Text_TI;
	wchar_t* NSudo_Text_Sys;
	wchar_t* NSudo_Text_CU;
	wchar_t* NSudo_Text_CP;
	wchar_t* NSudo_Text_CPD;

	wchar_t* NSudo_Text_EnableAll;
	wchar_t* NSudo_Text_DisableAll;

	wchar_t* NSudo_Text_Low;
	wchar_t* NSudo_Text_Medium;
	wchar_t* NSudo_Text_High;
	wchar_t* NSudo_Text_System;

	wchar_t* NSudo_AboutText;

	wchar_t* NSudo_Error_Text1;
	wchar_t* NSudo_Error_Text2;
	wchar_t* NSudo_Error_Text3;
	wchar_t* NSudo_Error_Text4;

	wchar_t* NSudoC_HelpText;
};

/*
LanguageResource TextRes = {
	L"Ĭ��",

	L"TrustedInstaller",
	L"System",
	L"��ǰ�û�",
	L"��ǰ����",
	L"��ǰ����(��Ȩ)",

	L"����ȫ����Ȩ",
	L"����������Ȩ",

	L"��",
	L"��",
	L"��",
	L"ϵͳ",

	L"NSudo 3.2(Build 1000)\n"
	L"\xA9 2015 NSudo Team. All rights reserved.\n\n"
	L"����֧�����˺�: wxh32lkk@live.cn\n"
	L"��лboyangpangzi,cjy__05,mhxkx,NotePad,tangmigoId,wondersnefu,xy137425740,�¹��Ĵ���֧�֣�������ĸ����",

	L"��ȡSE_DEBUG_NAME��Ȩʧ��(���Թ���ԱȨ������)",
	L"���������������������л�ѡ��������",
	L"���̴���ʧ��",
	L"�����в����������޸�(ʹ�� - ? �����鿴����)",

	L"��ʽ: NSudoC [ -U ] [ -P ] [ -M ] �����л���������\n"
	L"  -U:[ T | S | C | P | D ] �û�\n"
	L"  	T TrustedInstaller\n"
	L"  	S System\n"
	L"  	C ��ǰ�û�\n"
	L"  	P ��ǰ����\n"
	L"  	D ��ǰ����(��Ȩ)\n\n"
	L"  -P:[ E | D ] ��Ȩ\n"
	L"  	E ����ȫ����Ȩ\n"
	L"  	D ����������Ȩ\n"
	L"  	PS: �������Ĭ����Ȩ��ʽ�Ļ����벻Ҫ����-P����\n\n"
	L"  -M:[ S | H | M | L ] ������\n"
	L"  	S ϵͳ\n"
	L"  	H ��\n"
	L"  	M ��\n"
	L"  	L ��\n"
	L"  	PS: �������Ĭ�������Է�ʽ�Ļ����벻Ҫ����-M����\n\n"
	L"  -? ��ʾ������\n\n"
	L"  ���ӣ���TrustedInstallerȨ�ޣ�����������Ȩ��������Ĭ������������ʾ��\n"
	L"		NSudoC -U:T -P:E cmd\n"
};
*/