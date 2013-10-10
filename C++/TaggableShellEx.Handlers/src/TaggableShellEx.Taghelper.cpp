#include "../include/TaggableShellEx.Taghelper.h"
//#include "strsafe.h"	// StringCchLength

char * FID_NOT_EXIST = "-1";
char * TID_NOT_EXIST = "-1";

CTaghelper::CTaghelper(void): 
	_cached(false)
	,_dbFile(NULL)
	,_targetFileName(L""), _targetFileNameInSQL("")
	,_targetShellItem(NULL)
	,TagCount(0)
	,db(0)
{
	::PrintLog(L"CTaghelper.ctor");
	::WStr2Str(g_UserDb,_dbFile);
}


CTaghelper::~CTaghelper(void)
{
	::PrintLog(L"CTaghelper.~ctor");
	sqlite3_close(db);  
	db = 0; 
}

int _callback_exec_load_tags(void * tagHelper,int argc, char ** argv, char ** aszColName)  
{  
	wchar_t * tag = NULL;
	if(argv[0] != 0){
		::Str2WStr(argv[0],tag);
		CTaghelper* h = (CTaghelper*)tagHelper;
		h->Tags[h->TagCount].Tag = tag;
		h->Tags[h->TagCount].bAsso = *argv[1]!='0';
		h->TagCount++;
		::PrintLog( L"Got tag: %s", tag );  

		if(h->TagCount>= MAXCOUNT_TAG){
			return 1; // returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT without invoking the callback again and without running any subsequent SQL statements.
		}
	}
	return 0;  
}

void CTaghelper::LoadTags(IShellItem & ppv)
{
	if(!_cached || TagCount == 0){

		TagCount = 0;		

		// cache the target file, and get file fullname.
		_targetShellItem = &ppv;
		HRESULT hr = _targetShellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,(LPWSTR*)&_targetFileName);
		/*
		SIGDN_NORMALDISPLAY:				新建文本文档.txt
		SIGDN_PARENTRELATIVEPARSING:		新建文本文档.txt
		SIGDN_DESKTOPABSOLUTEPARSING:		E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\新建文本文档.txt
		SIGDN_PARENTRELATIVEEDITING:		新建文本文档.txt
		SIGDN_DESKTOPABSOLUTEEDITING:		E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\新建文本文档.txt
		SIGDN_FILESYSPATH:					E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\新建文本文档.txt
		SIGDN_URL:							file:///E:/Works/UDNZ/udnz.com.ShellEx.TaggableShell/C++/_Debug/x64/新建文本文档.txt
		SIGDN_PARENTRELATIVEFORADDRESSBAR:	新建文本文档.txt
		SIGDN_PARENTRELATIVE:				新建文本文档.txt
		SIGDN_PARENTRELATIVEFORUI:			新建文本文档.txt
		*/

		if(hr == S_OK){
			auto tmp = ::UnicodeToANSI(_targetFileName);
			_targetFileNameInSQL = ::Replace(tmp,"'","''");

			char * sSQLFormater = "Select t.[TAGNAME], \
								  (select count(*) from asso_file_tag a inner join files f on f.[ID]=a.[FILEID] where f.[FULLPATH]='%s' and a.[TAGID]=t.[ID])as asso \
								  from [tags] t order by t.[DISPLAY_ORDER];";
			char sSQL[MAXLENGTH_SQL];
			memset(sSQL,0,MAXLENGTH_SQL * sizeof(char));
			sprintf ( sSQL,sSQLFormater,_targetFileNameInSQL );

			char * pErrMsg = 0;
			int ret = 0;
			ret = sqlite3_open(_dbFile, &db);  
			if ( ret != SQLITE_OK )  
			{  
				::PrintLog("Could not open database: %s", sqlite3_errmsg(db));  
			}
			else
			{
				::PrintLog( L"Successfully connected to database.");  

				auto sSQLUTF8=ANSIToUTF8(sSQL);
				::PrintLog("LoadTags: %s",sSQLUTF8);
				sqlite3_exec( db,sSQLUTF8, _callback_exec_load_tags, this, &pErrMsg );  

				if ( ret != SQLITE_OK )  
				{  
					::PrintLog("SQL error: %s\n", pErrMsg);  
					sqlite3_free(pErrMsg);  
				}

				sqlite3_close(db);  
				db = 0; 
				_cached = true;
			}
		}
	}
}



