#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

// IContextMenu
HRESULT CShellFolderImpl::QueryContextMenu (
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags )
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if ( uFlags & CMF_DEFAULTONLY )
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

	HRESULT hr = S_FALSE;
	BOOL x = FALSE;

	if ( NULL != _hSubmenu )
	{
		x = DestroyMenu ( _hSubmenu );
		_hSubmenu = NULL;
	}

	// create and populate a submenu.
	_hSubmenu = CreatePopupMenu();
	if( _hSubmenu )
	{
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
		x = InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION | MF_SEPARATOR, _firstSpecialCmdID , L"MF_SEPARATOR" );
		x = InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_NEWTAG,::MyLoadString(IDS_CONTEXTMENU_SUB_NEWTAG));
		x = InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_SETTINGS,::MyLoadString(IDS_CONTEXTMENU_SUB_SETTINGS));
		x = InsertMenu ( _hSubmenu, uIdx++ , MF_BYPOSITION, _firstSpecialCmdID + CMD_ABOUT,::MyLoadString(IDS_CONTEXTMENU_SUB_ABOUT));

		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms647578(v=vs.85).aspx
		MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
		mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID | MIIM_BITMAP;
		mii.wID = uIdx++;
		mii.hSubMenu = _hSubmenu;
		mii.dwTypeData = ::MyLoadString(IDS_CONTEXTMENU_MAIN_TEXT);

		// TODO: take no effect, may be in a wrong way.
		//mii.hbmpItem = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PNG_16));

		x = InsertMenuItem  ( hmenu, uMenuIndex, true,&mii );

		hr = MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, USHORT(uIdx) );
	}
	return hr;
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


STDMETHODIMP CShellFolderImpl::CallBack(__in_opt IShellFolder *psf, HWND hwndOwner, __in_opt IDataObject *pdo, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_NOTIMPL;
	::PrintLog(L"CShellFolderImpl::CallBack: uiMsg=%ld wp=0x%X lp=0x%X\n", uiMsg, wParam, lParam);
	switch (uiMsg)
	{
	case DFM_MERGECONTEXTMENU:
		{
			BOOL x;
			QCMINFO* pqcmi = reinterpret_cast<QCMINFO*>(lParam);

			//_hSubmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU_Folder));
			_hSubmenu = CreatePopupMenu();

			if ( _hSubmenu )
			{
				// MF_BYCOMMAND  MF_BYPOSITION
				x = AppendMenu ( _hSubmenu, MF_BYCOMMAND, CMD_NEWTAG , L"CMD_NEWTAG");
				x = AppendMenu ( _hSubmenu, MF_BYCOMMAND, CMD_SETTINGS , L"CMD_SETTINGS");
				x = AppendMenu ( _hSubmenu, MF_BYCOMMAND, CMD_ABOUT , L"CMD_ABOUT");

				UINT uiMax = Shell_MergeMenus(pqcmi->hmenu, _hSubmenu, pqcmi->indexMenu, pqcmi->idCmdFirst, 0xFFFF,
					MM_ADDSEPARATOR | MM_SUBMENUSHAVEIDS | MM_DONTREMOVESEPS);
				x = DestroyMenu(_hSubmenu);
				pqcmi->idCmdFirst = uiMax;
				if (!(wParam & CMF_NODEFAULT))
				{
					x = SetMenuDefaultItem(pqcmi->hmenu, CMD_NEWTAG, MF_BYCOMMAND); //uiCmdIdFirst + 1
					::PrintLog(L"CShellFolderImpl::CallBack: SetMenuDefaultItem = %d",x);
				}
			}
			hr = S_OK;
		}
		break;

	case DFM_INVOKECOMMANDEX:
		{
			//PDFMICS pqcmi = reinterpret_cast<PDFMICS>(lParam);
			switch (wParam)
			{
			case CMD_NEWTAG:
				{
					MessageBox(hwndOwner,L"CMD_NEWTAG",L"NotImplemented",MB_OK);
				}
				break;
			case CMD_SETTINGS:
				{
					MessageBox(hwndOwner,L"CMD_NEWTAG",L"NotImplemented",MB_OK);
				}
				break;
			case CMD_ABOUT:
				{
					MessageBox(hwndOwner,L"CMD_ABOUT",L"NotImplemented",MB_OK);
				}
				break;

			default:
				hr = S_FALSE;   // do the default "Create Shortcut" for example
				break;
			}
			hr = S_OK;
		}
		break;

	case DFM_MAPCOMMANDNAME:
		hr = S_OK;
		break;
	case DFM_GETVERBW:
	case DFM_GETVERBA:
		{
			/*
			idCmd_cchMax [in]
			The low-order word of this parameter holds the command ID. The high-order word holds the number of characters in the pszText buffer.
			pszText [out]
			A pointer to a null-terminated string that contains the verb text.
			http://msdn.microsoft.com/en-us/library/bb787996(v=vs.85).aspx
			*/
			if( uiMsg == DFM_GETVERBA ) 
				hr = strncpy(reinterpret_cast<LPSTR>(lParam), "DFM_GETVERBA", HIWORD(wParam)) > 0 ? S_OK : E_FAIL;
			else
				hr = wcsncpy(reinterpret_cast<LPWSTR>(lParam), L"DFM_GETVERBW", HIWORD(wParam)) > 0 ? S_OK : E_FAIL;
		}
		break;
	case DFM_GETHELPTEXTW:
		break;
	case DFM_GETHELPTEXT:
		break;
	case DFM_GETDEFSTATICID: // Required for Windows 7 to pick a default
		hr = S_OK; 
		break;
	case DFM_VALIDATECMD:
		hr = S_OK;
		//hr = _HandleStandardMenuMessage(uiMsg, wParam, lParam, s_rgMenuMap, ARRAYSIZE(s_rgMenuMap));
		break;
	case DFM_MERGECONTEXTMENU_TOP:		
	case DFM_MERGECONTEXTMENU_BOTTOM:
		{
			UINT uFlags  = (UINT)wParam;			
			QCMINFO* pqcmi = reinterpret_cast<QCMINFO*>(lParam);
			hr = S_OK;
		}
		break;
	case DFM_MODIFYQCMFLAGS:
		hr = S_OK;
		break;
	}

	return hr;
}
