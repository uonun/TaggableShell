#include "Utils.h"

#ifdef LOG4CPP
#include "log4cpp/Category.hh"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
using namespace log4cpp;
#endif


WCHAR __loadStringBuffer[LOADSTRING_BUFFERSIZE];

LPWSTR MyLoadString(__in UINT uID){
	int n = LoadString(::g_hInst,uID,__loadStringBuffer,sizeof(__loadStringBuffer)/sizeof(__loadStringBuffer[0]));
	return __loadStringBuffer;
	//wostringstream _MyLoadStringBuffer;
	//if(n > 0){
	//	_MyLoadStringBuffer << fCaption;
	//}
	//else
	//{
	//	_MyLoadStringBuffer << "RESOURCE["<< uID <<"]" << "\0";
	//}
	//return _MyLoadStringBuffer.str();
}

void Str2WStr(LPSTR s1,LPWSTR s2)
{
	DWORD dwNum = MultiByteToWideChar(CP_UTF8,NULL,s1,-1,NULL,0);
	if(dwNum > 0){
		DWORD len = sizeof(s2)/sizeof(s2[0]);
		MultiByteToWideChar(CP_UTF8,NULL,s1,-1,s2,dwNum > len? len : dwNum);
	}else{
		// MultiByteToWideChar failed.
	}
}

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

	OutputDebugString(buf);
	OutputDebugString(L"\r\n");

#ifdef LOG4CPP
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

	log4cpp::Category &_log = log4cpp::Category::getRoot();

	//////////////////////////
	dwNum = WideCharToMultiByte(CP_OEMCP,NULL,g_LogFullName,-1,NULL,0,NULL,FALSE);
	char *psText2;
	psText2 = new char[dwNum];
	WideCharToMultiByte (CP_OEMCP,NULL,g_LogFullName,-1,psText2,dwNum,NULL,FALSE);
	string logPath;
	logPath = psText2;	
	delete []psText2;
	//////////////////////////

	log4cpp::RollingFileAppender* osAppender = new log4cpp::RollingFileAppender("main_log","1.log");
	osAppender->setLayout(new log4cpp::BasicLayout());
	_log.addAppender(osAppender);
	_log.setPriority(log4cpp::Priority::DEBUG);
	//ostringstream oss;
	//oss << "Got sub item: "<<szDst;
	_log.debug(szDst);
	log4cpp::Category::shutdown();    
#endif
}

// change the chars from "from" to "to" until '\0'
void Replace(LPWSTR path,char from,char to){
	int n=0;
	do
	{
		if(path[n]==from)
			path[n]=to;
	} while (path[++n]!='\0');
}
