#include "Utils.h"

Utils::Utils(void)
{
}


Utils::~Utils(void)
{
}

LPWSTR Utils::MyLoadString(__in UINT uID){
	WCHAR fCaption[LOADSTRING_BUFFERSIZE];
	int n = LoadString(NULL,uID,fCaption,sizeof(fCaption)/sizeof(fCaption[0]));
	return fCaption;
}