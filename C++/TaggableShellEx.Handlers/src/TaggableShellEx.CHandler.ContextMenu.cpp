#pragma once
#include "../include/dllmain.h"
#include "../include/TaggableShellEx.Taghelper.h"
#include "../include/TaggableShellEx.CHandler.h"


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
	UINT uPosition = 0x1000;

	for (int i = 0; i < _tagHelper.TagCount; i++)
	{
		if(!InsertMenu ( hSubmenu, uPosition|i, 
			_tagHelper.Tags[i].bAsso ? MF_BYCOMMAND | MF_CHECKED : MF_BYCOMMAND,
			uID++, _tagHelper.Tags[i].Tag ))
		{
			::PrintLog(L"Fail to add tag into the submenu: %s",_tagHelper.Tags[i]);
		}
	}

	InsertMenu ( hSubmenu, -1, MF_BYPOSITION | MF_SEPARATOR, uID++, L"MF_SEPARATOR" );
	InsertMenu ( hSubmenu, CMD_NEWTAG, MF_BYCOMMAND, uID++,::MyLoadString(IDS_CONTEXTMENU_SUB_NEWTAG));
	InsertMenu ( hSubmenu, CMD_SETTINGS, MF_BYCOMMAND, uID++,::MyLoadString(IDS_CONTEXTMENU_SUB_SETTINGS));

	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms647578(v=vs.85).aspx
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

// http://msdn.microsoft.com/en-us/library/windows/desktop/bb776096(v=vs.85).aspx
HRESULT CHandler::InvokeCommand (
	LPCMINVOKECOMMANDINFO pCmdInfo )
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if ( 0 != HIWORD( pCmdInfo->lpVerb ) )
		return E_INVALIDARG;

	// Get the command index - the only valid one is 0.
	auto cmd =LOWORD( pCmdInfo->lpVerb );
	switch (cmd)
	{
	case 0:
		{
			MessageBox ( pCmdInfo->hwnd, L"SDFSDFSDF", L"SimpleShlExt", MB_ICONINFORMATION );

			return S_OK;
		}
		break;

	default:
		{
			switch (cmd)
			{
			case CMD_NEWTAG:
				MessageBox ( pCmdInfo->hwnd, L"CMD_NEWTAG", L"SimpleShlExt", MB_ICONINFORMATION );
				break;
			case CMD_SETTINGS:
				MessageBox ( pCmdInfo->hwnd, L"CMD_SETTINGS", L"SimpleShlExt", MB_ICONINFORMATION );
				break;
			default:
				{
					if(cmd < _tagHelper.TagCount)
					{
						_tagHelper.SetTag(cmd);
					}
				}
				break;
			}

			TCHAR szMsg[MAX_PATH + 32];
			wsprintf ( szMsg, L"pCmdInfo->lpVerb: %d", cmd );
			MessageBox ( pCmdInfo->hwnd, szMsg, L"SimpleShlExt", MB_ICONINFORMATION );

			return E_INVALIDARG;
		}
		break;
	}
}
