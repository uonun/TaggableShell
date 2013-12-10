#pragma once
#include "../include/Utils.h"

#ifdef LOG4CPP
#include "log4cpp/Category.hh"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/PatternLayout.hh"
using namespace log4cpp;
#else
#ifdef SIMPLELOG
#include "../include/Log.h"
#else
//
#endif
#endif

wchar_t * MyLoadString(__in UINT uID){
	static wchar_t buffer[LOADSTRING_BUFFERSIZE];
	memset(buffer,0, sizeof(buffer));
	int n = LoadString(::g_hInst,uID,buffer,LOADSTRING_BUFFERSIZE);
	return buffer;
}

void Str2WStr(const LPSTR & s1,LPWSTR & result)
{
	DWORD dwNum = MultiByteToWideChar(CP_UTF8,NULL,s1,-1,NULL,0);
	result = new wchar_t[dwNum];
	memset(result,0,dwNum * sizeof(wchar_t));

	if(dwNum > 0){
		MultiByteToWideChar(CP_UTF8,NULL,s1,-1,result,dwNum);
	}else{
		// MultiByteToWideChar failed.
	}
}

void WStr2Str(const LPWSTR & s1,LPSTR & result)
{
	DWORD dwNum = WideCharToMultiByte(936,NULL,s1,-1,NULL,0,NULL,FALSE);
	result = new char[dwNum];
	memset(result,0,dwNum * sizeof(char));

	if(dwNum > 0){
		WideCharToMultiByte (936,NULL,s1,-1,result,dwNum,NULL,FALSE);
	}else{
		// WideCharToMultiByte failed.
	}
}

static wchar_t LogBuffer[LOADSTRING_BUFFERSIZE] = {0};
void __print_log_core()
{
	// http://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
	time_t now(time(NULL));
	tm *gmtm = gmtime(&now);
	wchar_t buffer[LOADSTRING_BUFFERSIZE] = {0};
	wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d %H:%M:%S > ",gmtm);

	StrCat(buffer,LogBuffer);
	StrCat(buffer,L"\r\n");
	OutputDebugString(buffer);

#pragma region write log
#ifdef LOG
#ifdef LOG4CPP
	if(CreateDirectory(g_LogDirectory, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		///////////////////////////////////////////////////////////////////
		//std::string的目标
		std::string szDst; 
		//WideCharToMultiByte的运用 
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,buffer,-1,NULL,0,NULL,FALSE);
		//psText为char*的临时数组，作为赋值给std::string的中间变量 
		char *psText;
		psText = new char[dwNum];
		// WideCharToMultiByte的再次运用 
		WideCharToMultiByte (CP_OEMCP,NULL,buffer,-1,psText,dwNum,NULL,FALSE);
		//std::string赋值 
		szDst = psText; //大功告成
		//psText的清除 
		delete []psText;

		char logPath[MAX_PATH];
		::UnicodeToANSI(g_LogFullName,logPath);
		log4cpp::Category &_log = log4cpp::Category::getRoot();
		log4cpp::RollingFileAppender* osAppender = new log4cpp::RollingFileAppender("TaggableShell_Logs",logPath);
		log4cpp::PatternLayout* pLayout = new log4cpp::PatternLayout();
		pLayout->setConversionPattern("%d: %p %c %x: %m%n");
		osAppender->setLayout(pLayout);
		_log.addAppender(osAppender);
		_log.setPriority(log4cpp::Priority::DEBUG);
		//ostringstream oss;
		//oss << "Got sub item: "<<szDst;
		_log.debug(szDst);
		log4cpp::Category::shutdown();    
	}
#else
#ifdef SIMPLELOG
	char logMsg[LOADSTRING_BUFFERSIZE];
	::UnicodeToANSI(buffer,logMsg);

	char logDirectory[MAX_PATH];
	::UnicodeToANSI(g_LogDirectory,logDirectory);

	Log mainLog;  
	mainLog.CommonLogInit(string(logDirectory)); 
	mainLog.LogOutLn(logMsg);
#else
	HANDLE h; 
	if ((h = RegisterEventSource(NULL,L"TaggableShellEx")) == NULL)
	{ 
		return ;
	}

	const wchar_t* ps = buffer;
	WORD wType=EVENTLOG_INFORMATION_TYPE;
	DWORD dwID=5001;
	int iStr = 1;
	BOOL bRet;
	if (h) 
	{
		bRet=::ReportEvent(h,
			wType,
			0,
			dwID,
			NULL, // sid
			iStr,
			0,
			&ps,
			NULL);
	}
#endif
#endif
#endif
#pragma endregion
}
// Print logs, depends on g_LogDirectory, g_LogFullName
void PrintLog(const char *format, ...)
{
	char buf[LOADSTRING_BUFFERSIZE] = {0}, *p = buf;
	va_list args;
	va_start(args, format);
	_vsnprintf(p, sizeof buf - 1,format, args);
	va_end(args);

	wchar_t *f = LogBuffer;
	UTF8ToUnicode(buf,f);

	__print_log_core();
}

