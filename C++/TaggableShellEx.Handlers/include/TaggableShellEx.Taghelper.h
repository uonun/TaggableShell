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

	void LoadTags(IShellItem & ppv);
	HRESULT SetTag(int tagIdx);

	unsigned int TagCount;				// The count of loaded tags. READ ONLY.
	TAG4FILE Tags[MAXCOUNT_TAG];		// The loaded tags. READ ONLY.

	BOOL IsTagExists(LPCWSTR & tag);

private:
	char * GetFileID(sqlite3 & db);
	char * GetTagID(sqlite3 & db, LPWSTR & tag);
	char * InsertFile(sqlite3 & db, LPWSTR & targetFile);
	char * InsertTag(sqlite3 & db, LPWSTR & newTag);

	bool _cached;
	LPSTR _dbFile;
	LPWSTR _targetFileName;			// the file full path of target item.
	LPSTR _targetFileNameInSQL;		// the file full path of target item with "'" replaced to "''" for SQL.
	IShellItem* _targetShellItem;
	sqlite3 * db;  

};

