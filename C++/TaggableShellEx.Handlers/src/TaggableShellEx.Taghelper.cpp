#include "../include/TaggableShellEx.Taghelper.h"

CTaghelper::CTaghelper(void): 
	_cached(false)
	,TagCount(0),FileCount(0)
	,_db(0)
{
	::PrintLog(L"CTaghelper.ctor");

	::WStr2Str(g_UserDb,_dbFile);
}


CTaghelper::~CTaghelper(void)
{
	::PrintLog(L"CTaghelper.~ctor");

	// new by ::WStr2Str in ctor, so need to delete manual.
	delete _dbFile;
	_dbFile = NULL;

	for (UINT i = 0; i < TagCount; i++)
	{
		if(NULL !=Tags[i].Tag)
		{
			delete Tags[i].Tag;
			Tags[i].Tag = NULL;
		}
	}

	sqlite3_close(_db);  
	_db = 0; 
}

int _callback_exec_load_tags(void * tagHelper,int argc, char ** argv, char ** aszColName)  
{  
	if(argv[0] != 0){
		CTaghelper* h = (CTaghelper*)tagHelper;

		auto &tag = h->Tags[h->TagCount];

		tag.TagID = atoi(argv[0]);
		::Str2WStr(argv[1],tag.Tag);
		tag.UseCount = atoi(argv[2]);
		tag.bAsso = *argv[3]!='0';

		::PrintLog( L"Got tag: %s",tag.Tag );

		h->TagCount++;

		if(h->TagCount>= MAXCOUNT_TAG){
			return 1; // returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT without invoking the callback again and without running any subsequent SQL statements.
		}
	}
	return 0;  
}

void CTaghelper::SetCurrentFiles(LPWSTR* ppv,const int count)
{
	_ASSERT_EXPR( ppv != NULL, L"ppv can not be NULL.");

	FileCount = count;
	if( FileCount > 0)
	{
		for (int i = 0; i < count; i++)
		{
			TargetFileNames[i] = ppv[i];

			//HRESULT hr = _targetShellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,(LPWSTR*)&_targetFileName);
			/*
			SIGDN_NORMALDISPLAY:				�½��ı��ĵ�.txt
			SIGDN_PARENTRELATIVEPARSING:		�½��ı��ĵ�.txt
			SIGDN_DESKTOPABSOLUTEPARSING:		E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\�½��ı��ĵ�.txt
			SIGDN_PARENTRELATIVEEDITING:		�½��ı��ĵ�.txt
			SIGDN_DESKTOPABSOLUTEEDITING:		E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\�½��ı��ĵ�.txt
			SIGDN_FILESYSPATH:					E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\�½��ı��ĵ�.txt
			SIGDN_URL:							file:///E:/Works/UDNZ/udnz.com.ShellEx.TaggableShell/C++/_Debug/x64/�½��ı��ĵ�.txt
			SIGDN_PARENTRELATIVEFORADDRESSBAR:	�½��ı��ĵ�.txt
			SIGDN_PARENTRELATIVE:				�½��ı��ĵ�.txt
			SIGDN_PARENTRELATIVEFORUI:			�½��ı��ĵ�.txt
			*/

			_targetFileNamesInSQL[i] = new char[MAXLENGTH_SQL]; // will be deleted automatically. not need to delete in ~ctor.
			memset(_targetFileNamesInSQL[i],0,MAXLENGTH_SQL * sizeof(_targetFileNamesInSQL[i][0]));

			::UnicodeToANSI(TargetFileNames[i],_targetFileNamesInSQL[i]);
			string s(_targetFileNamesInSQL[i]);
			::replace_all_distinct(s,"'","''");
			memcpy(_targetFileNamesInSQL[i],s.c_str(),s.length());
		}
	}
	return;
}

