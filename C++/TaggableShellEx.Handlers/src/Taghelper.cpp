#include "..\include\Taghelper.h"
#include "..\include\CBackgroundThread.h"

/*
1. delete records from ASSO_FILE_TAG where file,tag does not exist any more.
2. delete records from FILE which is associated with no any tag.
3. update USECOUNT of tags.
*/
const LPSTR sSQL_CLEARNUP 
	= "\
	  DELETE FROM [ASSO_FILE_TAG] WHERE [TAGID] NOT IN(SELECT [ID] FROM [TAGS]) OR [FILEID] NOT IN (SELECT [ID] FROM [FILES]);\r\n\
	  DELETE FROM [FILES] WHERE [ID] NOT IN (SELECT [FILEID] FROM [ASSO_FILE_TAG]);\r\n\
	  UPDATE [TAGS] SET [USECOUNT]=(SELECT COUNT(*) FROM [ASSO_FILE_TAG] A WHERE A.[TAGID]=[TAGS].[ID]);\
	  ";

CTaghelper* CTaghelper::p_instance_ = NULL;

LRESULT CALLBACK Callback_SetTagByRecordId();


CTaghelper::CTaghelper(void):
	_cRef(1)
	,_cached(false)
	,TagCount(0),FileCount(0)
	,_db(NULL)
	,m_mutex(NULL)
	,_Asyn_currentTag(NULL)
{
	::PrintLog(L"CTaghelper.ctor");

	m_mutex = ::CreateMutex(NULL, FALSE, NULL);

	for each (LPWSTR var in TargetFileNames) { var = NULL; }
	for each (LPSTR var in _targetFileNamesInSQL) { var = NULL; }
	for each (TAG4FILE var in Tags) {
		var.bAsso= false; 
		memset(var.Tag,0,ARRAYSIZE(var.Tag));
		var.TagID = 0;
		var.TagIdx = 0;
		var.UseCount = 0;
	}

	LPSTR tmp;
	::WStr2Str(g_UserDb,tmp);
	StrCpyA(_dbFile,tmp);
	_DELETE( tmp );
}


CTaghelper::~CTaghelper(void)
{
	::PrintLog(L"CTaghelper.~ctor");

	try{
		for (UINT i = 0; i < FileCount; i++)
		{
			if(NULL !=TargetFileNames[i])
			{
				_DELETE(TargetFileNames[i]);
			}
		}
	}catch(...){}

	try{
		for (UINT i = 0; i < FileCount; i++)
		{
			if(NULL !=_targetFileNamesInSQL[i])
			{
				_DELETE(_targetFileNamesInSQL[i]);
			}
		}
	}catch(...){}

	sqlite3_close(_db);  
	_db = NULL;

	if( NULL != m_mutex){
		::CloseHandle(m_mutex);
		m_mutex = NULL;
	}
}

ULONG CTaghelper::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

ULONG CTaghelper::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (!cRef)
		delete this;
	return cRef;
}

BOOL CTaghelper::OpenDb()
{
	if ( NULL == _db ){
		_ASSERT_EXPR(_dbFile != NULL,L"_dbFile can not be NULL.");
		int ret = sqlite3_open_v2(_dbFile, &_db,SQLITE_OPEN_READWRITE,NULL);  
		if ( ret == SQLITE_OK )  
		{
			::PrintLog( L"Successfully connected to database.");  
		}
		else
		{  
			::PrintLog("Could not open database(0x%x): %s",ret, sqlite3_errmsg(_db));
			sqlite3_close(_db);  
			_db = NULL; 
		}
	}

	return NULL != _db;
}

