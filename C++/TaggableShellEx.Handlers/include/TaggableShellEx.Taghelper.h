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

	void SetCurrentFiles(IShellItem** ppv,const int count);
	void LoadTags(bool ignoreCache = false);
	HRESULT SetTag(int tagIdx);

	unsigned int TagCount;				// The count of loaded tags. READ ONLY.
	TAG4FILE Tags[MAXCOUNT_TAG];		// The loaded tags. READ ONLY.

	BOOL IsTagExists(LPCWSTR & tag);
	char * GetFileID();
	char * GetTagID(LPWSTR & tag);
	char * InsertFile(LPWSTR & targetFile);
	char * InsertTag(LPWSTR & newTag,const int useCount);

private:

	bool _cached;
	LPSTR _dbFile;
	LPWSTR _targetFileName;			// the file full path of target item.
	LPSTR _targetFileNameInSQL;		// the file full path of target item with "'" replaced to "''" for SQL.
	IShellItem* _targetShellItem;
	sqlite3 * _db;

};