void CTaghelper::LoadTags(bool ignoreCache)
{
	if(ignoreCache || !_cached || TagCount == 0){

		TagCount = 0;

		char * sSQLFormater = NULL;
		char sSQL[MAXLENGTH_SQL];
		memset(sSQL,0,MAXLENGTH_SQL * sizeof(char));

		if( FileCount == 1)
		{
			sSQLFormater = "Select t.[ID],t.[TAGNAME],t.[USECOUNT], \
						   (select count(*) from asso_file_tag a inner join files f on f.[ID]=a.[FILEID] where f.[FULLPATH]='%s' and a.[TAGID]=t.[ID])as asso \
						   from [TAGS] t order by t.[USECOUNT] DESC;";
			sprintf ( sSQL,sSQLFormater,_targetFileNamesInSQL[0] );
		}
		else
		{
			sSQLFormater = "Select t.[ID],t.[TAGNAME],t.[USECOUNT], \
						   '%s' as asso \
						   from [TAGS] t order by t.[USECOUNT] DESC;";

			sprintf ( sSQL,sSQLFormater,"0" );
		}

		char * pErrMsg = 0;
		int ret = 0;
		ret = sqlite3_open(_dbFile, &_db);  
		if ( ret != SQLITE_OK )  
		{  
			::PrintLog("Could not open database: %s", sqlite3_errmsg(_db));  
		}
		else
		{
			::PrintLog( L"Successfully connected to database.");  

			ANSIToUTF8(sSQL);
			::PrintLog("LoadTags: %s",sSQL);
			ret = sqlite3_exec( _db,sSQL, _callback_exec_load_tags, this, &pErrMsg );  

			if ( ret != SQLITE_OK )  
			{  
				::PrintLog("SQL error: %s\n", pErrMsg);  
				sqlite3_free(pErrMsg);  
			}

			sqlite3_close(_db);  
			_db = 0; 
			_cached = true;
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

	ret = sqlite3_open(_dbFile, &_db);  
	if ( ret != SQLITE_OK )  
	{  
		::PrintLog("Could not open database: %s", sqlite3_errmsg(_db));  
	}
	else
	{	
		auto &currentTag = Tags[tagIdx];

		char * TID = NULL;
		TID = GetTagID(currentTag.Tag);
		if( string(TID)==string(TID_NOT_EXIST) ){
			TID = InsertTag(currentTag.Tag,1);
		}

		_ASSERT_EXPR(string(TID)!=string(TID_NOT_EXIST),L"TID is not available!");

		char * sSQLFormater = NULL;
		char sSQL[MAXLENGTH_SQL];

		if( FileCount == 1)
		{
			char * FID = NULL;
			FID = GetFileID(_targetFileNamesInSQL[0]);

			BOOL isAsso = currentTag.bAsso;

			if(!isAsso)
			{
				sSQLFormater = "INSERT INTO [ASSO_FILE_TAG] (FILEID,TAGID) VALUES ('%s','%s');UPDATE [TAGS] SET [USECOUNT]=[USECOUNT]+1 WHERE [ID]=%s";
				if( string(FID)==string(FID_NOT_EXIST) ){
					FID = InsertFile(TargetFileNames[0]);
				}

				_ASSERT_EXPR(string(FID)!=string(FID_NOT_EXIST),L"FID is not available!");

			}else{

				sSQLFormater = "DELETE FROM [ASSO_FILE_TAG] WHERE FILEID='%s' AND TAGID='%s';UPDATE [TAGS] SET [USECOUNT]=[USECOUNT]-1 WHERE [ID]=%s";

				// check any other tag associated with current file.
				BOOL anyAsso = false;
				for (unsigned int i = 0; i < TagCount; i++)
				{
					if(i!=tagIdx && Tags[i].bAsso)
					{
						anyAsso = true;
						break;
					}
				}

				// delete the record for current file in the table [FILES]
				if(!anyAsso){
					char sSQL_tmp[MAXLENGTH_SQL];
					sprintf ( sSQL_tmp,"%s;DELETE FROM [FILES] WHERE ID='%s'",sSQLFormater,FID );
					sSQLFormater = sSQL_tmp;
					::PrintLog("No any tag associated with current file, about to delete from table [FILE]: ID = %d, %s",FID,TargetFileNames[0]);
				}
			}

			sprintf ( sSQL,sSQLFormater,FID,TID,TID );
			ANSIToUTF8(sSQL);
			::PrintLog("SetTag: %s",sSQL);
			sqlite3_exec( _db, sSQL, NULL, 0, 0 );
		}
		else if( FileCount > 1)
		{
			char * FID = NULL;
			for (UINT i = 0; i < FileCount; i++)
			{
				if ( !IsAsso(TargetFileNames[i],currentTag.Tag) )
				{
					FID = GetFileID(_targetFileNamesInSQL[i]);
					if( string(FID)==string(FID_NOT_EXIST) ){
						FID = InsertFile(TargetFileNames[i]);
					}

					_ASSERT_EXPR( string(FID)!=string(FID_NOT_EXIST) ,L"FID is not available!");

					sSQLFormater = "INSERT INTO [ASSO_FILE_TAG] (FILEID,TAGID) VALUES ('%s','%s');UPDATE [TAGS] SET [USECOUNT]=[USECOUNT]+1 WHERE [ID]=%s";
					sprintf ( sSQL,sSQLFormater,FID,TID,TID );
					ANSIToUTF8(sSQL);
					::PrintLog("SetTag: %s",sSQL);
					sqlite3_exec( _db, sSQL, NULL, 0, 0 );
				}
			}
		}
	}

	sqlite3_free_table(pazResult);
	sqlite3_close(_db);  
	_db = 0; 
	return hr;
}

// get file id in _db, return FID_NOT_EXIST if not exists.
// the parameter fileNameInSQL can not contains "'"!
char * CTaghelper::GetFileID(LPSTR fileNameInSQL)
{
	char * FID = FID_NOT_EXIST;

	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL];

	// get file ID
	sSQLFormater = "SELECT [ID] FROM [FILES] WHERE [FULLPATH]='%s'";
	sprintf ( sSQL,sSQLFormater,fileNameInSQL );

	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pzErrmsg = 0;
	int ret = 0;  

	// utf8 used in database
	ANSIToUTF8(sSQL);
	::PrintLog("GetFileID: %s",sSQL);
	ret = sqlite3_get_table(_db,sSQL,&pazResult,&pnRow,&pnColumn,&pzErrmsg);

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

char * CTaghelper::GetTagID(LPWSTR & tag)
{
	char * TID = TID_NOT_EXIST;

	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL];

	// get tag ID
	sSQLFormater = "SELECT [ID] FROM [TAGS] WHERE [TAGNAME]='%s'";

	LPSTR tagInSQL = new char[MAXLENGTH_SQL];
	::UnicodeToANSI(tag,tagInSQL);
	::Replace(tagInSQL,"'","''");
	sprintf ( sSQL,sSQLFormater,tagInSQL );
	delete tagInSQL;

	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pzErrmsg = 0;
	int ret = 0;  

	// utf8 used in database
	ANSIToUTF8(sSQL);
	::PrintLog("GetTagID: %s",sSQL);
	ret = sqlite3_get_table(_db,sSQL,&pazResult,&pnRow,&pnColumn,&pzErrmsg);
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

char * CTaghelper::InsertFile(LPWSTR & targetFile)
{
	_ASSERT_EXPR(NULL != targetFile,"_targetFile could not be NULL");

	char * tmp = new char[MAXLENGTH_SQL];
	::UnicodeToANSI(targetFile,tmp);
	std::tr2::sys::path p(tmp);
	delete tmp;

	// get dir
	string parent_dir = p.parent_path().string();
	::replace_all_distinct(parent_dir,"'","''");

	// get name
	string itemname = p.filename();
	::replace_all_distinct(itemname,"'","''");

	// get ext
	string ext = p.extension();
	::replace_all_distinct(ext,"'","''");

	// get file full name for sql, replace ' to ''
	LPSTR targetFileInSQL = new char[MAXLENGTH_SQL];
	::UnicodeToANSI(targetFile,targetFileInSQL);
	_ASSERT_EXPR(NULL != targetFileInSQL,"_targetFileInSQL could not be NULL");
	string s(targetFileInSQL);
	::replace_all_distinct(s,"'","''");
	memcpy(targetFileInSQL,s.c_str(),s.length());


	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL];

	// get sql
	sSQLFormater = "INSERT INTO [FILES] (parent_dir,fullpath,itemname,ext,remark) values ('%s','%s','%s','%s','')";
	sprintf ( sSQL,sSQLFormater,parent_dir.c_str(),targetFileInSQL,itemname.c_str(),ext.c_str());

	ANSIToUTF8(sSQL);

	::PrintLog("InsertFile: %s",sSQL);
	sqlite3_exec( _db, sSQL, NULL, 0, 0 );

	char* id = GetFileID(targetFileInSQL);
	
	delete targetFileInSQL;
	targetFileInSQL = NULL;

	return id;
}

char * CTaghelper::InsertTag(LPWSTR & newTag, const int useCount)
{
	char * pErrMsg = 0;
	int ret = 0;
	ret = sqlite3_open(_dbFile, &_db);  
	if ( ret != SQLITE_OK )  
	{  
		::PrintLog("Could not open database: %s", sqlite3_errmsg(_db));  
	}
	else
	{	
		char * sSQLFormater = NULL;
		char sSQL[MAXLENGTH_SQL];

		sSQLFormater = "INSERT INTO [TAGS] (TAGNAME,USECOUNT,DISPLAY_ORDER) values ('%s','%d','0');";
		LPSTR tagInSQL = new char[MAXLENGTH_SQL];
		::UnicodeToANSI(newTag,tagInSQL);
		::Replace(tagInSQL,"'","''");
		sprintf ( sSQL,sSQLFormater,tagInSQL,useCount );
		delete tagInSQL;

		ANSIToUTF8(sSQL);

		::PrintLog("InsertTag: %s",sSQL);
		ret = sqlite3_exec( _db,sSQL, NULL, NULL, &pErrMsg );  
		if ( ret == SQLITE_OK )  
		{
			// reload tags.
			LoadTags(true);
		}
		else
		{  
			::PrintLog("SQL error: %s\n", pErrMsg);  
			sqlite3_free(pErrMsg);  
		}
	}
	return GetTagID(newTag);
}

BOOL CTaghelper::IsAsso(LPCWSTR file, LPCWSTR tag)
{
	return FALSE;
}

BOOL CTaghelper::IsTagExists(LPCWSTR & tag)
{
	if(TagCount > 0)
	{
		for (UINT i = 0; i < TagCount; i++)
		{
			// UNDONE: check if the new tag exists already.
			if(tag == Tags[i].Tag)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
/*
д�ļ�
wofstream f;
f.imbue(locale( "", locale::all ^ locale::numeric));  
f.open("E:\\Works\\UDNZ\\udnz.com.ShellEx.TaggableShell\\C++\\_Debug\\x64\\1.d",ios::out|ios::binary,_SH_DENYNO);
f << "IShellItem:" << name;
f.close();
*/



/*
���ж�ȡ�ļ�
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