int _callback_exec_load_tags(void * tagHelper,int argc, char ** argv, char ** aszColName)  
{  
	if(argv[0] != 0){
		CTaghelper* h = (CTaghelper*)tagHelper;

		auto &tag = h->Tags[h->TagCount];

		tag.bAsso = *argv[3]!='0';
		tag.TagID = atoi(argv[0]);
		tag.TagIdx = h->TagCount;
		tag.UseCount = atoi(argv[2]);
		LPWSTR tmpTag;
		::Str2WStr(argv[1],tmpTag);
		memcpy(tag.Tag,tmpTag,ARRAYSIZE(tag.Tag));
		delete tmpTag;

		::PrintLog( L"Got tag: %s, TagID=%d, TagIdx=%d, UseCount=%d, bAsso=%d",tag.Tag,tag.TagID,tag.TagIdx,tag.UseCount,tag.bAsso );

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
			TargetFileNames[i] = new wchar_t[MAXLENGTH_SQL]; // will be deleted in ~ctor.
			_targetFileNamesInSQL[i] = new char[MAXLENGTH_SQL]; // will be deleted in ~ctor.

			StrCpy(TargetFileNames[i],ppv[i]);

			::UnicodeToANSI(TargetFileNames[i],_targetFileNamesInSQL[i]);
			string s(_targetFileNamesInSQL[i]);
			::replace_all_distinct(s,"'","''");
			StrCpyA(_targetFileNamesInSQL[i],s.c_str());
		}
	}
	return;
}

void CTaghelper::LoadTags(bool ignoreCache)
{
	if(ignoreCache || !_cached || TagCount == 0){

		WaitForSingleObject(m_mutex, INFINITE);

		TagCount = 0;

		char * sSQLFormater = NULL;
		char sSQL[MAXLENGTH_SQL] = {0};

		// attach the SQL for clean up.
		strcat(sSQL,sSQL_CLEARNUP);
		strcat(sSQL,";\r\n");

		char sSQL_SELECT_TMP[MAXLENGTH_SQL] = {0};
		if( FileCount == 1)
		{
			sSQLFormater = "Select t.[ID],t.[TAGNAME],t.[USECOUNT], \
						   (select count(*) from asso_file_tag a inner join files f on f.[ID]=a.[FILEID] where f.[FULLPATH]='%s' and a.[TAGID]=t.[ID])as asso \
						   from [TAGS] t order by t.[USECOUNT] DESC;";
			sprintf ( sSQL_SELECT_TMP,sSQLFormater,_targetFileNamesInSQL[0] );
		}
		else
		{
			sSQLFormater = "Select t.[ID],t.[TAGNAME],t.[USECOUNT], \
						   '%s' as asso \
						   from [TAGS] t order by t.[USECOUNT] DESC;";

			sprintf ( sSQL_SELECT_TMP,sSQLFormater,"0" );
		}

		strcat(sSQL,sSQL_SELECT_TMP);

		char * pErrMsg = 0;
		int ret = 0;

		ANSIToUTF8(sSQL);
		::PrintLog("LoadTags: %s",sSQL);
		ret = sqlite3_exec( _db,sSQL, _callback_exec_load_tags, this, &pErrMsg );  
		if ( ret != SQLITE_OK )  
		{  
			::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
			sqlite3_free(pErrMsg);  
		}

		_cached = true;

		ReleaseMutex(m_mutex);
	}
}

HRESULT CTaghelper::AsynSetTagByRecordId(HWND progressBarHwnd,UINT & tagIdInDb)
{
	_ASSERT_EXPR( tagIdInDb > 0,L"tagIdInDb must be greater than 0.");

	LoadTags(true);

	for (UINT i = 0; i < TagCount; i++)
	{
		if( Tags[i].TagID == tagIdInDb )
		{
			_Asyn_currentTag = &Tags[i];
			break;
		}
	}

	if ( NULL != _Asyn_currentTag )
	{
		IOperationsProgressDialog *_pOPD = NULL;
		ShowProgressDlg(progressBarHwnd,_pOPD);
		CBackgroundThread<CTaghelper,IOperationsProgressDialog> *pfrobt = new (std::nothrow) CBackgroundThread<CTaghelper,IOperationsProgressDialog>(this);
		if (pfrobt)
		{
			pfrobt->StartThread(_pOPD);
			pfrobt->Release();
		}
	}

	return S_OK;
}

