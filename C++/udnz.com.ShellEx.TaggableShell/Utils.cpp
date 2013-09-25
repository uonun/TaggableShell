#include "Utils.h"

#ifdef LOG4CPP
#include "log4cpp/Category.hh"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
using namespace log4cpp;
#endif

Utils::Utils(void)
{

}


Utils::~Utils(void)
{

}

//LPWSTR Utils::MyLoadString(__in UINT uID){
//	WCHAR fCaption[LOADSTRING_BUFFERSIZE];
//	int n = LoadString(NULL,uID,fCaption,sizeof(fCaption)/sizeof(fCaption[0]));
//	return fCaption;
//}
void __cdecl Utils::PrintLog(const wchar_t *format, ...)
{
#ifdef LOG4CPP
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
	string logfile = string(LOG_FOLDER).append(LOG_FILENAME);
	log4cpp::RollingFileAppender* osAppender = new log4cpp::RollingFileAppender("main_log",logfile);
	osAppender->setLayout(new log4cpp::BasicLayout());
	_log.addAppender(osAppender);
	_log.setPriority(log4cpp::Priority::DEBUG);
	//ostringstream oss;
	//oss << "Got sub item: "<<szDst;
	_log.debug(szDst);
	log4cpp::Category::shutdown();    
#endif
}
