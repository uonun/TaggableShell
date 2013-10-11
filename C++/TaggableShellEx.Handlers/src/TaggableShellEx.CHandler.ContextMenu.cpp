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
	_hSubmenu = CreatePopupMenu();
	UINT uIdx = 0;

	//MENUITEMINFO mm = { sizeof(MENUITEMINFO) };
	//mm.fMask = MIIM_DATA | MIIM_STRING | MIIM_ID;
	//mm.wID = CMD_NEWTAG;
	//mm.dwItemData = CMD_NEWTAG;
	//mm.dwTypeData = L"AAAAAAAAAAAA";
	//InsertMenuItem  ( _hSubmenu, CMD_NEWTAG,false,&mm );


	//_tagsOffsetInMenu = uID - uidFirstCmd;

	for (unsigned int i = 0; i < _tagHelper.TagCount; i++)
	{
		if(!InsertMenu (
			_hSubmenu, uIdx++, 
			_tagHelper.Tags[i].bAsso ? MF_BYPOSITION | MF_CHECKED : MF_BYPOSITION,
			uidFirstCmd + i, _tagHelper.Tags[i].Tag ))
		{
			::PrintLog(L"Fail to add tag into the submenu: %s",_tagHelper.Tags[i]);
		}
	}

	UINT _firstSpecialCmdID = uidFirstCmd + _tagHelper.TagCount;

	// MF_BYCOMMAND  MF_BYPOSITION
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION | MF_SEPARATOR, _firstSpecialCmdID , L"MF_SEPARATOR" );
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_NEWTAG,::MyLoadString(IDS_CONTEXTMENU_SUB_NEWTAG));
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_SETTINGS,::MyLoadString(IDS_CONTEXTMENU_SUB_SETTINGS));
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_ABOUT,::MyLoadString(IDS_CONTEXTMENU_SUB_ABOUT));

	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms647578(v=vs.85).aspx
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = uIdx++;
	mii.hSubMenu = _hSubmenu;
	mii.dwTypeData = ::MyLoadString(IDS_CONTEXTMENU_MAIN_TEXT);

	InsertMenuItem  ( hmenu, uMenuIndex, true,&mii );

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, uIdx );
}

HRESULT CHandler::GetCommandString (UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if ( uFlags & GCS_HELPTEXT )
	{
		LPWSTR info;
		UINT size = LOADSTRING_BUFFERSIZE * sizeof(wchar_t);
		info = (wchar_t*)malloc(size);
		memset(info,0,size);

		if( _hSubmenu != NULL ){
			if( idCmd < _tagHelper.TagCount)
			{
				LPWSTR formater = ::MyLoadString(IDS_COMMANDSTRING_TAG_FORMATER);
				wsprintf ( info,formater,_tagHelper.Tags[idCmd].Tag );
			}
			else
			{	
				switch (idCmd - _tagHelper.TagCount)
				{
				case CMD_NEWTAG:
					info = ::MyLoadString(IDS_COMMANDSTRING_CMD_NEWTAG);
					break;
				case CMD_SETTINGS:
					info = ::MyLoadString(IDS_COMMANDSTRING_CMD_SETTINGS);
					break;
				case CMD_ABOUT:
					info = ::MyLoadString(IDS_COMMANDSTRING_CMD_ABOUT);
					break;
				default:
					{
						return E_INVALIDARG;
					}
					break;
				}
			}
		}
		else 
		{
			info = ::MyLoadString(IDS_COMMANDSTRING_MAIN);
		}

		lstrcpynW ( (LPWSTR) pszName,info, cchMax );
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

	// Get the command index.
	auto cmd =LOWORD( pCmdInfo->lpVerb );
	if( cmd < _tagHelper.TagCount)
	{
		_tagHelper.SetTag(cmd);
	}
	else
	{	
		switch (cmd - _tagHelper.TagCount)
		{
		case CMD_NEWTAG:
			MessageBox ( pCmdInfo->hwnd, L"CMD_NEWTAG", L"SimpleShlExt", MB_ICONINFORMATION );
			break;
		case CMD_SETTINGS:
			MessageBox ( pCmdInfo->hwnd, L"CMD_SETTINGS", L"SimpleShlExt", MB_ICONINFORMATION );
			break;
		case CMD_ABOUT:
			MessageBox ( pCmdInfo->hwnd, L"CMD_ABOUT", L"SimpleShlExt", MB_ICONINFORMATION );
			break;
		default:
			{

			}
			break;
		}

		return E_FAIL;
	}


	return S_OK;
}
