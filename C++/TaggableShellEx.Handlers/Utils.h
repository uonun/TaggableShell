#pragma once
#include <Windows.h>  
#include <string>
#include "resource.h"
//#include <sstream>
using namespace std;

#ifdef LOG4CPP
#define LOG_FOLDER					".\\"
#define LOG_FILENAME				"TaggableShell.Handlers.debug.log"
#endif

class Utils
{
public:
	Utils(void);
	~Utils(void);

	//static LPWSTR MyLoadString(__in UINT uID);

#ifdef LOG4CPP
	static void __cdecl PrintLog(const wchar_t *format, ...);
#endif
};

