#pragma once
#include <Windows.h>  
#include <string>
#include "resource.h"
#include "WinMain.h"
//#include <sstream>
using namespace std;

class Utils
{
public:
	Utils(void);
	~Utils(void);

	//static LPWSTR MyLoadString(__in UINT uID);
	static void __cdecl PrintLog(const wchar_t *format, ...);
};

