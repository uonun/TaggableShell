#pragma once
#include "../include/NsExt.CShellFolderImpl.h"


HRESULT CShellFolderImpl::QueryContextMenu (
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags )
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if ( uFlags & CMF_DEFAULTONLY )
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

	if ( NULL != _hSubmenu )
	{
		DestroyMenu ( _hSubmenu );
		_hSubmenu = NULL;
	}

	// create and populate a submenu.
	_hSubmenu = CreatePopupMenu();
	UINT uIdx = 0;

	for (unsigned int i = 0; i < this->pTagHelper->TagCount; i++)
	{
		if(AppendMenu (
			_hSubmenu,  
			this->pTagHelper->Tags[i].bAsso ? MF_BYPOSITION | MF_CHECKED : MF_BYPOSITION,
			uidFirstCmd + i, this->pTagHelper->Tags[i].Tag ))
		{
			uIdx++;
		}else{
			::PrintLog(L"Fail to add tag into the submenu: %s",this->pTagHelper->Tags[i]);
		}
	}

	UINT _firstSpecialCmdID = uidFirstCmd + this->pTagHelper->TagCount;

	// MF_BYCOMMAND  MF_BYPOSITION
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION | MF_SEPARATOR, _firstSpecialCmdID , L"MF_SEPARATOR" );
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_NEWTAG,::MyLoadString(IDS_CONTEXTMENU_SUB_NEWTAG));
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_SETTINGS,::MyLoadString(IDS_CONTEXTMENU_SUB_SETTINGS));
	InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_ABOUT,::MyLoadString(IDS_CONTEXTMENU_SUB_ABOUT));

	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms647578(v=vs.85).aspx
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID | MIIM_BITMAP;
	mii.wID = uIdx++;
	mii.hSubMenu = _hSubmenu;
	mii.dwTypeData = ::MyLoadString(IDS_CONTEXTMENU_MAIN_TEXT);

	// TODO: take no effect, may be in a wrong way.
	//mii.hbmpItem = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PNG_16));

	InsertMenuItem  ( hmenu, uMenuIndex, true,&mii );

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, uIdx );
}

HRESULT CShellFolderImpl::GetCommandString (UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if ( uFlags & GCS_HELPTEXT )
	{
		LPWSTR info = NULL;
		if( _hSubmenu != NULL ){
			if( idCmd < this->pTagHelper->TagCount)
			{
				LPWSTR formater = ::MyLoadString(IDS_COMMANDSTRING_TAG);
				wchar_t tmp[LOADSTRING_BUFFERSIZE] = {0};
				wsprintf ( tmp,formater,this->pTagHelper->Tags[idCmd].Tag );
				info = tmp;
			}
			else
			{	
				switch (idCmd - this->pTagHelper->TagCount)
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
HRESULT CShellFolderImpl::InvokeCommand (
	LPCMINVOKECOMMANDINFO pCmdInfo )
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if ( 0 != HIWORD( pCmdInfo->lpVerb ) )
		return E_INVALIDARG;

	return S_OK;
}
