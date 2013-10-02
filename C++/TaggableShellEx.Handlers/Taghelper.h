#pragma once
#include "dllmain.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;

class CTaghelper
{
public:
	CTaghelper(void);
	virtual ~CTaghelper(void);

	void LoadTags();
	HRESULT SetTag(IShellItem & ppv, int tagIdx);
	
	int TagCount;					// The count of loaded tags. READ ONLY.
	LPWSTR Tags[MAXCOUNT_TAG];		// The loaded tags. READ ONLY.

private:
	fstream* _pfile;
	bool _cached;
};