// Print logs, depends on g_LogDirectory, g_LogFullName
void PrintLog(const wchar_t *format, ...)
{
	wchar_t *p = LogBuffer;
	va_list args;
	va_start(args, format);
	_vsnwprintf(p, sizeof LogBuffer - 1,format, args);
	va_end(args);

	__print_log_core();
}


//参数，src 字符串源，sub想要替换的字符串，dst，用来替换的字符串
void Replace(char* src, const char* sub, const char* dst)
{
	string s(src);
	auto a = replace_all_distinct(s,sub,dst);
	memcpy(src,a.c_str(),a.length());
}

//  replace_all(string("12212"),"12","21") => 22211   
string& replace_all(string& str,const string& old_value,const string& new_value)   
{   
	while(true)   {   
		string::size_type   pos(0);   
		if(   (pos=str.find(old_value))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}

//  replace_all_distinct(string("12212"),"12","21") => 21221   
string& replace_all_distinct(string& str,const string& old_value,const string& new_value)   
{   
	for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
		if(   (pos=str.find(old_value,pos))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}

wchar_t * ANSIToUnicode(const char* str, wchar_t* & result )
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	int textlen ;
	textlen = MultiByteToWideChar( 936, 0, str,-1, NULL,0 );
	memset(result,0,(textlen+1)*sizeof(wchar_t));
	MultiByteToWideChar(936, 0,str,-1,result,textlen );
	return result;
}

char * UnicodeToANSI( const wchar_t* str, char* result )
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	int textlen;
	textlen = WideCharToMultiByte( 936, 0, str, -1, NULL, 0, NULL, NULL );
	memset( result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( 936, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}

wchar_t * UTF8ToUnicode( const char* str , wchar_t* & result )
{
	_ASSERT_EXPR(NULL != result,L"the parameter \"result\" does not initialized!");

	int textlen ;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );
	memset(result,0,(textlen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );
	return result;
}

char * UnicodeToUTF8( const wchar_t* str , char* result )
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	int textlen;
	textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}

/*宽字符转换为多字符Unicode - ANSI*/
char* w2m(const wchar_t* wcs,char* & result)
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	size_t len;
	len = wcstombs(NULL,wcs,0);
	if (len <= 0)
	{
		result = NULL;
		return NULL;
	}
	memset(result, 0, sizeof(char) *(len+1));
	len = wcstombs(result,wcs,len+1);
	return result;
}

/*多字符转换为宽字符ANSI - Unicode*/
wchar_t* m2w(const char* mbs, wchar_t* & result )
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	size_t len;
	len = mbstowcs(NULL,mbs,0);
	if (len <= 0)
	{
		result = NULL;
		return NULL;
	}
	memset(result, 0, sizeof(wchar_t) *(len+1));
	len = mbstowcs(result,mbs,len+1);
	return result;
}

char* ANSIToUTF8(char* str)
{
	wchar_t* tmp = new wchar_t[LOADSTRING_BUFFERSIZE];
	ANSIToUnicode(str,tmp);
	char* a = UnicodeToUTF8(tmp,str);
	delete(tmp);
	return a;
}

char* UTF8ToANSI(char* str)
{
	wchar_t* tmp = new wchar_t[LOADSTRING_BUFFERSIZE];
	UTF8ToUnicode(str,tmp);
	char* a = UnicodeToANSI(tmp,str);
	delete(tmp);
	return a;
}

//Tested: WinXP, Vista, Win7, Win8(RP) both 32 & 64 bits on VMs
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL Is64BitWindows()
{
#if defined(_WIN64)
	return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows
	// so must sniff
	BOOL f64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(_T("kernel32")),"IsWow64Process");
	if(NULL != fnIsWow64Process)
	{
		return fnIsWow64Process(GetCurrentProcess(),&f64) && f64;
	}
	return FALSE;
#else
	return FALSE; // Win64 does not support Win16
#endif
}

