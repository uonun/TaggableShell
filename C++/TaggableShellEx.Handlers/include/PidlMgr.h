#pragma once
#include "../include/stdafx.h"

// Data structure to be stored in our PIDLs.
typedef struct tagMYPIDLDATA {
  USHORT cb;	// MAKE SURE cb = sizeof(MYPIDLDATA)
  UINT TagIdx;
  UINT UseCount;
  WCHAR wszDisplayName[MAXLENGTH_EACHTAG];
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
