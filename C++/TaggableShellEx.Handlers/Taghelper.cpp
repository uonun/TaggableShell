#include "Taghelper.h"


CTaghelper::CTaghelper(void)
{

}


CTaghelper::~CTaghelper(void)
{
}

void CTaghelper::GetTags(LPWSTR* tags,int* count){
	//std::locale utf8_locale(std::locale(), new gel::stdx::utf8cvt<true>);
	//std::wfstream fs;
	//fs.imbue(utf8_locale);
	//fs.open(yourfile, mode);


	//*count=0;
	//wfstream file;
	//file.open(g_TagsFullName,ios::in);
	//if(!file.fail())
	//{
	//int maxloop = 1000;
	//while(file.peek() != EOF && !file.eof() && maxloop-- >0)
	//	{
	//		tags[*count] = new wchar_t[TAG_LENGTH];
	//		memset(tags[*count],0,TAG_LENGTH * sizeof(wchar_t));
	//		file.getline(tags[*count],TAG_LENGTH,'\n');

	//		if(tags[*count][0]!='\0'){
	//			(*count)++;

	//			if((*count)>= MAXCOUNT_TAG)
	//				return;
	//		}
	//	}
	//}
	//file.close();


	*count=0;
	fstream file;
	file.open(g_TagsFullName,ios::in);
	if(!file.fail())
	{
		int maxloop = 1000;
		while(file.peek() != EOF && !file.eof() && maxloop-- >0)
		{
			char tmp[TAG_LENGTH];
			memset(tmp,0,TAG_LENGTH * sizeof(char));
			file.getline(tmp,TAG_LENGTH,'\n');

			if(tmp[0]!='\0'){
				tags[*count] = new wchar_t[TAG_LENGTH];
				memset(tags[*count],0,TAG_LENGTH * sizeof(wchar_t));

				DWORD dwNum = MultiByteToWideChar(CP_UTF8,NULL,tmp,-1,NULL,0);
				if(dwNum > 0){
					MultiByteToWideChar(CP_UTF8,NULL,tmp,-1,tags[*count],dwNum > TAG_LENGTH? TAG_LENGTH : dwNum);
					(*count)++;
				}

				if((*count)>= MAXCOUNT_TAG)
					return;
			}
		}
	}
	file.close();


	/*
	ifstream file; 
	int LINES;
	file.open(g_TagsFullName,ios::in);
	if(file.fail())
	{
	cout<<"文件不存在."<<endl;
	file.close();
	cin.get();
	cin.get();
	}
	else//文件存在
	{
	#pragma region CountLines
	int n=0;
	string temp;
	while(getline(file,temp))
	{
	n++;
	}
	LINES= n;
	#pragma endregion

	int *tc=new int[LINES]; 
	char *t=new char[LINES]; 
	int i=0; 
	while(!file.eof()) //读取数据到数组
	{ 
	file>>tc[i]; 
	file>>t[i]; 
	i++; 
	} 
	file.close(); //关闭文件
	for(i=0;i<LINES;i++)//输出数组内容
	cout<<tc[i]<<"\t"<<t[i]<<endl;
	cin.get();
	cin.get();
	}
	*/
}