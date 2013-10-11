#pragma once
#include "StdAfx.h"

wchar_t * MyLoadString(UINT uID);

void PrintLog(const char *format, ...);
void PrintLog(const wchar_t *format, ...);
//void WStr2Str(LPWSTR s1,LPSTR s2);
void Str2WStr(const LPSTR & s1,LPWSTR & s2);
void WStr2Str(const LPWSTR & s1,LPSTR & s2);

char * Replace(const char* src, char* sub, char* dst);
//wchar_t* Replace(wchar_t* src, wchar_t* sub, wchar_t* dst);
string& replace_all(string& str,const string& old_value,const string& new_value) ;
string& replace_all_distinct(string& str,const string& old_value,const string& new_value);

wchar_t * ANSIToUnicode( const char* str );
char * UnicodeToANSI( const wchar_t* str );
wchar_t * UTF8ToUnicode( const char* str );
char * UnicodeToUTF8( const wchar_t* str );
char* w2m(const wchar_t* wcs);
wchar_t* m2w(const char* mbs);
char* ANSIToUTF8(const char* str);
char* UTF8ToANSI(const char* str);