wchar_t * GetOSVersionStr()
{
	SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器 
	GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构 

	OSVERSIONINFOEX os;
	ZeroMemory(&os, sizeof(OSVERSIONINFO));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	static wchar_t result[LOADSTRING_BUFFERSIZE] = {0};
	if(GetVersionEx((OSVERSIONINFO *)&os))                  /*调用GetVersionEx函数OSVERSIONINFOEX结构必须将指针类型强制转换*/
	{ 
		//下面根据版本信息判断操作系统名称 
		switch(os.dwMajorVersion){                        //判断主版本号 
		case 4: 
			switch(os.dwMinorVersion){                //判断次版本号 
			case 0: 
				if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)
					StrCat(result,L"Microsoft Windows NT 4.0");                //1996年7月发布 
				else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) 
					StrCat(result,L"Microsoft Windows 95"); 
				break; 
			case 10: 
				StrCat(result,L"Microsoft Windows 98"); 
				break; 
			case 90: 
				StrCat(result,L"Microsoft Windows Me"); 
				break; 
			} 
			break; 
		case 5: 
			switch(os.dwMinorVersion){               //再比较dwMinorVersion的值 
			case 0: 
				StrCat(result,L"Microsoft Windows 2000");                    //1999年12月发布 
				break; 
			case 1: 
				StrCat(result,L"Microsoft Windows XP");                    //2001年8月发布 
				break; 
			case 2: 
				if(os.wProductType==VER_NT_WORKSTATION && \
					info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
					StrCat(result,L"Microsoft Windows XP Professional"); 
				else if(GetSystemMetrics(SM_SERVERR2)==0) 
					StrCat(result,L"Microsoft Windows Server 2003");        //2003年3月发布 
				else if(GetSystemMetrics(SM_SERVERR2)!=0) 
					StrCat(result,L"Microsoft Windows Server 2003 R2"); 
				else if(os.wSuiteMask & VER_SUITE_WH_SERVER)
					StrCat(result,L"Microsoft Windows Home Server");
				break; 
			} 
			break; 
		case 6: 
			switch(os.dwMinorVersion){ 
			case 0: 
				if(os.wProductType==VER_NT_WORKSTATION)/* VER_NT_WORKSTATION是桌面系统 */
					StrCat(result,L"Microsoft Windows Vista"); 
				else 
					StrCat(result,L"Microsoft Windows Server 2008");          //服务器版本 
				break; 
			case 1: 
				if(os.wProductType==VER_NT_WORKSTATION) 
					StrCat(result,L"Microsoft Windows 7"); 
				else 
					StrCat(result,L"Microsoft Windows Server 2008 R2"); 
				break; 
			case 2:
				if(os.wProductType==VER_NT_WORKSTATION)
					StrCat(result,L"Microsoft Windows 8");
				else
					StrCat(result,L"Windows Server 2012"); 
				break;
			case 3:
				if(os.wProductType==VER_NT_WORKSTATION)
					StrCat(result,L"Microsoft Windows 8.1 Preview");
				else
					StrCat(result,L"Windows Server 2012 R2 Preview"); 
				break;
			} 
			break; 
		default: 
			break;
		}

		if(Is64BitWindows())
		{
			StrCat(result,L", x64 Edition");
		}else{
			StrCat(result,L", x86 Edition");
		}
	} 

	return result; 
}

// Check minimum OS requirement
// This function returns TRUE if OS is Vista or later
// (for example Windows Vista, Windows 7)
BOOL TestMinimumOSRequirement()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op=VER_GREATER_EQUAL;
	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 0; // Windows Vista
	osvi.wServicePackMajor = 0;  
	osvi.wServicePackMinor = 0;
	// Initialize the condition mask.
	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );
	// Perform the test.
	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		dwlConditionMask);
}

