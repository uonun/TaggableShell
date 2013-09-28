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

	static void CTaghelper::GetTags(LPWSTR* tags,int* count);
};

