#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <PrSht.h>
#include "TaggableShellEx.ContextMenuHandler.h"
#include "dllmain.h"
#include "resource.h"
#include "Taghelper.h"

ContextMenuHandler::ContextMenuHandler() : _cRef(1),m_pIDFolder(NULL),m_pDataObj(NULL),m_hRegKey(NULL)
{
	*m_szFile = NULL;

	DllAddRef();
#ifdef LOG4CPP
	Utils::PrintLog(L"ContextMenuHandler.ctor");
#endif
}

ContextMenuHandler::~ContextMenuHandler(void)
{   
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
	if(m_pIDFolder!=NULL){
		ILFree(m_pIDFolder);
		m_pIDFolder = NULL;
	}

	// If Initialize has already been called, release the old
	// IDataObject pointer.
	if (m_pDataObj)
	{ 
		m_pDataObj->Release(); 
	}

	//Store the new PIDL.
	if(pIDFolder)
	{
		m_pIDFolder = ILClone(pIDFolder);
	}

	// If a data object pointer was passed in, save it and
	// extract the file name. 
	if (pDataObj) 
	{ 
		m_pDataObj = pDataObj; 
		pDataObj->AddRef(); 
	}
	return S_OK; 
}


HRESULT ContextMenuHandler::QueryContextMenu (
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags )
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if ( uFlags & CMF_DEFAULTONLY )
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

	// create and populate a submenu.
	HMENU hSubmenu = CreatePopupMenu();
	UINT uID = uidFirstCmd;
	UINT uPosition = 0;

	LPWSTR tags[MAXCOUNT_TAG];
	int count = 0;
	CTaghelper::GetTags(tags,&count);
	for (int i = 0; i < count; i++)
	{
		InsertMenu ( hSubmenu, uPosition++, MF_BYPOSITION, uID++, tags[i] );
	}

	InsertMenu ( hSubmenu, uPosition++, MF_SEPARATOR, uID++, L"MF_SEPARATOR" );
	InsertMenu ( hSubmenu, uPosition++, MF_BYPOSITION, uID++,::MyLoadString(IDS_CONTEXTMENU_SUB_NEWTAG));
	InsertMenu ( hSubmenu, uPosition++, MF_BYPOSITION, uID++,::MyLoadString(IDS_CONTEXTMENU_SUB_SETTINGS));

	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = uID++;
	mii.hSubMenu = hSubmenu;
	mii.dwTypeData = ::MyLoadString(IDS_CONTEXTMENU_MAIN_TEXT);

	InsertMenuItem  ( hmenu, uMenuIndex, true,&mii );

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL,uID - uidFirstCmd );
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
		{
			TCHAR szMsg[MAX_PATH + 32];

			wsprintf ( szMsg, L"pCmdInfo->lpVerb: %d", LOWORD( pCmdInfo->lpVerb ) );

			MessageBox ( pCmdInfo->hwnd, szMsg, L"SimpleShlExt", MB_ICONINFORMATION );
			return E_INVALIDARG;
		}
		break;
	}
}