HWND CreateSingletonDlg(HWND parent,UINT dlgResourceID,LPWSTR winCaption,DLGPROC lpDialogFunc,LPARAM lParam)
{
	BOOL createNew = true;
	wchar_t windowCaption[LOADSTRING_BUFFERSIZE] = {0};
	memcpy(windowCaption,winCaption,sizeof(windowCaption));
	HWND _hdlg = NULL;
	_hdlg = FindWindowEx(parent, NULL, WINDOWCLASS_DLG, windowCaption);
	if( NULL == _hdlg )
		_hdlg = FindWindowEx(NULL, NULL, WINDOWCLASS_DLG, windowCaption);

	if( NULL == _hdlg )
	{
		_hdlg = CreateDialogParam(g_hInst,MAKEINTRESOURCE(dlgResourceID),parent,lpDialogFunc,lParam);
		::PrintLog(L"New window created: %s, handle = 0x%x",windowCaption, _hdlg);
	}else{
		createNew = FALSE;
		::PrintLog(L"Got existed window: %s, handle = 0x%x",windowCaption, _hdlg);
	}

	if(_hdlg != NULL){
		// the caption of window.
		// must be same as it when the window got msg WM_INITDIALOG for the reason of using FindWindow/FindWindowEx.
		SetWindowText(_hdlg,windowCaption);

		if( dlgResourceID == IDD_ABOUT )
			AnimateWindow(_hdlg,200, AW_VER_POSITIVE);
		else
			ShowWindow(_hdlg,SW_SHOW );

		UpdateWindow(_hdlg);

		if( !createNew )
		{
			SetWindowPos(_hdlg,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
	}
	return _hdlg;
}


#if _DEBUG
#include <initguid.h>
#include <Filter.h>	// for IID_IFilter
#include <ShlGuid.h>
#include <ShObjIdl.h>
#include <ExDisp.h>
DEFINE_GUID(IID_IBrowserSettings,					0xDD1E21CC, 0xE2C7, 0x402C, 0xBF,0x05, 0x10,0x32,0x8D,0x3F,0x6B,0xAD);
DEFINE_GUID(IID_IShellBrowserService,				0xDFBC7E30, 0xF9E5, 0x455F, 0x88,0xF8, 0xFA,0x98,0xC1,0xE4,0x94,0xCA);
DEFINE_GUID(IID_IBrowserWithActivationNotification,	0x6DB89131, 0x7B4C, 0x4E1C, 0x8B,0x01, 0x5D,0x31,0x2C,0x9C,0x73,0x88);
DEFINE_GUID(IID_ILayoutModifier,					0x90B4135A, 0x95BA, 0x46EA, 0x8C,0xAA, 0xE0,0x5B,0x45,0xCD,0x80,0x1E);
DEFINE_GUID(CLSID_Desktop,							0x00021400, 0x0000, 0x0000, 0xC0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46);
DEFINE_GUID(IID_IFileDialogPrivate,					0xAC92FFC5, 0xF0E9, 0x455A, 0x90,0x6B, 0x4A,0x83,0xE7,0x4A,0x80,0x3B);
DEFINE_GUID(IID_IFolderTypeModifier,				0x04BA120E, 0xAD52, 0x4A2D, 0x98,0x07, 0x2D,0xA1,0x78,0xD0,0xC3,0xE1);

struct RIIDNAME
{
	REFIID riid;
	LPWSTR riidName;
	LPWSTR riidCode;
};

// known riid names
const RIIDNAME c_riidNames[] =
{
	{ CLSID_Desktop,							L"CLSID_Desktop",							L"", },
	{ IID_IUnknown,								L"IID_IUnknown",							L"", },
	{ IID_ILayoutModifier,						L"IID_ILayoutModifier",						L"", },
	{ IID_IBrowserSettings,						L"IID_IBrowserSettings",					L"", },
	{ IID_ICommDlgBrowser,						L"IID_ICommDlgBrowser",						L"", },
	{ IID_ICommDlgBrowser2,						L"IID_ICommDlgBrowser2",					L"", },
	{ IID_ICommDlgBrowser3,						L"IID_ICommDlgBrowser3",					L"", },
	{ IID_IFolderTypeModifier,					L"IID_IFolderTypeModifier",					L"", },
	{ IID_IBrowserWithActivationNotification,	L"IID_IBrowserWithActivationNotification",	L"", },
	{ IID_IWebBrowserApp,						L"IID_IWebBrowserApp",						L"", },
	{ IID_IConnectionPointContainer,			L"IID_IConnectionPointContainer",			L"", },
	{ IID_IFolderView,							L"IID_IFolderView",							L"", },
	{ IID_IFileDialog,							L"IID_IFileDialog",							L"", },
	{ IID_IFileDialogPrivate,					L"IID_IFileDialogPrivate",					L"", },
	{ SID_STopLevelBrowser,						L"SID_STopLevelBrowser",					L"", },
	{ IID_IShellBrowserService,					L"IID_IShellBrowserService",				L"", },
	{ IID_IShellBrowser,						L"IID_IShellBrowser",						L"", },
	{ IID_IConnectionPointContainer,			L"IID_IConnectionPointContainer",			L"", },
	{ IID_IProfferService,						L"IID_IProfferService",						L"", },
	{ IID_IShellTaskScheduler,					L"IID_IShellTaskScheduler",					L"", },
	{ SID_DefView,								L"SID_DefView",								L"", },
	{ IID_IShellFolderViewCB,					L"IID_IShellFolderViewCB",					L"", },
	//{ IID_IShellSearchTarget,					L"IID_IShellSearchTarget",					L"", },
	//{ IID_IShellItemBrowser,					L"IID_IShellItemBrowser",					L"", },
	//{ IID_IDelayedVisibility,					L"IID_IDelayedVisibility",					L"", },
	{ IID_IShellFolder,							L"IID_IShellFolder",						L"{000214E6-0000-0000-C000-000000000046}", },
	{ IID_IShellLinkA,							L"IID_IShellLinkA",							L"{000214EE-0000-0000-C000-000000000046}", },
	{ IID_IShellLinkW,							L"IID_IShellLinkW",							L"{000214F9-0000-0000-C000-000000000046}", },
	{ IID_IPersistFile,							L"IID_IPersistFile",						L"{0000010B-0000-0000-C000-000000000046}", },
	{ IID_IPersistStream,						L"IID_IPersistStream",						L"{00000109-0000-0000-C000-000000000046}", },
	{ IID_IShellExtInit,						L"IID_IShellExtInit",						L"{000214E8-0000-0000-C000-000000000046}", },
	{ IID_IContextMenu,							L"IID_IContextMenu",						L"{000214E4-0000-0000-C000-000000000046}", },
	{ IID_IContextMenu2,						L"IID_IContextMenu2",						L"{000214F4-0000-0000-C000-000000000046}", },
	{ IID_IContextMenu3,						L"IID_IContextMenu3",						L"{BCFCE0A0-EC17-11D0-8D10-00A0C90F2719}", },
	{ IID_IDropTarget,							L"IID_IDropTarget",							L"{00000122-0000-0000-C000-000000000046}", },
	{ IID_IExtractIconA,						L"IID_IExtractIconA",						L"{000214EB-0000-0000-C000-000000000046}", },
	{ IID_IExtractIconW,						L"IID_IExtractIconW",						L"{000214FA-0000-0000-C000-000000000046}", },
	{ IID_IShellLinkDataList,					L"IID_IShellLinkDataList",					L"{45E2B4AE-B1C3-11D0-B92F-00A0C90312E1}", },
	{ IID_IQueryInfo,							L"IID_IQueryInfo",							L"{00021500-0000-0000-C000-000000000046}", },
	{ IID_IPersistPropertyBag,					L"IID_IPersistPropertyBag",					L"{37D84F60-42CB-11CE-8135-00AA004BB851}", },
	{ IID_IObjectWithSite,						L"IID_IObjectWithSite",						L"{FC4801A3-2BA9-11CF-A229-00AA003D7352}", },
	{ IID_IServiceProvider,						L"IID_IServiceProvider",					L"{6D5140C1-7436-11CE-8034-00AA006009FA}", },
	{ IID_IFilter,								L"IID_IFilter",								L"{89BCB740-6119-101A-BCB7-00DD010655AF}", },
	{ IID_IShellTaskScheduler ,					L"IID_IShellTaskScheduler ",				L"{6CCB7BE0-6807-11D0-B810-00C04FD706EC}", },
	{ IID_IExplorerPaneVisibility,				L"IID_IExplorerPaneVisibility",				L"{E07010EC-BC17-44C0-97B0-46C7C95B9EDC}", },
	{ IID_ICommDlgBrowser,						L"IID_ICommDlgBrowser",						L"{000214F1-0000-0000-C000-000000000046}", },
	{ IID_ICommDlgBrowser2,						L"IID_ICommDlgBrowser2",					L"{10339516-2894-11D2-9039-00C04F8EEB3E}", },
	{ IID_IShellTaskScheduler,					L"IID_IShellTaskScheduler",					L"{6CCB7BE0-6807-11D0-B810-00C04FD706EC}", },
	{ IID_IQueryAssociations,					L"IID_IQueryAssociations",					L"{C46CA590-3C3F-11D2-BEE6-0000F805CA57}", },
	{ IID_IDataObject,							L"IID_IDataObject",							L"{0000010E-0000-0000-C000-000000000046}", },
};
#endif

LPWSTR GetRiidName(REFIID riid)
{
#if _DEBUG
	for (size_t i = 0; i < ARRAYSIZE(c_riidNames); i++)
	{
		if ( IsEqualIID( riid , c_riidNames[i].riid ))
		{
			return c_riidNames[i].riidName;
		}
	}
#endif
	static wchar_t buffer[50];
	LPOLESTR str;
	StringFromIID(riid,&str);
	StrCpy(buffer,str);
	CoTaskMemFree(str);
	return buffer;
}

