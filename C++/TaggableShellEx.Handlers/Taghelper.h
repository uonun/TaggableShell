#pragma once
#include "stdafx.h"
#include <iostream>
#include <fstream>
using namespace std;

class CTaghelper
{
public:
	CTaghelper(void);
	virtual ~CTaghelper(void);

	void CTaghelper::LoadTags();

	
	int TagCount;					// The count of loaded tags. READ ONLY.
	LPWSTR Tags[MAXCOUNT_TAG];		// The loaded tags. READ ONLY.

private:
	fstream* _pfile;
	bool _cached;
};

