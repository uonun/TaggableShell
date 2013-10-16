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

	void SetCurrentFiles(LPWSTR* ppv,const int count);
	void LoadTags(bool ignoreCache = false);
	HRESULT SetTag(int tagIdx);

	LPWSTR TargetFileNames[MAXCOUNT_ITEM];			// the file full path of target item.
	UINT FileCount;
	UINT TagCount;				// The count of loaded tags. READ ONLY.
	TAG4FILE Tags[MAXCOUNT_TAG];		// The loaded tags. READ ONLY.

	BOOL IsAsso(LPCWSTR file, LPCWSTR tag);
	BOOL IsTagExists(LPCWSTR & tag);
	char * GetFileID(LPSTR fileNameInSQL);
	char * GetTagID(LPWSTR & tag);
	char * InsertFile(LPWSTR & targetFile);
	char * InsertTag(LPWSTR & newTag,const int useCount);

private:
	bool _cached;									// does tags loaded from database cached.
	LPSTR _targetFileNamesInSQL[MAXCOUNT_ITEM];		// the file full path of target item with "'" replaced to "''" for SQL.
	
	sqlite3 * _db;
	LPSTR _dbFile;

};

