#pragma once
#include "dllmain.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;
#include "../sqlite-201310070036/src/sqlite3.h"


typedef struct tagTAG4FILE
{
	bool bAsso;		// is the tag associated with current target file. makes no sense if there are multi-filse specificed.
	UINT TagID;		// the ID of current tag in database.
	UINT TagIdx;	// the index of current tag in CTaghelper::Tags
	UINT UseCount;	// the count of files associated with current tag.
	WCHAR Tag[MAXLENGTH_EACHTAG];		// the name of current tag in database.
}TAG4FILE,*LPTAG4FILE;

class CTaghelper
{
public:
	BOOL OpenDb();

	void SetCurrentFiles(LPWSTR* ppv,const int count);
	void LoadTags(bool ignoreCache = false);

	// Bind/Unbind tag to files by index of tag in the array of Tags.
	HRESULT SetTagByIdx(UINT & tagIdx);
	// Bind/Unbind tag to files by record ID in database.
	HRESULT SetTagByRecordId(UINT & tagIdInDb);

	LPWSTR TargetFileNames[MAXCOUNT_SELECTED_ITEM];			// the file full path of target item.
	UINT FileCount;
	UINT TagCount;				// The count of loaded tags. READ ONLY.
	TAG4FILE Tags[MAXCOUNT_TAG];		// The loaded tags. READ ONLY.

	BOOL IsAsso(LPCWSTR file, LPCWSTR tag);
	BOOL IsTagExists(LPCWSTR & tag);

	UINT GetFileID(LPSTR* fileNameInSQL);				// return DB_RECORD_NOT_EXIST if fail
	UINT GetTagID(LPWSTR & tag);						// return DB_RECORD_NOT_EXIST if fail
	UINT InsertFile(LPWSTR & targetFile);				// return DB_RECORD_NOT_EXIST if fail
	UINT InsertTag(LPWSTR & newTag,const int useCount);	// return DB_RECORD_NOT_EXIST if fail

	void DeleteTags(wchar_t tags[MAXCOUNT_TAG][MAXLENGTH_EACHTAG], const int count);

	HRESULT GetFilesByTagID(LPWSTR* & files,UINT & count,const UINT tagIdInDb);

#pragma region singleton
public:
	static CTaghelper *instance () 
	{ 
		if (0 == p_instance_) { 
			static CTaghelper instance; 
			p_instance_ = &instance; 
		} 
		return p_instance_; 
	} 


private :
	CTaghelper();
	~CTaghelper();

	// use instance() instead!
	static CTaghelper *p_instance_; 
	
	// mutex lock
	HANDLE m_mutex;

#pragma endregion

private :
	bool _cached;									// does tags loaded from database cached.
	LPSTR _targetFileNamesInSQL[MAXCOUNT_SELECTED_ITEM];		// the file full path of target item with "'" replaced to "''" for SQL.

	sqlite3 * _db;
	CHAR _dbFile[MAX_PATH];
};

