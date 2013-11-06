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