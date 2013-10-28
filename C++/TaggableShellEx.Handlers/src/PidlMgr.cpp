#pragma once
#include "../include/PidlMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CPidlMgr::CPidlMgr()
{
	HRESULT hr = SHGetMalloc ( &m_spMalloc );
}

CPidlMgr::~CPidlMgr()
{
}


/*---------------------------------------------------------------*/
// CPidlMgr::Create( HWND )
/*---------------------------------------------------------------*/
LPITEMIDLIST CPidlMgr::Create ( const MYPIDLDATA* data )
{
	// Global size of the PIDL, including SHITEMID
	UINT uSize = sizeof(ITEMIDLIST) + sizeof(MYPIDLDATA);

	// Also allocate memory for the final null SHITEMID.
	LPITEMIDLIST pidlNew = (LPITEMIDLIST) m_spMalloc->Alloc(uSize + sizeof(ITEMIDLIST));

	if ( pidlNew )
	{
		LPITEMIDLIST pidlTemp = pidlNew;

		// Prepares the PIDL to be filled with actual data
		pidlTemp->mkid.cb = uSize;
		MYPIDLDATA* pData = (MYPIDLDATA*) pidlTemp->mkid.abID;

		// Fill the PIDL
		pData=(MYPIDLDATA*)data;

		// A PIDL of size 0 means the end of the chain
		pidlTemp = GetNextItem ( pidlTemp );
		pidlTemp->mkid.cb = 0;
		pidlTemp->mkid.abID[0] = 0;
	}

	return pidlNew;
}


/*---------------------------------------------------------------*/
// CPidlMgr::Delete( LPITEMIDLIST )
// Delete a PIDL
/*---------------------------------------------------------------*/
void CPidlMgr::Delete(LPITEMIDLIST pidl)
{
	m_spMalloc->Free(pidl);
}


/*---------------------------------------------------------------*/
// CPidlMgr::GetNextItem( LPCITEMIDLIST )
// Retrieves the next element in a ITEMIDLIST
/*---------------------------------------------------------------*/
LPITEMIDLIST CPidlMgr::GetNextItem ( LPCITEMIDLIST pidl )
{
	_ASSERT(pidl != NULL);

	return LPITEMIDLIST(LPBYTE(pidl) + pidl->mkid.cb);
}


/*---------------------------------------------------------------*/
// CPidlMgr::GetLastItem( LPCITEMIDLIST )
// Retrieves the last element in a ITEMIDLIST
/*---------------------------------------------------------------*/
LPITEMIDLIST CPidlMgr::GetLastItem ( LPCITEMIDLIST pidl )
{
	return ILFindLastID(pidl);
/*
	LPITEMIDLIST pidlLast = NULL;

	_ASSERT(pidl != NULL);

	while ( 0 != pidl->mkid.cb )
	{
		pidlLast = (LPITEMIDLIST) pidl;
		pidl = GetNextItem ( pidl );
	}

	return pidlLast;*/
}


/*---------------------------------------------------------------*/
// CPidlMgr::GetSize( LPITEMIDLIST )
// Retrieves the size of item list 
/*---------------------------------------------------------------*/
UINT CPidlMgr::GetSize ( LPCITEMIDLIST pidl )
{
	UINT uSize = 0;
	LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;

	_ASSERT(pidl != NULL);

	while ( 0 != pidlTemp->mkid.cb )
	{
		uSize += pidlTemp->mkid.cb;
		pidlTemp = GetNextItem ( pidlTemp );
	}  

	// add the size of the NULL terminating ITEMIDLIST
	uSize += sizeof(ITEMIDLIST);

	return uSize;
}


/*---------------------------------------------------------------*/
// CPidlMgr::GetData(LPCITEMIDLIST)
// Retrieves the drive letter of a given PIDL (last element)
/*---------------------------------------------------------------*/
MYPIDLDATA* CPidlMgr::GetData ( LPCITEMIDLIST pidl )
{
	LPITEMIDLIST pLast;
	MYPIDLDATA* pData;

	if ( NULL == pidl )
		return NULL;

	// Get the last item of the PIDL to make sure we get the right TCHAR
	// in case of multiple nesting levels
	pLast = GetLastItem ( pidl );

	// we use MYPIDLDATA as SHITEMID in LPCITEMIDLIST.
	pData = (MYPIDLDATA*)pLast;
	return pData;
}


/*---------------------------------------------------------------*/
// CPidlMgr::Copy( LPITEMIDLIST )
// Duplicates a PIDL
/*---------------------------------------------------------------*/
LPITEMIDLIST CPidlMgr::Copy ( LPCITEMIDLIST pidlSrc )
{
	LPITEMIDLIST pidlTarget = NULL;
	UINT cbSrc = 0;

	if ( NULL == pidlSrc )
		return NULL;

	// Allocate memory for the new PIDL.

	cbSrc = GetSize ( pidlSrc );
	pidlTarget = (LPITEMIDLIST) m_spMalloc->Alloc ( cbSrc );

	if ( NULL == pidlTarget )
		return NULL;

	// Copy the source PIDL to the target PIDL.

	CopyMemory ( pidlTarget, pidlSrc, cbSrc );

	return pidlTarget;
}

///*---------------------------------------------------------------*/
//// CPidlMgr::GetPidlDescription ( LPITEMIDLIST, LPTSTR)
//// Sets the text to be returned to identify the item
///*---------------------------------------------------------------*/
//void CPidlMgr::GetPidlDescription ( LPCITEMIDLIST pidl, LPTSTR szDesc )
//{
//	LPWSTR chDrive = GetData ( pidl );
//
//	if ( '\0' != chDrive )
//		wsprintf ( szDesc, L"Drive %s:", chDrive );
//	else
//		*szDesc = '\0';
//}
