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
			if( idCmd < this->TagHelper.TagCount)
			{
				LPWSTR formater = ::MyLoadString(IDS_COMMANDSTRING_TAG_FORMATER);
				wsprintf ( info,formater,this->TagHelper.Tags[idCmd].Tag );
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
	auto fTagManager = FormTagManager::instance();
	_ASSERT_EXPR(NULL != fTagManager,L"fTagManager could not be NULL.");
	return fTagManager->DlgProc(hwnd,uMsg,wParam,lParam);
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
		this->TagHelper.SetTag(cmd);
	}
	else
	{	
		HWND hdlg = NULL;
		//HWND parent = pCmdInfo->hwnd;
		HWND parent = GetDesktopWindow();
		BOOL createNew = true;

		// the caption of window, must be same as it when the window got msg WM_INITDIALOG.
		LPWSTR windowCaption = NULL;

		switch (cmd - this->TagHelper.TagCount)
		{
		case CMD_NEWTAG:
			{
				windowCaption = ::MyLoadString(IDS_DLG_TAGMANAGER_CAPTION);
				hdlg = FindWindowEx(parent, NULL, WINDOWCLASS_DLG, windowCaption);
				if ( NULL == hdlg ){
					hdlg = CreateDialogParam(g_hInst,MAKEINTRESOURCE(IDD_TAG_MANAGER),parent,DlgProc_TagManager,(LPARAM)this);
					createNew = true;
					::PrintLog(L"New window created: %s, handle = 0x%x",windowCaption, hdlg);
				}else
				{
					createNew = false;
					::PrintLog(L"Got existed window: %s, handle = 0x%x",windowCaption, hdlg);
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

		if(hdlg != NULL){
			ShowWindow(hdlg, SW_SHOW);
			UpdateWindow(hdlg);

			if( createNew ){
				SetWindowPos(hdlg,HWND_TOP,150,100,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

				// message queue.
				MSG msg;     
				while(GetMessage(&msg, NULL, 0, 0))     
				{     
					TranslateMessage(&msg);     
					DispatchMessage(&msg);     
				}
			}
			else
			{
				SetWindowPos(hdlg,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
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
