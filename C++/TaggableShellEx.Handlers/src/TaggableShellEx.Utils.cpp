#include "../include/TaggableShellEx.Utils.h"

#ifdef LOG4CPP
#include "log4cpp/Category.hh"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/PatternLayout.hh"
using namespace log4cpp;
#endif


WCHAR __loadStringBuffer[LOADSTRING_BUFFERSIZE];

LPWSTR MyLoadString(__in UINT uID){
	int n = LoadString(::g_hInst,uID,__loadStringBuffer,sizeof(__loadStringBuffer)/sizeof(__loadStringBuffer[0]));
	return __loadStringBuffer;
}

void Str2WStr(const LPSTR & s1,LPWSTR & s2)
{
	DWORD dwNum = MultiByteToWideChar(CP_UTF8,NULL,s1,-1,NULL,0);
	if(dwNum > 0){
		s2 = new wchar_t[dwNum];
		memset(s2,0,dwNum);
		MultiByteToWideChar(CP_UTF8,NULL,s1,-1,s2,dwNum);
	}else{
		// MultiByteToWideChar failed.
		s2 = L"";
	}
}

void WStr2Str(const LPWSTR & s1,LPSTR & s2)
{
	DWORD dwNum = WideCharToMultiByte(936,NULL,s1,-1,NULL,0,NULL,FALSE);
	if(dwNum > 0){
		s2 = new char[dwNum];
		memset(s2,0,dwNum);
		WideCharToMultiByte (936,NULL,s1,-1,s2,dwNum,NULL,FALSE);
	}else{
		// WideCharToMultiByte failed.
		s2 = "";
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

	auto f = UTF8ToUnicode(buf);
	PrintLog(f,L"");
}

// Print logs, depends on g_LogDirectory, g_LogFullName
void PrintLog(const wchar_t *format, ...)
{
	wchar_t buf[4096], *p = buf;
	va_list args;
	va_start(args, format);
	p += _vsnwprintf(p, sizeof buf - 1, format, args);
	va_end(args);

	//#pragma region NewLine
	//	while ( p > buf && *p != '\0' )
	//	{
	//		p++;
	//	}
	//	*p = '\0';
	//	*p++ = '\r';
	//	*p++ = '\n';
	//	*p = '\0';
	//#pragma endregion

	// http://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
	time_t now(time(NULL));
	tm *gmtm = gmtime(&now);
	wchar_t buffer[256];
	wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d %H:%M:%S\0",gmtm);	
	std::wstringstream ss;
	ss << buffer << " > " << buf << L"\r\n";
	OutputDebugString(ss.str().c_str());

#ifdef LOG4CPP
	if(CreateDirectory(g_LogDirectory, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		///////////////////////////////////////////////////////////////////
		//std::string的目标
		std::string szDst; 
		//WideCharToMultiByte的运用 
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,buf,-1,NULL,0,NULL,FALSE);
		//psText为char*的临时数组，作为赋值给std::string的中间变量 
		char *psText;
		psText = new char[dwNum];
		// WideCharToMultiByte的再次运用 
		WideCharToMultiByte (CP_OEMCP,NULL,buf,-1,psText,dwNum,NULL,FALSE);
		//std::string赋值 
		szDst = psText; //大功告成
		//psText的清除 
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

//参数，src 字符串源，sub想要替换的字符串，dst，用来替换的字符串
char * Replace(const char* src, char* sub, char* dst)
{
	string s(src);
	auto a = replace_all_distinct(s,"'","''");
	int textlen =a.length();
	char * tmp=(char *)malloc(textlen * sizeof(char));;	
	memset(tmp,0,(textlen + 1)*sizeof(char));
	memcpy(tmp,a.c_str(),textlen);
	return tmp;
}

string& replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value)   
{   
	for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
		if(   (pos=str.find(old_value,pos))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}

wchar_t * ANSIToUnicode( const char* str )
{
	int textlen ;
	wchar_t * result;
	textlen = MultiByteToWideChar( 936, 0, str,-1, NULL,0 );
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
	memset(result,0,(textlen+1)*sizeof(wchar_t));
	MultiByteToWideChar(936, 0,str,-1,(LPWSTR)result,textlen );
	return result;
}

char * UnicodeToANSI( const wchar_t* str )
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte( 936, 0, str, -1, NULL, 0, NULL, NULL );
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset( result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( 936, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}

wchar_t * UTF8ToUnicode( const char* str )
{
	int textlen ;
	wchar_t * result;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
	memset(result,0,(textlen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );
	return result;
}

char * UnicodeToUTF8( const wchar_t* str )
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}

/*宽字符转换为多字符Unicode - ANSI*/
char* w2m(const wchar_t* wcs)
{
	int len;
	char* buf;
	len = wcstombs(NULL,wcs,0);
	if (len == 0)
		return NULL;
	buf = (char *)malloc(sizeof(char)*(len+1));
	memset(buf, 0, sizeof(char) *(len+1));
	len =wcstombs(buf,wcs,len+1);
	return buf;
}

/*多字符转换为宽字符ANSI - Unicode*/
wchar_t* m2w(const char* mbs)
{
	int len;
	wchar_t* buf;
	len =mbstowcs(NULL,mbs,0);
	if (len == 0)
		return NULL;
	buf = (wchar_t *)malloc(sizeof(wchar_t)*(len+1));
	memset(buf, 0, sizeof(wchar_t) *(len+1));
	len =mbstowcs(buf,mbs,len+1);
	return buf;
}

char* ANSIToUTF8(const char* str)
{
	return UnicodeToUTF8(ANSIToUnicode(str));
}

char* UTF8ToANSI(const char* str)
{
	return UnicodeToANSI(UTF8ToUnicode(str));
}