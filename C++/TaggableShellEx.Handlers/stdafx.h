// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ


#include <Windows.h>  
#include <string>
#include <sstream>
#include "resource.h"
#include "Utils.h"
using namespace std;


#ifdef LOG4CPP
#pragma comment(lib, "log4cpp.lib")
#define LOG_FILENAME					L"TaggableShell.Handlers.log"
#endif

#define LOADSTRING_BUFFERSIZE			512
#define TAG_LENGTH						10					// max length of each tag.
#define MAXCOUNT_TAG					100
#define FOLDER_PROFILE					L"Profile"
#define FOLDER_LOG						L"Logs"
#define FILE_TAGS						L"Tags.tagsex"

// Handle the the DLL's module
extern HINSTANCE g_hInst;
extern WCHAR __loadStringBuffer[LOADSTRING_BUFFERSIZE];

extern WCHAR g_DllDirectory[MAX_PATH];
extern WCHAR g_DllFullName[MAX_PATH];
extern WCHAR g_ProfileDirectory[MAX_PATH];
extern WCHAR g_TagsFullName[MAX_PATH];
#ifdef LOG4CPP
extern WCHAR g_LogFullName[MAX_PATH];
extern WCHAR g_LogDirectory[MAX_PATH];
#endif
