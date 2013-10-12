#include "../include/TaggableShellEx.Utils.h"

#ifdef LOG4CPP
#include "log4cpp/Category.hh"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/PatternLayout.hh"
using namespace log4cpp;
#endif

wchar_t * MyLoadString(__in UINT uID){
	const UINT len = LOADSTRING_BUFFERSIZE * sizeof(wchar_t);
	static wchar_t buffer[len];
	memset(buffer,0,len);
	int n = LoadString(::g_hInst,uID,buffer,len);
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

// Print logs, depends on g_LogDirectory, g_LogFullName
void PrintLog(const char *format, ...)
{
	char buf[4096], *p = buf;
	va_list args;
	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);

	wchar_t* f = new wchar_t[LOADSTRING_BUFFERSIZE]; 
	UTF8ToUnicode(buf,f);
	PrintLog(f,L"");
	delete(f);
}

// Print logs, depends on g_LogDirectory, g_LogFullName
void PrintLog(const wchar_t *format, ...)
{
	wchar_t buf[4096], *p = buf;
	va_list args;
	va_start(args, format);
	p += _vsnwprintf(p, sizeof buf - 1, format, args);
	va_end(args);

	// http://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
	time_t now(time(NULL));
	tm *gmtm = gmtime(&now);
	wchar_t buffer[LOADSTRING_BUFFERSIZE];
	wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d %H:%M:%S\0",gmtm);	
	std::wstringstream ss;
	ss << buffer << " > " << buf << L"\r\n\0";
	OutputDebugString(ss.str().c_str());

#ifdef LOG4CPP
	if(CreateDirectory(g_LogDirectory, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		///////////////////////////////////////////////////////////////////
		//std::string��Ŀ��
		std::string szDst; 
		//WideCharToMultiByte������ 
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,buf,-1,NULL,0,NULL,FALSE);
		//psTextΪchar*����ʱ���飬��Ϊ��ֵ��std::string���м���� 
		char *psText;
		psText = new char[dwNum];
		// WideCharToMultiByte���ٴ����� 
		WideCharToMultiByte (CP_OEMCP,NULL,buf,-1,psText,dwNum,NULL,FALSE);
		//std::string��ֵ 
		szDst = psText; //�󹦸��
		//psText����� 
		delete []psText;

		auto logPath = ::UnicodeToANSI(g_LogFullName);
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
#endif
}

//������src �ַ���Դ��sub��Ҫ�滻���ַ�����dst�������滻���ַ���
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

char * UnicodeToANSI( const wchar_t* str, char* & result )
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
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	int textlen ;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );
	memset(result,0,(textlen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );
	return result;
}

char * UnicodeToUTF8( const wchar_t* str , char* & result )
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	int textlen;
	textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}

/*���ַ�ת��Ϊ���ַ�Unicode - ANSI*/
char* w2m(const wchar_t* wcs,char* & result)
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	int len;
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

/*���ַ�ת��Ϊ���ַ�ANSI - Unicode*/
wchar_t* m2w(const char* mbs, wchar_t* & result )
{
	_ASSERT_EXPR(NULL!=result,L"the parameter \"result\" does not initialized!");

	int len;
	len =mbstowcs(NULL,mbs,0);
	if (len <= 0)
	{
		result = NULL;
		return NULL;
	}
	memset(result, 0, sizeof(wchar_t) *(len+1));
	len =mbstowcs(result,mbs,len+1);
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
	SYSTEM_INFO info;                                   //��SYSTEM_INFO�ṹ�ж�64λAMD������ 
	GetSystemInfo(&info);                               //����GetSystemInfo�������ṹ 

	OSVERSIONINFOEX os;
	ZeroMemory(&os, sizeof(OSVERSIONINFO));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	static wchar_t result[LOADSTRING_BUFFERSIZE];
	memset(result,0,sizeof(result[0])*LOADSTRING_BUFFERSIZE);
	if(GetVersionEx((OSVERSIONINFO *)&os))                  /*����GetVersionEx����OSVERSIONINFOEX�ṹ���뽫ָ������ǿ��ת��*/
	{ 
		//������ݰ汾��Ϣ�жϲ���ϵͳ���� 
		switch(os.dwMajorVersion){                        //�ж����汾�� 
		case 4: 
			switch(os.dwMinorVersion){                //�жϴΰ汾�� 
			case 0: 
				if(os.dwPlatformId==VER_PLATFORM_WIN32_NT) 
					wcscat(result,L"Microsoft Windows NT 4.0");                //1996��7�·��� 
				else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) 
					wcscat(result,L"Microsoft Windows 95"); 
				break; 
			case 10: 
				wcscat(result,L"Microsoft Windows 98"); 
				break; 
			case 90: 
				wcscat(result,L"Microsoft Windows Me"); 
				break; 
			} 
			break; 
		case 5: 
			switch(os.dwMinorVersion){               //�ٱȽ�dwMinorVersion��ֵ 
			case 0: 
				wcscat(result,L"Microsoft Windows 2000");                    //1999��12�·��� 
				break; 
			case 1: 
				wcscat(result,L"Microsoft Windows XP");                    //2001��8�·��� 
				break; 
			case 2: 
				if(os.wProductType==VER_NT_WORKSTATION && \
					info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
					wcscat(result,L"Microsoft Windows XP Professional"); 
				else if(GetSystemMetrics(SM_SERVERR2)==0) 
					wcscat(result,L"Microsoft Windows Server 2003");        //2003��3�·��� 
				else if(GetSystemMetrics(SM_SERVERR2)!=0) 
					wcscat(result,L"Microsoft Windows Server 2003 R2"); 
				else if(os.wSuiteMask & VER_SUITE_WH_SERVER)
					wcscat(result,L"Microsoft Windows Home Server");
				break; 
			} 
			break; 
		case 6: 
			switch(os.dwMinorVersion){ 
			case 0: 
				if(os.wProductType==VER_NT_WORKSTATION)/* VER_NT_WORKSTATION������ϵͳ */
					wcscat(result,L"Microsoft Windows Vista"); 
				else 
					wcscat(result,L"Microsoft Windows Server 2008");          //�������汾 
				break; 
			case 1: 
				if(os.wProductType==VER_NT_WORKSTATION) 
					wcscat(result,L"Microsoft Windows 7"); 
				else 
					wcscat(result,L"Microsoft Windows Server 2008 R2"); 
				break; 
			case 2:
				if(os.wProductType==VER_NT_WORKSTATION)
					wcscat(result,L"Microsoft Windows 8");
				else
					wcscat(result,L"Windows Server 2012"); 
				break;
			case 3:
				if(os.wProductType==VER_NT_WORKSTATION)
					wcscat(result,L"Microsoft Windows 8.1 Preview");
				else
					wcscat(result,L"Windows Server 2012 R2 Preview"); 
				break;
			} 
			break; 
		default: 
			break;
		}

		if(Is64BitWindows())
		{
			wcscat(result,L", x64 Edition");
		}else{
			wcscat(result,L", x86 Edition");
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