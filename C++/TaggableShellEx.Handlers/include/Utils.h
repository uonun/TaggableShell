#pragma once
#include "StdAfx.h"

wchar_t * MyLoadString(UINT uID);

void PrintLog(const char *format, ...);
void PrintLog(const wchar_t *format, ...);

/*
* use "new" to create result, you must delete it yourself!
*/
void Str2WStr(const LPSTR & s1,LPWSTR & result);

/*
* use "new" to create result, you must delete it yourself!
*/
void WStr2Str(const LPWSTR & s1,LPSTR & result);

void Replace(char* src, const char* sub, const char* dst);
//wchar_t* Replace(wchar_t* src, wchar_t* sub, wchar_t* dst);
string& replace_all(string& str,const string& old_value,const string& new_value) ;
string& replace_all_distinct(string& str,const string& old_value,const string& new_value);

wchar_t * ANSIToUnicode( const char* str, wchar_t * & result );
char * UnicodeToANSI( const wchar_t* str, char * result );
wchar_t * UTF8ToUnicode( const char* str, wchar_t * & result );
char * UnicodeToUTF8( const wchar_t* str, char * result );
char* w2m(const wchar_t* wcs, char * & result);
wchar_t* m2w(const char* mbs, wchar_t * & result);
char* ANSIToUTF8(char* str);
char* UTF8ToANSI(char* str);

BOOL Is64BitWindows();
wchar_t * GetOSVersionStr();
BOOL TestMinimumOSRequirement();

#define RIIDNAME(riid,riid2) GetRiidName(riid),GetRiidName(riid2)
LPWSTR GetRiidName(REFIID riid);

HWND CreateSingletonDlg(HWND parent,UINT dlgResourceID,LPWSTR windowCaption,DLGPROC lpDialogFunc,LPARAM lParam);