#include "Utils.h"

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
#if !defined(Debug)
	wchar_t buf[4096], *p = buf;
	va_list args;
	va_start(args, format);
	p += _vsnwprintf(p, sizeof buf - 1, format, args);
	va_end(args);
	/*while ( p > buf && isspace(p[-1]) )
	{
		*--p = '\0';
		*p++ = '\r';
		*p++ = '\n';
		*p = '\0';
	}*/
	OutputDebugString(buf);
#endif
}