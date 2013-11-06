#pragma once
#include "../include/dllmain.h"
#include "../include/CHandler.h"
#include "../include/Form.TagManager.h"


HRESULT CHandler::QueryContextMenu (
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags )
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if ( uFlags & CMF_DEFAULTONLY || !_contextMenuSupposed)
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
	mii.hbmpItem = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDI_ICON));

	InsertMenu ( hmenu, uMenuIndex++ , MF_BYPOSITION | MF_SEPARATOR, _firstSpecialCmdID , L"MF_SEPARATOR" ); 
	uIdx++;

	InsertMenuItem  ( hmenu, uMenuIndex, true, &mii );

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, uIdx );
}

HRESULT CHandler::GetCommandString (UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
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
HRESULT CHandler::InvokeCommand (
	LPCMINVOKECOMMANDINFO pCmdInfo )
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if ( 0 != HIWORD( pCmdInfo->lpVerb ) )
		return E_INVALIDARG;

	// Get the command index.
	auto cmd =LOWORD( pCmdInfo->lpVerb );
	if( cmd < this->pTagHelper->TagCount)
	{
		UINT cmd_tagIdx = cmd;
		this->pTagHelper->SetTagByIdx(cmd_tagIdx);
	}
	else
	{	
		HWND parent = GetDesktopWindow();
		BOOL createNew = true;

		// the caption of window, must be same as it when the window got msg WM_INITDIALOG.
		wchar_t windowCaption[LOADSTRING_BUFFERSIZE] = {0};

		switch (cmd - this->pTagHelper->TagCount)
		{
		case CMD_NEWTAG:
			{
				auto tmp = ::MyLoadString(IDS_DLG_TAGMANAGER_CAPTION);
				memcpy(windowCaption,tmp,sizeof(windowCaption));
				_hdlg = FindWindowEx(parent, NULL, WINDOWCLASS_DLG, windowCaption);
				if ( NULL == _hdlg ){
					_hdlg = CreateDialogParam(g_hInst,MAKEINTRESOURCE(IDD_TAG_MANAGER),parent,DlgProc_TagManager,(LPARAM)this);
					createNew = true;
					::PrintLog(L"New window created: %s, handle = 0x%x",windowCaption, _hdlg);
				}else
				{
					createNew = false;
					::PrintLog(L"Got existed window: %s, handle = 0x%x",windowCaption, _hdlg);
				}
			}
			break;
		case CMD_SETTINGS:
			_hdlg = CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_PROPERTYPAGE),pCmdInfo->hwnd,NULL);
			break;
		case CMD_ABOUT:
			_hdlg = CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_ABOUT),pCmdInfo->hwnd,DlgProc_About);
			break;
		default:
			{
				return E_FAIL;
			}
			break;
		}

		if(_hdlg != NULL){
			SetWindowText(_hdlg,windowCaption);
			ShowWindow(_hdlg, SW_SHOW);
			UpdateWindow(_hdlg);

			if( !createNew )
			{
				SetWindowPos(_hdlg,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			}
		}else{
			DWORD e = GetLastError();
			::PrintLog(L"Can not create window. cmd = %d, error = %d",cmd - this->pTagHelper->TagCount,e);
			MessageBox(pCmdInfo->hwnd,L"Can not create window",L"Error",MB_OK);
			return E_FAIL;
		}
	}

	return S_OK;
}

LRESULT CALLBACK DlgProc_TagManager(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam){
	FormTagManager *pebhd = reinterpret_cast<FormTagManager *>(GetWindowLongPtr(hwnd, DWLP_USER));
	if (uMsg == WM_INITDIALOG)
	{
		pebhd = FormTagManager::instance();
		SetWindowLongPtr(hwnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pebhd));
	}
	return pebhd ? pebhd->DlgProc(hwnd, uMsg, wParam, lParam) : 0;
}

LRESULT CALLBACK DlgProc_About(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam){
	switch(uMsg)     
	{   
	case WM_INITDIALOG:
		{
			SetWindowText(hwnd,::MyLoadString(IDS_DLG_ABOUT_BU_CLOSE));
			SetDlgItemText(hwnd, IDC_ABOUT_BU_CLOSE,::MyLoadString(IDS_DLG_ABOUT_BU_CLOSE));

			wchar_t info[LOADSTRING_BUFFERSIZE] = {0};
			StrCat(info,::MyLoadString(IDS_ProductName));StrCat(info,L"\r\n");
			StrCat(info,::MyLoadString(IDS_ProductIntro));StrCat(info,L"\r\n");
			StrCat(info,L"Author: ");
			StrCat(info,::MyLoadString(IDS_ProductAuthor));StrCat(info,L"\r\n");
			StrCat(info,L"Contact: ");
			StrCat(info,::MyLoadString(IDS_ProductAuthorMail));StrCat(info,L"\r\n");
			StrCat(info,L"Homepage: ");
			StrCat(info,::MyLoadString(IDS_ProductHomepage));StrCat(info,L"\r\n");
			SetDlgItemText(hwnd, IDC_ABOUT_INFO,info);

			SendDlgItemMessage(hwnd, IDC_ABOUT_ICON, STM_SETIMAGE,IMAGE_ICON,(WPARAM)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON)));

			break;
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))     
			{    
			case IDC_ABOUT_BU_CLOSE:
				DestroyWindow(hwnd);
				break;
			default:
				break;
			}
		}
		break;
	case WM_DESTROY:     
		{
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}

	return (INT_PTR)FALSE;
}