#include "Taghelper.h"
#include "strsafe.h"	// StringCchLength

CTaghelper::CTaghelper(void): _cached(false),_pfile(new fstream())
{

}


CTaghelper::~CTaghelper(void)
{
	_pfile->close();
}

void CTaghelper::LoadTags()
{
	if(!_cached && Tags != NULL && sizeof(Tags)/sizeof(Tags[0])>0){
		_pfile->open(g_TagsFullName,ios::in,_SH_DENYNO);
		if(!_pfile->fail())
		{	
			TagCount = 0;
			int maxloop = 1000;
			const int buffLen = TAG_LENGTH * 10;
			while(_pfile->peek() != EOF && maxloop-- > 0)
			{
				char tmp[buffLen];
				memset(tmp,0,sizeof(tmp) * sizeof(char));
				_pfile->getline(tmp,buffLen,'\n');

				if(tmp[0]!='\0'){
					Tags[TagCount] = new wchar_t[TAG_LENGTH];
					memset(Tags[TagCount],0,TAG_LENGTH * sizeof(wchar_t));

					DWORD dwNum = MultiByteToWideChar(CP_UTF8,NULL,tmp,-1,NULL,0);
					if(dwNum > 0){
						MultiByteToWideChar(CP_UTF8,NULL,tmp,-1,Tags[TagCount],TAG_LENGTH);
						Tags[TagCount][TAG_LENGTH] = '\0'; // end the string. in case of the string "tmp" is null-terminated.
						TagCount++;
					}

					if(TagCount>= MAXCOUNT_TAG)
						break;
				}
			}
			_cached = true;
		}
		_pfile->close();
	}
}

HRESULT CTaghelper::SetTag(IShellItem & ppv, int tagIdx)
{
	HRESULT hr = S_FALSE;
	IShellItem* s = NULL; 
	s = &ppv;
	LPWSTR name;
	s->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,(LPWSTR*)&name);

	MessageBox(NULL,name,L"",MB_OK);
	wofstream f;
	f.open("F:\\works\\udnz.com.ShellEx.TaggableShell\\C++\\_Debug\\x64\\1.d",ios::out,_SH_DENYNO);
	f << name;
	f.close();



	HANDLE hFile = CreateFile(L"F:\\works\\udnz.com.ShellEx.TaggableShell\\C++\\_Debug\\x64\\1.d",                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		OPEN_ALWAYS,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template


	size_t dwBytesToWrite=0;
	StringCchLength(name,MAX_PATH,&dwBytesToWrite);
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;
	bErrorFlag = WriteFile( 
		hFile,           // open file handle
		name,      // start of data to write
		dwBytesToWrite,  // number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure

	if (FALSE == bErrorFlag)
	{

	}else{

	}
	CloseHandle(hFile);

	return hr;
}