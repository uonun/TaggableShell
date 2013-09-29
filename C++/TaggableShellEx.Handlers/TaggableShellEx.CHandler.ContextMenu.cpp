#pragma once
#include "dllmain.h"
#include "Taghelper.h"
#include "TaggableShellEx.CHandler.h"


HRESULT CHandler::QueryContextMenu (
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
		InsertMenu ( hSubmenu, uPosition++, MF_BYPOSITION | MF_CHECKED, uID++, tags[i] );
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

HRESULT CHandler::GetCommandString (UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
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

HRESULT CHandler::InvokeCommand (
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
			MessageBox ( pCmdInfo->hwnd, L"SDFSDFSDF", L"SimpleShlExt", MB_ICONINFORMATION );

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