HRESULT CTaghelper::AsynSetTagByIdx(HWND progressBarHwnd,UINT & tagIdx)
{
	_ASSERT_EXPR( tagIdx < TagCount,L"tagIdx must be less than TagCount.");
	return AsynSetTagByRecordId(progressBarHwnd,Tags[tagIdx].TagID);
}

// get file id in _db, return DB_RECORD_NOT_EXIST if fail.
// the parameter fileNameInSQL can not contains "'"!
UINT CTaghelper::GetFileID(LPSTR* pFileNameInSQL)
{
	UINT FID = DB_RECORD_NOT_EXIST;

	LPSTR fileNameInSQL = *pFileNameInSQL;
	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL] = {0};

	// get file ID
	sSQLFormater = "SELECT [ID] FROM [FILES] WHERE [FULLPATH]='%s'";
	sprintf ( sSQL,sSQLFormater,fileNameInSQL );

	// utf8 used in database
	ANSIToUTF8(sSQL);
	::PrintLog("GetFileID: %s",sSQL);

	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pErrMsg = 0;
	int ret = 0;  
	ret = sqlite3_get_table(_db,sSQL,&pazResult,&pnRow,&pnColumn,&pErrMsg);

	if ( ret != SQLITE_OK )  
	{
		::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
		sqlite3_free(pErrMsg);  
	}
	else
	{
		// record exists
		if(pnRow > 0 && pnColumn > 0)
		{
			int id = atoi(pazResult[1]);
			if( id > 0 ) 
				FID = id;
			::PrintLog("[Find file id exists] Got file id: %d",FID);
		}else{
			::PrintLog("[Find file id exists] Fail to get file id.");
		}

		sqlite3_free_table(pazResult);
	}

	return FID;
}

UINT CTaghelper::GetTagID(LPWSTR & tag)
{
	UINT TID = DB_RECORD_NOT_EXIST;

	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL];

	// get tag ID
	sSQLFormater = "SELECT [ID] FROM [TAGS] WHERE [TAGNAME]='%s'";

	LPSTR tagInSQL = new char[MAXLENGTH_SQL];
	::UnicodeToANSI(tag,tagInSQL);
	::Replace(tagInSQL,"'","''");
	sprintf ( sSQL,sSQLFormater,tagInSQL );
	delete tagInSQL;

	// utf8 used in database
	ANSIToUTF8(sSQL);
	::PrintLog("GetTagID: %s",sSQL);

	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pErrMsg = 0;
	int ret = 0;  
	ret = sqlite3_get_table(_db,sSQL,&pazResult,&pnRow,&pnColumn,&pErrMsg);
	if ( ret != SQLITE_OK )  
	{
		::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
		sqlite3_free(pErrMsg);  
	}
	else
	{
		if(pnRow > 0 && pnColumn > 0)
		{
			int id = atoi(pazResult[1]);
			if( id > 0 ) 
				TID = id;
			::PrintLog("Got tag id: %d",TID);
		}else{
			::PrintLog("Fail to get tag id.");
		}
		sqlite3_free_table(pazResult);
	}

	return TID;
}

