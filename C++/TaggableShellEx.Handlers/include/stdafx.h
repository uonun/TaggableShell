// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

// 导入静态库     
#pragma comment(lib, "Comctl32.lib")
// 开启视觉效果 Copy from MSDN     
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>  
#include <Windowsx.h>
#include <new> // std::nothrow
#include <windows.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <commoncontrols.h> // IID_IImageList
#include <string>
#include <sstream>
#include <ctime>
#include <filesystem>
#include "../src/resource.h"

using namespace std;
using namespace std::tr2::sys;


#ifdef LOG4CPP
#pragma comment(lib, "log4cpp.lib")
#define LOG_FILENAME					L"TaggableShell.log"
#endif

#define LOADSTRING_BUFFERSIZE			10240				// NOTE: could not be 102400, will throw exception while print log. not sure why.
#define MAXLENGTH_SQL					100000				// The maximum number of bytes in the text of an SQL statement is limited to SQLITE_MAX_SQL_LENGTH which defaults to 1000000. You can redefine this limit to be as large as the smaller of SQLITE_MAX_LENGTH and 1073741824
#define MAXLENGTH_EACHTAG				100					// max length of each tag.
#define MAXCOUNT_TAG					100
#define MAXCOUNT_ITEM					0xFFFF				// max count of selected shell items.

#define FOLDER_PROFILE					L"Profile"
#define FOLDER_LOG						L"Logs"
#define FILE_USERDB						L"User.db"
#define DB_RECORD_NOT_EXIST				UINT_MAX

#define WINDOWCLASS_DLG					L"#32770"			// the class name of dialog. for FindWindow/FindWindowEx

// CMD-IDs for ContextMenu, must be consecutive from 0x1
#define CMD_NEWTAG						0x1
#define CMD_SETTINGS					0x2
#define CMD_ABOUT						0x3

#define COLOR_MY_DEFAULT					COLOR_WINDOWTEXT
#define COLOR_MY_ERROR						RGB(0xFF,0,0)
#define COLOR_MY_WARNING					RGB(0xFF,0x80,0)
#define COLOR_MY_NOTE						RGB(0,0,0xFF)
#define COLOR_MY_REMARK						RGB(0x3C,0x3C,0x3C)
#define COLOR_MY_OK							RGB(0,0x80,0)

// My messages
#define MSG_TRANSFER_INSTANCES			WM_USER + 0x10

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
