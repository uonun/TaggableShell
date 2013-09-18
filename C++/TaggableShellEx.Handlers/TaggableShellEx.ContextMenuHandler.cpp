#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <PrSht.h>
#include "TaggableShellEx.ContextMenuHandler.h"
#include "dllmain.h"
#include "resource.h"

ContextMenuHandler::ContextMenuHandler() : _cRef(1),_pStream(NULL)
{
	DllAddRef();
#ifdef LOG4CPP
	Utils::PrintLog(L"ContextMenuHandler.ctor");
#endif
}

ContextMenuHandler::~ContextMenuHandler(void){   
#ifdef LOG4CPP
	Utils::PrintLog(L"ContextMenuHandler.~ctor");
#endif
	DllRelease();
}

HRESULT ContextMenuHandler_CreateInstance(REFIID riid, void **ppv)
{
#ifdef LOG4CPP
	Utils::PrintLog(L"ContextMenuHandler.ContextMenuHandler_CreateInstance");
#endif
	ContextMenuHandler *pNew = new(std::nothrow) ContextMenuHandler;
	HRESULT hr = pNew ? S_OK : E_OUTOFMEMORY;
	if (pNew)
	{
		hr = pNew->QueryInterface(riid, ppv);
		pNew->Release();
	}
	return hr;
}

// IUnknown
IFACEMETHODIMP ContextMenuHandler::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(ContextMenuHandler, IShellExtInit),
		QITABENT(ContextMenuHandler, IContextMenu),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) ContextMenuHandler::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) ContextMenuHandler::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

HRESULT ContextMenuHandler::Initialize(LPCITEMIDLIST pIDFolder, 
                                   IDataObject *pDataObj, 
                                   HKEY hRegKey) 
{ 
    // If Initialize has already been called, release the old PIDL
    ILFree(m_pIDFolder);
    m_pIDFolder = NULL;

    //Store the new PIDL.
    if(pIDFolder)
    {
        m_pIDFolder = ILClone(pIDFolder);
    }
    
    // If Initialize has already been called, release the old
    // IDataObject pointer.
    if (m_pDataObj)
    { 
        m_pDataObj->Release(); 
    }
     
    // If a data object pointer was passed in, save it and
    // extract the file name. 
    if (pDataObj) 
    { 
        m_pDataObj = pDataObj; 
        pDataObj->AddRef(); 
      
        STGMEDIUM   medium;
        FORMATETC   fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        UINT        uCount;

        if(SUCCEEDED(m_pDataObj->GetData(&fe, &medium)))
        {
            // Get the count of files dropped.
            uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

            // Get the first file name from the CF_HDROP.
            if(uCount)
                DragQueryFile((HDROP)medium.hGlobal, 0, m_szFile, 
                              sizeof(m_szFile)/sizeof(TCHAR));

            ReleaseStgMedium(&medium);
        }
    }

    // Duplicate the registry handle. 
    if (hRegKey) 
        RegOpenKeyEx(hRegKey, 
                     NULL, 
                     0L, 
                     MAXIMUM_ALLOWED, 
                     &m_hRegKey); 
    return S_OK; 
}


HRESULT ContextMenuHandler::QueryContextMenu (
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags )
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if ( uFlags & CMF_DEFAULTONLY )
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

	InsertMenu ( hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd,L"SimpleShlExt Test Item" );

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1 );
}

HRESULT ContextMenuHandler::GetCommandString (UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	// Check idCmd, it must be 0 since we have only one menu item.
	if ( 0 != idCmd )
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if ( uFlags & GCS_HELPTEXT )
	{
		LPCWSTR szText = L"This is the simple shell extension's help";

		lstrcpynW ( (LPWSTR) pszName, szText, cchMax );

		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT ContextMenuHandler::InvokeCommand (
  LPCMINVOKECOMMANDINFO pCmdInfo )
{
  // If lpVerb really points to a string, ignore this function call and bail out.
  if ( 0 != HIWORD( pCmdInfo->lpVerb ) )
    return E_INVALIDARG;
 
  // Get the command index - the only valid one is 0.
  switch ( LOWORD( pCmdInfo->lpVerb ) )
    {
    case 0:
      {
      TCHAR szMsg[MAX_PATH + 32];
 
      wsprintf ( szMsg, L"The selected file was:\n\n%s", m_szFile );
 
      MessageBox ( pCmdInfo->hwnd, szMsg, L"SimpleShlExt", MB_ICONINFORMATION );
 
      return S_OK;
      }
    break;
 
    default:
      return E_INVALIDARG;
    break;
    }
}