UINT CTaghelper::InsertFile(LPWSTR & targetFile)
{
	_ASSERT_EXPR(NULL != targetFile,L"_targetFile could not be NULL");

	char tmp[MAX_PATH] = {0};
	::UnicodeToANSI(targetFile,tmp);
	std::tr2::sys::path p(tmp);

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
	char targetFileInSQL[MAX_PATH] = {0};
	::UnicodeToANSI(targetFile,targetFileInSQL);
	_ASSERT_EXPR(NULL != targetFileInSQL,L"_targetFileInSQL could not be NULL");
	string s(targetFileInSQL);
	::replace_all_distinct(s,"'","''");
	memcpy(targetFileInSQL,s.c_str(),s.length());


	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL];

	// get sql
	sSQLFormater = "INSERT INTO [FILES] (parent_dir,fullpath,itemname,ext,remark) values ('%s','%s','%s','%s','');SELECT [ID] FROM [FILES] WHERE [FULLPATH]='%s'";
	sprintf ( sSQL,sSQLFormater,parent_dir.c_str(),targetFileInSQL,itemname.c_str(),ext.c_str(),targetFileInSQL);

	ANSIToUTF8(sSQL);

	UINT id;
	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pErrMsg = 0;
	int ret = 0;  
	::PrintLog("InsertFile: %s",sSQL);
	//ret = sqlite3_exec( _db, sSQL, NULL, 0, &pErrMsg );
	ret = sqlite3_get_table(_db,sSQL,&pazResult,&pnRow,&pnColumn,&pErrMsg);
	if ( ret != SQLITE_OK )  
	{
		::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
		sqlite3_free(pErrMsg);  
	}
	else
	{
		// record exists
		if(pnRow > 0 && pnColumn > 0)
		{
			int idTmp = atoi(pazResult[1]);
			if( idTmp > 0 ) 
				id = idTmp;
			::PrintLog("[Insert file & get the new id] Got file id: %d",idTmp);
		}else{
			::PrintLog("[Insert file & get the new id] Fail to get file id.");
		}

		sqlite3_free_table(pazResult);
	}

	return id;
}

UINT CTaghelper::InsertTag(LPWSTR & newTag)
{
	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL] = {0};
	
	// attach the SQL for clean up first.
	strcat(sSQL,sSQL_CLEARNUP);	
	strcat(sSQL,";\r\n");

	sSQLFormater = "INSERT INTO [TAGS] (TAGNAME,USECOUNT,DISPLAY_ORDER) values ('%s','0','0');";
	char tagInSQL[MAXLENGTH_EACHTAG] = {0};
	::UnicodeToANSI(newTag,tagInSQL);
	::Replace(tagInSQL,"'","''");
	sprintf ( sSQL,sSQLFormater,tagInSQL );

	ANSIToUTF8(sSQL);

	// select the inserted tag id. must be the last query for sqlite3_get_table.
	strcat(sSQL,"SELECT [ID] FROM [TAGS] WHERE [TAGNAME]='");
	strcat(sSQL,tagInSQL);
	strcat(sSQL,"';");

	UINT id = 0;
	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pErrMsg = 0;
	int ret = 0;
	::PrintLog("InsertTag: %s",sSQL);
	//ret = sqlite3_exec( _db,sSQL, NULL, NULL, &pErrMsg );  
	ret = sqlite3_get_table(_db,sSQL,&pazResult,&pnRow,&pnColumn,&pErrMsg);
	if ( ret != SQLITE_OK )  
	{  
		::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
		sqlite3_free(pErrMsg);  
	}
	else
	{
		// record exists
		if(pnRow > 0 && pnColumn > 0)
		{
			int idTmp = atoi(pazResult[1]);
			if( idTmp > 0 ) 
				id = idTmp;
			::PrintLog("[InsertTag] Got tag id: %d",idTmp);
		}else{
			::PrintLog("[InsertTag] Fail to get tag id.");
		}

		sqlite3_free_table(pazResult);

		_cached = false;	// reload tags.
	}

	return id;
}