HRESULT CTaghelper::SetTag(int tagIdx)
{
	HRESULT hr = S_FALSE;
	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pzErrmsg = 0;
	int ret = 0;  

	ret = sqlite3_open(_dbFile, &db);  
	if ( ret != SQLITE_OK )  
	{  
		::PrintLog("Could not open database: %s", sqlite3_errmsg(db));  
	}
	else
	{		
		BOOL isAsso = Tags[tagIdx].bAsso;

		char * FID;
		FID = GetFileID(*db);

		char * TID;
		TID = GetTagID(*db,tagIdx);

		char * sSQLFormater;
		char sSQL[MAXLENGTH_SQL];

		if(!isAsso)
		{
			sSQLFormater = "INSERT INTO [ASSO_FILE_TAG] (FILEID,TAGID) VALUES ('%s','%s')";
			if(FID==FID_NOT_EXIST ){
				FID = InsertFile(*db);
			}
			if(TID==TID_NOT_EXIST){
				TID = InsertTag(*db,tagIdx);
			}
		}else{
			sSQLFormater = "DELETE FROM [ASSO_FILE_TAG] WHERE FILEID='%s' AND TAGID='%s'";
		}
		sprintf ( sSQL,sSQLFormater,FID,TID );

		auto sSQLUTF8=ANSIToUTF8(sSQL);
		::PrintLog("SetTag: %s",sSQLUTF8);
		sqlite3_exec( db, sSQLUTF8, NULL, 0, 0 );
	}

	sqlite3_free_table(pazResult);
	sqlite3_close(db);  
	db = 0; 
	return hr;
}

// get file id in db, return FID_NOT_EXIST if not exists.
char * CTaghelper::GetFileID(sqlite3 & db)
{
	char * FID;
	FID = FID_NOT_EXIST;

	char * sSQLFormater;
	char sSQL[MAXLENGTH_SQL];

	// get file ID
	sSQLFormater = "SELECT [ID] FROM [FILES] WHERE [FULLPATH]='%s'";
	sprintf ( sSQL,sSQLFormater,_targetFileNameInSQL );

	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pzErrmsg = 0;
	int ret = 0;  

	// utf8 used in database
	auto sSQLUTF8=ANSIToUTF8(sSQL);
	::PrintLog("GetFileID: %s",sSQLUTF8);
	ret = sqlite3_get_table(&db,sSQLUTF8,&pazResult,&pnRow,&pnColumn,&pzErrmsg);

	if ( ret != SQLITE_OK )  
	{
		sqlite3_free(pzErrmsg);  
	}
	else
	{
		// record exists
		if(pnRow > 0 && pnColumn > 0)
		{
			FID = pazResult[1];
			::PrintLog("Got file id: %s",FID);
		}
	}
	return FID;
}

char * CTaghelper::GetTagID(sqlite3 & db, int & targetTagIdx)
{
	char * TID;
	TID = TID_NOT_EXIST;

	char * sSQLFormater;
	char sSQL[MAXLENGTH_SQL];

	// get tag ID
	sSQLFormater = "SELECT [ID] FROM [TAGS] WHERE [TAGNAME]='%s'";
	auto tmp = ::UnicodeToANSI(Tags[targetTagIdx].Tag);
	LPSTR _tagInSQL = ::Replace(tmp,"'","''");
	sprintf ( sSQL,sSQLFormater,_tagInSQL );

	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pzErrmsg = 0;
	int ret = 0;  

	// utf8 used in database
	auto sSQLUTF8=ANSIToUTF8(sSQL);
	::PrintLog("GetTagID: %s",sSQLUTF8);
	ret = sqlite3_get_table(&db,sSQLUTF8,&pazResult,&pnRow,&pnColumn,&pzErrmsg);
	if ( ret != SQLITE_OK )  
	{
		sqlite3_free(pzErrmsg);  
	}
	else
	{
		if(pnRow > 0 && pnColumn > 0)
		{
			TID = pazResult[1];
			::PrintLog("Got tag id: %s",TID);
		}
	}

	return TID;
}

char * CTaghelper::InsertFile(sqlite3 & db)
{
	char * sSQLFormater;
	char sSQL[MAXLENGTH_SQL];

	sSQLFormater = "INSERT INTO [FILES] (parent_dir,fullpath,filename,ext,remark) values ('parent_dir','%s','filename','ext','remark')";
	sprintf ( sSQL,sSQLFormater,_targetFileNameInSQL );

	auto sSQLUTF8=ANSIToUTF8(sSQL);
	::PrintLog("InsertFile: %s",sSQLUTF8);
	sqlite3_exec( &db, sSQLUTF8, NULL, 0, 0 );

	return GetFileID(db);
}

char * CTaghelper::InsertTag(sqlite3 & db, int & targetTagIdx)
{
	char * sSQLFormater;
	char sSQL[MAXLENGTH_SQL];

	sSQLFormater = "INSERT INTO [FILES] (TAGNAME,DISPLAY_ORDER) values ('%s','0')";
	auto tmp = ::UnicodeToANSI(Tags[targetTagIdx].Tag);
	LPSTR _tagInSQL = ::Replace(tmp,"'","''");
	sprintf ( sSQL,sSQLFormater,_tagInSQL );

	auto sSQLUTF8=ANSIToUTF8(sSQL);
	::PrintLog("InsertTag: %s",sSQLUTF8);
	sqlite3_exec( &db, sSQLUTF8, NULL, 0, 0 );

	return GetTagID(db,targetTagIdx);
}











/*
写文件
wofstream f;
f.imbue(locale( "", locale::all ^ locale::numeric));  
f.open("E:\\Works\\UDNZ\\udnz.com.ShellEx.TaggableShell\\C++\\_Debug\\x64\\1.d",ios::out|ios::binary,_SH_DENYNO);
f << "IShellItem:" << name;
f.close();
*/



/*
按行读取文件
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
*/
