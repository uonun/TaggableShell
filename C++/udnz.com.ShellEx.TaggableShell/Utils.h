#pragma once
#include <Windows.h>  
#include <string>
#include "resource.h"
#include "WinMain.h"
//#include <sstream>
using namespace std;

#define LOADSTRING_BUFFERSIZE       162


class Utils
{
public:
	Utils(void);
	~Utils(void);

	static LPWSTR MyLoadString(__in UINT uID);
};