void CTaghelper::DeleteTags(wchar_t tags[MAXCOUNT_TAG][MAXLENGTH_EACHTAG], const int count)
{
	_ASSERT_EXPR(tags != NULL, L"tags could not be NULL");
	_ASSERT_EXPR(count > 0,L"count must be greater than 0.");

	LPWSTR t = NULL;
	char tagInSQL[MAXLENGTH_EACHTAG] = {0};
	char sSQL[MAXLENGTH_SQL] = {0};
	strcat(sSQL,"DELETE FROM [TAGS] WHERE 1=0 ");
	for (int i = 0; i < count; i++)
	{
		t = tags[i];
		if( t != NULL )
		{
			::UnicodeToANSI(t,tagInSQL);
			::Replace(tagInSQL,"'","''");
			strcat(sSQL," OR [TAGNAME]='");
			strcat(sSQL,tagInSQL);
			strcat(sSQL,"' ");
		}
	}

	// attach the SQL for clean up.
	strcat(sSQL,";\r\n");
	strcat(sSQL,sSQL_CLEARNUP);

	char * pErrMsg = 0;
	int ret = 0;
	ANSIToUTF8(sSQL);
	::PrintLog("Delete Tags: %s",sSQL);
	ret = sqlite3_exec( _db,sSQL, NULL, NULL, &pErrMsg );  
	if ( ret != SQLITE_OK )  
	{  
		::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
		sqlite3_free(pErrMsg);			
	}
	else
	{
		_cached = false;	// reload tags.
	}
}

BOOL CTaghelper::IsAsso(LPCWSTR file, LPCWSTR tag)
{
	// TODO: check is associated already.
	return FALSE;
}

