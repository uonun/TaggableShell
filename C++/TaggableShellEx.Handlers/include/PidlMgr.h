#pragma once
#include "../include/stdafx.h"

#define MYSHITEMTYPE_TAG	DWORD(1);
#define MYSHITEMTYPE_FILE	DWORD(2);

// Data structure to be stored in our PIDLs.
typedef struct tagMYPIDLDATA {
	USHORT cb;	// MAKE SURE cb = sizeof(MYPIDLDATA)
	DWORD Type;	// MYSHITEMTYPE_TAG = tag, MYSHITEMTYPE_FILE = file.
	UINT TagID;		// the ID of current tag in database.
	UINT TagIdx;	// the index of current tag in CTaghelper::Tags
	UINT UseCount;	// the count of files associated with current tag.
	WCHAR wszDisplayName[MAX_PATH];	// file fullname or tag name.
	WCHAR wszRemark[MAX_PATH];		//
} MYPIDLDATA, *LPMYPIDLDATA;

// Class that creates/destroyes PIDLs and gets data from PIDLs.
class CPidlMgr  
{
public:
	CPidlMgr();
	~CPidlMgr();

	LPITEMIDLIST Create ( const MYPIDLDATA* );
	LPITEMIDLIST Copy ( LPCITEMIDLIST );
	void Delete ( LPITEMIDLIST );
	UINT GetSize ( LPCITEMIDLIST );

	LPITEMIDLIST GetNextItem ( LPCITEMIDLIST );
	LPITEMIDLIST GetLastItem ( LPCITEMIDLIST );

	MYPIDLDATA* GetData ( LPCITEMIDLIST );

private:
	IMalloc* m_spMalloc;
};
