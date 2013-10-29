#pragma once
#include "../include/dllmain.h"
#include "../include/TaggableShellEx.CHandler.h"
#include "../include/TaggableShellEx.Form.TagManager.h"


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

	for (unsigned int i = 0; i < this->TagHelper.TagCount; i++)
	{
		if(AppendMenu (
			_hSubmenu,  
			this->TagHelper.Tags[i].bAsso ? MF_BYPOSITION | MF_CHECKED : MF_BYPOSITION,
			uidFirstCmd + i, this->TagHelper.Tags[i].Tag ))
		{
			uIdx++;
		}else{
			::PrintLog(L"Fail to add tag into the submenu: %s",this->TagHelper.Tags[i]);
		}
	}

	UINT _firstSpecialCmdID = uidFirstCmd + this->TagHelper.TagCount;

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
	mii.hbmpItem = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PNG_16));

	InsertMenuItem  ( hmenu, uMenuIndex, true,&mii );

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
			if( idCmd < this->TagHelper.TagCount)
			{
				LPWSTR formater = ::MyLoadString(IDS_COMMANDSTRING_TAG);
				wchar_t tmp[LOADSTRING_BUFFERSIZE] = {0};
				wsprintf ( tmp,formater,this->TagHelper.Tags[idCmd].Tag );
				info = tmp;
			}
			else
			{	
				switch (idCmd - this->TagHelper.TagCount)
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

LRESULT CALLBACK DlgProc_TagManager(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam){
	FormTagManager *pebhd = reinterpret_cast<FormTagManager *>(GetWindowLongPtr(hwnd, DWLP_USER));
	if (uMsg == WM_INITDIALOG)
	{
		pebhd = FormTagManager::instance();
		SetWindowLongPtr(hwnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pebhd));
	}
	return pebhd ? pebhd->DlgProc(hwnd, uMsg, wParam, lParam) : 0;
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
	if( cmd < this->TagHelper.TagCount)
	{
		UINT cmd_tagIdx = cmd;
		this->TagHelper.SetTagByIdx(cmd_tagIdx);
	}
	else
	{	
		HWND parent = GetDesktopWindow();
		BOOL createNew = true;

		// the caption of window, must be same as it when the window got msg WM_INITDIALOG.
		wchar_t windowCaption[LOADSTRING_BUFFERSIZE] = {0};

		switch (cmd - this->TagHelper.TagCount)
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
			MessageBox ( pCmdInfo->hwnd, L"CMD_SETTINGS", L"SimpleShlExt", MB_ICONINFORMATION );
			break;
		case CMD_ABOUT:
			MessageBox ( pCmdInfo->hwnd, L"CMD_ABOUT", L"SimpleShlExt", MB_ICONINFORMATION );
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

			if( createNew )
			{
				//// message queue.
				//MSG msg;
				//BOOL bRet;
				//while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
				//{
				//	if (bRet == -1)
				//	{
				//		DWORD e = GetLastError();
				//		::PrintLog(L"Got error = %d",e);
				//		MessageBox(pCmdInfo->hwnd,L"Got error",L"Error",MB_OK);
				//		return E_FAIL;
				//	}
				//	else
				//	{
				//		TranslateMessage(&msg);
				//		if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP ) {
				//			SendMessage (_hdlg, msg.message, msg.wParam, msg.lParam);
				//		}
				//		DispatchMessage(&msg);     
				//	}
				//}
			}
			else
			{
				SetWindowPos(_hdlg,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			}
		}else{
			DWORD e = GetLastError();
			::PrintLog(L"Can not create window. cmd = %d, error = %d",cmd - this->TagHelper.TagCount,e);
			MessageBox(pCmdInfo->hwnd,L"Can not create window",L"Error",MB_OK);
			return E_FAIL;
		}
	}

	return S_OK;
}