BOOL CTaghelper::IsTagExists(LPCWSTR & tag)
{
	// reload again.
	LoadTags(true);

	if(TagCount > 0)
	{
		for (UINT i = 0; i < TagCount; i++)
		{
			if(wstring(tag) == wstring(Tags[i].Tag))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CTaghelper::RenameTag(LPCWSTR & oldTag, LPCWSTR & newTag)
{
	BOOL x = FALSE;
	char oldTagInSQL[MAXLENGTH_EACHTAG] = {0};
	::UnicodeToANSI(oldTag,oldTagInSQL);
	::Replace(oldTagInSQL,"'","''");

	char newTagInSQL[MAXLENGTH_EACHTAG] = {0};
	::UnicodeToANSI(newTag,newTagInSQL);
	::Replace(newTagInSQL,"'","''");

	size_t len = strlen(newTagInSQL);
	if( len > 0 )
	{
		x = IsTagExists(newTag);
		if( !x )
		{
			char * sSQLFormater = NULL;
			char sSQL[MAXLENGTH_SQL] = {0};
			sSQLFormater = "UPDATE [TAGS] SET [TAGNAME]='%s' WHERE [TAGNAME]='%s';";
			sprintf ( sSQL,sSQLFormater,newTagInSQL,oldTagInSQL );

			char * pErrMsg = 0;
			int ret = 0;
			ANSIToUTF8(sSQL);
			::PrintLog("RenameTag: %s",sSQL);
			ret = sqlite3_exec( _db,sSQL, NULL, NULL, &pErrMsg );  
			if ( ret != SQLITE_OK )  
			{  
				::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
				sqlite3_free(pErrMsg);
				x = FALSE;
			}
			else
			{
				// reload again.
				LoadTags(true);
				x = TRUE;
			}
		}
	}
	return x;
}

HRESULT CTaghelper::GetFilesByTagID(LPWSTR* & files,UINT & count,const UINT tagIdInDb)
{
	_ASSERT_EXPR( files != NULL ,L"files could not be null.");

	count = 0;

	if ( tagIdInDb < 1 )
		return S_FALSE;

	char * sSQLFormater = NULL;
	char sSQL[MAXLENGTH_SQL];

	sSQLFormater = "SELECT [FULLPATH] FROM FILES f \
				   INNER JOIN [ASSO_FILE_TAG] a on f.[ID] = a.[FILEID]\
				   INNER JOIN [TAGS] t on t.[ID] = a.[TAGID]\
				   WHERE t.[ID] = '%d'\
				   ORDER BY f.[ID] DESC";

	sprintf ( sSQL,sSQLFormater,tagIdInDb );

	// utf8 used in database
	ANSIToUTF8(sSQL);
	::PrintLog("GetFilesByTagID: %s",sSQL);

	char** pazResult = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char * pErrMsg = 0;
	int ret = 0;  
	ret = sqlite3_get_table(_db,sSQL,&pazResult,&pnRow,&pnColumn,&pErrMsg);
	if ( ret != SQLITE_OK )  
	{
		::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
		sqlite3_free(pErrMsg);  
	}
	else
	{
		if(pnRow > 0 && pnColumn > 0)
		{
			for (int i = 1; i <= pnRow; i++)
			{
				LPWSTR fname;
				::Str2WStr( pazResult[i],fname);
				files[count++] = fname;

				::PrintLog(L"Got file: %s",fname);
			}
		}else{
			::PrintLog("Fail to get file associated with tag: id = %d.",tagIdInDb);
		}
		sqlite3_free_table(pazResult);
	}

	return S_OK;
}

HRESULT CTaghelper::ShowProgressDlg(HWND hwnd,IOperationsProgressDialog * & _pOPD)
{
	//
	// Initialize COM as STA.
	//
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); 
	if (SUCCEEDED(hr))
	{
		IFileOperation *pfo;

		//
		// Create the IFileOperation interface 
		//
		hr = CoCreateInstance(CLSID_FileOperation, 
			NULL, 
			CLSCTX_ALL, 
			IID_PPV_ARGS(&pfo));
		if (SUCCEEDED(hr))
		{
			//
			// Set the operation flags. Turn off all UI from being shown to the
			// user during the operation. This includes error, confirmation,
			// and progress dialogs.
			//
			hr = pfo->SetOperationFlags(FOFX_DONTDISPLAYDESTPATH | FOFX_NOCOPYHOOKS | FOFX_NOSKIPJUNCTIONS);
			if (SUCCEEDED(hr))
			{
				if (SUCCEEDED(hr))
				{
					hr = CoCreateInstance(CLSID_ProgressDialog, 
						NULL, 
						CLSCTX_ALL, 
						IID_PPV_ARGS(&_pOPD));

					if (SUCCEEDED(hr))
					{
						hr = _pOPD->StartProgressDialog(hwnd, PROGDLG_MODAL | PROGDLG_NOCANCEL | PROGDLG_AUTOTIME | OPPROGDLG_DONTDISPLAYLOCATIONS | OPPROGDLG_DONTDISPLAYDESTPATH );						
						hr = _pOPD->SetMode(PDM_RUN);
						hr = _pOPD->SetOperation(SPACTION_CALCULATING);
						hr = pfo->SetProgressDialog(_pOPD);						
					}
				}

				if (SUCCEEDED(hr))
				{
					// Perform the operation.
					hr = pfo->PerformOperations();
				}        
			}

			//
			// Release the IFileOperation interface.
			//
			pfo->Release();
		}

		CoUninitialize();
	}
	return hr;
}

// Do real work for AsynSetTagByRecordId
HRESULT CTaghelper::DoWorkAsyn(IOperationsProgressDialog * & _pOPD)
{
	_ASSERT_EXPR( _pOPD != NULL ,L"_pOPD could not be NULL");
	
	HRESULT hr = S_FALSE;

	// found the tag
	if ( _Asyn_currentTag != NULL ){
		_ASSERT_EXPR( _Asyn_currentTag != NULL ,L"_Asyn_currentTag could not be NULL");

		char * pErrMsg = 0;
		int ret = 0;  

		UINT TID = _Asyn_currentTag->TagID;

		char sSQLFormater[MAXLENGTH_SQL] = {0};
		char sSQL[MAXLENGTH_SQL] = {0};

		if( FileCount == 1) // toggle the association state between file and tag.
		{
			BOOL isAsso = _Asyn_currentTag->bAsso;
			UINT FID = GetFileID(&_targetFileNamesInSQL[0]);
			if(!isAsso)
			{
				StrCpyA(sSQLFormater,"INSERT INTO [ASSO_FILE_TAG] (FILEID,TAGID) VALUES ('%d','%d');UPDATE [TAGS] SET [USECOUNT]=[USECOUNT]+1 WHERE [ID]=%d;");
				if( FID == DB_RECORD_NOT_EXIST ){
					FID = InsertFile(TargetFileNames[0]);
				}

				_ASSERT_EXPR(FID > 0 && FID != DB_RECORD_NOT_EXIST,L"FID is not available!");

			}else{

				StrCpyA(sSQLFormater,"DELETE FROM [ASSO_FILE_TAG] WHERE FILEID='%d' AND TAGID='%d';UPDATE [TAGS] SET [USECOUNT]=[USECOUNT]-1 WHERE [ID]=%d;");

				// check any other tag associated with current file.
				BOOL anyAsso = false;
				for (unsigned int i = 0; i < TagCount; i++)
				{
					if( Tags[i].TagID != _Asyn_currentTag->TagID && Tags[i].bAsso)
					{
						anyAsso = true;
						break;
					}
				}

				// delete the record for current file in the table [FILES]
				if(!anyAsso){
					char sSQL_tmp[MAXLENGTH_SQL];
					sprintf ( sSQL_tmp,"DELETE FROM [FILES] WHERE ID='%d';",FID );
					strcat(sSQLFormater,sSQL_tmp);
					::PrintLog("No any tag associated with current file, about to delete from table [FILE]: ID = %d, %s",FID,TargetFileNames[0]);
				}
			}

			sprintf ( sSQL,sSQLFormater,FID,TID,TID );
			ANSIToUTF8(sSQL);
			::PrintLog("DoWorkAsyn: %s",sSQL);
			ret = sqlite3_exec( _db, sSQL, NULL, 0, &pErrMsg );
			if( ret != SQLITE_OK)
			{
				::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
				sqlite3_free(pErrMsg);  
			}
		}		
		else if( FileCount > 1) // associate the tag with specificed files no matter whether it is associated already or not.
		{
			UINT FID = DB_RECORD_NOT_EXIST;
			for (UINT i = 0; i < FileCount; i++)
			{
				FID = GetFileID(&_targetFileNamesInSQL[i]);
				if( FID == DB_RECORD_NOT_EXIST ){
					FID = InsertFile(TargetFileNames[i]);
				}

				_ASSERT_EXPR(FID > 0 && FID != DB_RECORD_NOT_EXIST,L"FID is not available!");

				if ( !IsAsso(TargetFileNames[i],_Asyn_currentTag->Tag) )
				{
					StrCpyA(sSQLFormater,"INSERT INTO [ASSO_FILE_TAG] (FILEID,TAGID) VALUES ('%d','%d');UPDATE [TAGS] SET [USECOUNT]=[USECOUNT]+1 WHERE [ID]=%d;");
					sprintf ( sSQL,sSQLFormater,FID,TID,TID );
					ANSIToUTF8(sSQL);
					::PrintLog("SetTag: %s",sSQL);
					ret = sqlite3_exec( _db, sSQL, NULL, 0, &pErrMsg );
					if( ret != SQLITE_OK)
					{
						::PrintLog("SQL error(0x%x): %s", ret,pErrMsg);  
						sqlite3_free(pErrMsg);  
					}
				}

				// update the progress
				hr = _pOPD->UpdateProgress(i+1,FileCount,0,0,i+1,FileCount);

				/*
				IShellItem *psi;
				hr = SHCreateItemFromParsingName(TargetFileNames[i],NULL,IID_PPV_ARGS(&psi));
				if( SUCCEEDED(hr) )
				{
				hr = _pOPD->UpdateLocations(psi,NULL,NULL);	// TODO: will get a Catastrophic failure. not sure why.
				psi->Release();
				}
				*/
			}
		}

		// done. clean up the progress dialog.
		hr = _pOPD->StopProgressDialog();
		_pOPD->Release();

		_cached = false;	// reload tags.
		_Asyn_currentTag = NULL;

	}else{
		// can not find the tag
		hr = S_FALSE;
	}
	
	Callback_SetTagByRecordId();

	return hr;
}
