#include "Taghelper.h"

CTaghelper::CTaghelper(void)
{

}


CTaghelper::~CTaghelper(void)
{
}

void CTaghelper::GetTags(LPWSTR* tags,int* count)
{
	*count = 0;
	fstream file;
	try	{
		file.open(g_TagsFullName,ios::in,_SH_DENYNO);
		if(!file.fail())
		{
			int maxloop = 1000;
			const int buffLen = 1024;
			while(file.peek() != EOF && maxloop-- > 0)
			{
				char tmp[buffLen];
				memset(tmp,0,sizeof(tmp) * sizeof(char));
				file.getline(tmp,buffLen,'\n');

				if(tmp[0]!='\0'){
					tags[*count] = new wchar_t[TAG_LENGTH];
					memset(tags[*count],0,TAG_LENGTH * sizeof(wchar_t));

					DWORD dwNum = MultiByteToWideChar(CP_UTF8,NULL,tmp,-1,NULL,0);
					if(dwNum > 0){
						MultiByteToWideChar(CP_UTF8,NULL,tmp,-1,tags[*count],TAG_LENGTH);
						tags[*count][TAG_LENGTH] = '\0'; // end the string. in case of the string "tmp" is null-terminated.
						(*count)++;
					}

					if((*count)>= MAXCOUNT_TAG)
						break;
				}
			}
		}
		file.close();
	}
	catch(int& value){
		file.close();
	}
}