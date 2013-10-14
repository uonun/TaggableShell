// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

// ���뾲̬��     
#pragma comment(lib, "Comctl32.lib")
// �����Ӿ�Ч�� Copy from MSDN     
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>  
#include <string>
#include <sstream>
#include "resource.h"
#include <ctime>
#include <filesystem>

using namespace std;
using namespace std::tr2::sys;


#ifdef LOG4CPP
#pragma comment(lib, "log4cpp.lib")
#define LOG_FILENAME					L"TaggableShell.log"
#endif

#define LOADSTRING_BUFFERSIZE			1024 * 10
#define MAXLENGTH_SQL					512					// sql sentance buffer
#define MAXLENGTH_EACHTAG				10					// max length of each tag.
#define MAXCOUNT_TAG					100

#define FOLDER_PROFILE					L"Profile"
#define FOLDER_LOG						L"Logs"
#define FILE_USERDB						L"User.db"

// Handle the the DLL's module
extern HINSTANCE g_hInst;
extern WCHAR __loadStringBuffer[LOADSTRING_BUFFERSIZE];

extern WCHAR g_DllDirectory[MAX_PATH];
extern WCHAR g_DllFullName[MAX_PATH];
extern WCHAR g_ProfileDirectory[MAX_PATH];
extern WCHAR g_UserDb[MAX_PATH];
#ifdef LOG4CPP
extern WCHAR g_LogFullName[MAX_PATH];
extern WCHAR g_LogDirectory[MAX_PATH];
#endif