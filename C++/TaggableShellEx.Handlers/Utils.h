#pragma once
#include "StdAfx.h"

LPWSTR MyLoadString(__in UINT uID);

void PrintLog(const wchar_t *format, ...);
void Replace(LPWSTR path,char from,char to);
//void WStr2Str(LPWSTR s1,LPSTR s2);
void Str2WStr(LPSTR s1,LPWSTR s2);