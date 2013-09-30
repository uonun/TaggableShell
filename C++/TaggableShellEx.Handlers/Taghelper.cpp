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