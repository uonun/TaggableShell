#include "Taghelper.h"

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
	wchar_t* name[MAX_PATH];
	s->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,name);
	ofstream f;
	f.open("F:\\works\\udnz.com.ShellEx.TaggableShell\\C++\\_Debug\\x64\\1.d",ios::out|ios::binary,_SH_DENYRW);
	f << L"000000000000000000000";
	f << name;
	const int size = sizeof(IShellItem);
	char tmp[size];
	memcpy(tmp,s,size);
	f << tmp;
	f.close();




	/*FILE *f;
	int* fHandle;
	int x = _wsopen_s(fHandle,L"F:\\works\\udnz.com.ShellEx.TaggableShell\\C++\\_Debug\\x64\\1.d",_O_APPEND |_O_CREAT|_O_WTEXT|_O_BINARY|_O_WRONLY,_S_IREAD | _S_IWRITE);
	if(x > 0 || *fHandle == -1){

	}else{

	}*/


	return hr;
}