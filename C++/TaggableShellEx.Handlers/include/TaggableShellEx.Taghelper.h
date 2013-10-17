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
	bool bAsso;	
	LPWSTR Tag;		// the name of current tag in database.
	UINT TagID;		// the ID of current tag in database.
	UINT TagIdx;	// the index of current tag in CTaghelper::Tags
	UINT UseCount;	// the count of files associated with current tag.
}TAG4FILE,*LPTAG4FILE;

class CTaghelper
{
public:
	CTaghelper(void);
	virtual ~CTaghelper(void);

	BOOL OpenDb();

	void SetCurrentFiles(LPWSTR* ppv,const int count);
	void LoadTags(bool ignoreCache = false);

	// Bind/Unbind tag to files by index of tag in the array of Tags.
	HRESULT SetTagByIdx(UINT tagIdx);
	// Bind/Unbind tag to files by record ID in database.
	HRESULT SetTagByRecordId(UINT tagIdInDb);

	LPWSTR TargetFileNames[MAXCOUNT_ITEM];			// the file full path of target item.
	UINT FileCount;
	UINT TagCount;				// The count of loaded tags. READ ONLY.
	TAG4FILE Tags[MAXCOUNT_TAG];		// The loaded tags. READ ONLY.

	BOOL IsAsso(LPCWSTR file, LPCWSTR tag);
	BOOL IsTagExists(LPCWSTR & tag);
	
	UINT GetFileID(LPSTR fileNameInSQL);				// return DB_RECORD_NOT_EXIST if fail
	UINT GetTagID(LPWSTR & tag);						// return DB_RECORD_NOT_EXIST if fail
	UINT InsertFile(LPWSTR & targetFile);				// return DB_RECORD_NOT_EXIST if fail
	UINT InsertTag(LPWSTR & newTag,const int useCount);	// return DB_RECORD_NOT_EXIST if fail

	void DeleteTags(wchar_t** tags, const int count);

private:
	bool _cached;									// does tags loaded from database cached.
	LPSTR _targetFileNamesInSQL[MAXCOUNT_ITEM];		// the file full path of target item with "'" replaced to "''" for SQL.

	sqlite3 * _db;
	LPSTR _dbFile;

};

