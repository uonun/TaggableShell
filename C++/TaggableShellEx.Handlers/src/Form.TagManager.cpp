#pragma once
#include "../include/Form.TagManager.h"

FormTagManager* FormTagManager::p_instance_ = NULL;
IImageList* FormTagManager::_sysImgList = NULL;
HBRUSH FormTagManager::_hbrBkgnd = NULL;	// for colored message info

FormTagManager::FormTagManager(void):
	_cRef(1)
	,_handler(0)
	,_hwnd(0)
	,_hErrorInfo(0),_hListTags(0),_hListFiles(0),_hEditCtlNewTag(0),_hCheckAttachToFiles(0),_hBuAdd(0),_hBuEdit(0),_hBuDel(0)
	,_msgColor(COLOR_MY_DEFAULT)
{
	::PrintLog(L"FormTagManager.ctor.");

	DllAddRef();
}


FormTagManager::~FormTagManager(void)
{
	::PrintLog(L"FormTagManager.~ctor.");

	if( p_instance_ != NULL )
	{
		delete p_instance_;
		p_instance_ = 0;
	}

	_hbrBkgnd = NULL;

	if ( NULL != _sysImgList )
	{
		long cRef = _sysImgList->Release();
		if( cRef == 0 )
		{
			_sysImgList = NULL;
		}
	}

	DllRelease();
}

// to keep the instance after the Form opend.
// this method must be called after the assignment of this->_handler, to make sure this->_handler.AddRef() be called.
IFACEMETHODIMP_(ULONG) FormTagManager::AddRef()
{
	::PrintLog(L"FormTagManager.AddRef.");
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) FormTagManager::Release()
{
	::PrintLog(L"FormTagManager.Release.");
	long cRef = 0;

	cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}

	return cRef;
}

void FormTagManager::ShowMsg(LPWSTR msg, COLORREF color)
{
	_ASSERT_EXPR(NULL != msg,L"msg could not be NULL.");
	_msgColor = color;
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_STATIC_ERROR_INFO, msg);
}

void FormTagManager::LoadTags(void)
{
	::PrintLog(L"FormTagManager.LoadTags.");
	_ASSERT_EXPR(NULL!=_hListTags,"_hListTags could not be NULL.");

	ListView_DeleteAllItems(_hListTags);

	auto &_tagHelper = *_handler->pTagHelper;

	// reload tags.
	_tagHelper.LoadTags(true);

	if ( _tagHelper.TagCount > 0 )
	{
		LVITEM item = {0};
		for (unsigned int i = 0; i < _tagHelper.TagCount; i++)
		{
			item.pszText = LPSTR_TEXTCALLBACK;
			item.mask = LVIF_TEXT | LVIF_IMAGE|LVIF_STATE;
			item.iItem = i;	//Zero-based index of the item to which this structure refers.
			item.stateMask = 0;
			item.iSubItem  = 0;
			item.state     = 0;
			item.iImage = I_IMAGECALLBACK;			
			ListView_InsertItem(_hListTags,&item);
		}
	}
}

void FormTagManager::LoadSelectedFiles(void)
{
	::PrintLog(L"FormTagManager.LoadSelectedFiles.");
	_ASSERT_EXPR(NULL!=_hListFiles,"_hListFiles could not be NULL.");

	ListView_DeleteAllItems(_hListFiles);

	auto &_tagHelper = *_handler->pTagHelper;
	if ( _tagHelper.FileCount > 0 )
	{
		LVITEM item = {0};
		for (unsigned int i = 0; i < _tagHelper.FileCount; i++)
		{
			item.pszText = LPSTR_TEXTCALLBACK;
			item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
			item.iItem = i;
			item.stateMask = 0;
			item.iSubItem  = 0;
			item.state     = 0;
			item.iImage = I_IMAGECALLBACK;
			ListView_InsertItem(_hListFiles,&item);
		}
	}
}

void FormTagManager::AddTag(void)
{
	::PrintLog(L"FormTagManager.AddTag.");
	_ASSERT_EXPR(NULL!=_hEditCtlNewTag,"_hEditCtlNewTag could not be NULL.");

	wchar_t tmp[MAXLENGTH_EACHTAG];
	LPWSTR key = tmp;
	UINT keyLength;
	BOOL x = IsNewTagOk(key,keyLength);

	if( TRUE == x)
	{
		::PrintLog(L"Got new tag =[ %s ], adding to database.", key);
		BOOL isAttchWithFilesChecked = Button_GetCheck(_hCheckAttachToFiles) == BST_CHECKED;
		int count = isAttchWithFilesChecked ? _handler->pTagHelper->FileCount : 0;
		UINT TID = 0;
		TID = _handler->pTagHelper->InsertTag(key,count); // not need to reload tags here, as they will be reloaded after be associated with files.
		if( TID != DB_RECORD_NOT_EXIST )
		{
			if( isAttchWithFilesChecked )
			{
				_handler->pTagHelper->SetTagByRecordId(_hwnd,TID);
			}

			LoadTags();

			ShowMsg(L"添加成功",COLOR_MY_OK);
		}
		else
		{
			// TODO: move the magic string to resource file.
			ShowMsg(L"添加失败",COLOR_MY_ERROR);
		}
	}else{
		::PrintLog(L"Got new tag =[ %s ], but it is not available.", key);
		ShowMsg(::MyLoadString(IDS_MSG_TAG_UNAVAILABLE),COLOR_MY_ERROR);
	}
}

void FormTagManager::EditTag(void)
{
	::PrintLog(L"FormTagManager.EditTag.");
}

void FormTagManager::DelTags(void)
{
	::PrintLog(L"FormTagManager.DelTags.");

	wchar_t selectedTags[MAXCOUNT_TAG][MAXLENGTH_EACHTAG] = {0};

	int count = 0;
	int searchFrom = -1;
	int selectedIdx = -1;

	// find first
	selectedIdx = ListView_GetNextItem(_hListTags,searchFrom,LVIS_SELECTED);
	while (selectedIdx > -1){
		ListView_GetItemText(_hListTags,selectedIdx,0,selectedTags[count],MAXLENGTH_EACHTAG);

		searchFrom = selectedIdx;
		count++;

		// find next
		selectedIdx = ListView_GetNextItem(_hListTags,searchFrom,LVIS_SELECTED);
	}

	if ( NULL != selectedTags && count > 0)
	{
		wchar_t msg[LOADSTRING_BUFFERSIZE];
		const int maxShown = 20;
		const UINT len = (MAXLENGTH_EACHTAG + 3) * MAXCOUNT_TAG;	// 3 for \0\r\n.
		wchar_t tmp[len] = {0};
		for (int i = 0; i < count && i < maxShown; i++)
		{
			StrCat(tmp,selectedTags[i]);
			if(i < count -1 )
				StrCat(tmp,L"\r\n");
		}

		if( maxShown < count ){
			wsprintf(msg,::MyLoadString(IDS_MSG_MORE_N_HIDDEN),count-maxShown);
			StrCat(tmp,L"\r\n");
			StrCat(tmp,msg);
		}

		wsprintf(msg,::MyLoadString(IDS_MSG_CONFRIM_DEL_TAG),tmp);
		int result = MessageBox(_hwnd,msg,::MyLoadString(IDS_MSGBOX_CAPTION_WARNING),MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON1);
		if( IDYES == result )
		{
			this->_handler->pTagHelper->DeleteTags(selectedTags,count);
			LoadTags();
		}
		else
		{
			MessageBox(_hwnd,L"CANCEL",L"Warning",MB_OK);
		}
	}
	else
	{
		// should not be occured.
		::PrintLog(L"Selected none while deleting. Should not be occured here.");
	}
}

BOOL FormTagManager::IsNewTagOk(_Out_ wchar_t* key,_Out_  UINT & keyLength)
{
	_ASSERT_EXPR(NULL!=key,"key could not be NULL");
	_ASSERT_EXPR(NULL!=_hEditCtlNewTag,"_hEditCtlNewTag could not be NULL.");

	BOOL result = FALSE;
	int nMaxCount = MAXLENGTH_EACHTAG;
	keyLength = GetWindowText(_hEditCtlNewTag,key,nMaxCount);
	if( keyLength > 0 )
	{
		LPCWSTR tmp = key;
		auto &_tagHelper = *_handler->pTagHelper;
		result = !_tagHelper.IsTagExists(tmp);
	}

	return result;
}

int FormTagManager::GetImgIdxInList(LPWSTR & pszPath)
{
	int imgIdx = 0;
	SHFILEINFOW sfi = {0};
	auto hr = SHGetFileInfo(pszPath, -1,&sfi,sizeof(sfi), SHGFI_ICON|SHGFI_LARGEICON);
	imgIdx = sfi.iIcon;
	DestroyIcon(sfi.hIcon); // we do not need the handle of icon, free the memory.
	return imgIdx;
}

void FormTagManager::InitText()
{	
	SetWindowText(_hwnd,::MyLoadString(IDS_DLG_TAGMANAGER_CAPTION));
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_STATIC_TagsExist,::MyLoadString(IDS_DLG_TAGMANAGER_LABEL_TAGSEXISTED));
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_STATIC_NewTag,::MyLoadString(IDS_DLG_TAGMANAGER_LABEL_NEWTAG));

	LPWSTR tmpFormater = ::MyLoadString(IDS_DLG_TAGMANAGER_CHECKBOX_ATTACHNEWTAGTOFILE);
	wchar_t str[LOADSTRING_BUFFERSIZE];
	wsprintf ( str,tmpFormater,this->_handler->pTagHelper->FileCount );
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_CHECK_AttachNewTagToFiles,str);

	SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_EDIT,::MyLoadString(IDS_DLG_TAGMANAGER_BU_EDIT));
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_DEL,::MyLoadString(IDS_DLG_TAGMANAGER_BU_DEL));
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_ADD,::MyLoadString(IDS_DLG_TAGMANAGER_BU_ADD));
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_CANCEL,::MyLoadString(IDS_DLG_TAGMANAGER_BU_CANCEL));
	SetDlgItemText(_hwnd, IDC_TAGMANAGER_STATIC_ERROR_INFO,L"");
}

void FormTagManager::InitTagList()
{
	_ASSERT_EXPR(NULL!=_hListTags,"_hListTags could not be NULL.");

	ListView_SetImageList(_hListTags,_sysImgList,LVSIL_SMALL);

	UINT cIdx = 0;
	LVCOLUMN c = {0};
	c.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_TAGNAME);
	c.mask = LVCF_TEXT | LVCF_MINWIDTH | LVCF_WIDTH;
	c.cxMin = 80;
	c.cx = 115;
	ListView_InsertColumn(_hListTags,cIdx++,&c);

	c.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_USECOUNT);
	c.mask = LVCF_TEXT | LVCF_MINWIDTH | LVCF_WIDTH | LVCF_FMT;
	c.cxMin = 30;
	c.cx = 40;
	c.fmt = LVCFMT_RIGHT;
	ListView_InsertColumn(_hListTags,cIdx++,&c);

	LoadTags();
}

void FormTagManager::InitFileList()
{
	_ASSERT_EXPR(NULL!=_hListFiles,"_hListFiles could not be NULL.");

	ListView_SetImageList(_hListFiles,_sysImgList,LVSIL_SMALL);

	UINT cIdx = 0;
	LVCOLUMN c = {0};
	c.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_FILES_HEADER_SELECTEDITEMS);
	c.mask = LVCF_TEXT | LVCF_MINWIDTH | LVCF_WIDTH;
	c.cxMin = 100;
	c.cx = 600;
	ListView_InsertColumn(_hListFiles,cIdx++,&c);

	LoadSelectedFiles();
}

void FormTagManager::GetListColValue_Tags(NMLVDISPINFO* & plvdi)
{
	switch (plvdi->item.iSubItem)
	{
	case 0:
		{
			plvdi->item.pszText = _handler->pTagHelper->Tags[plvdi->item.iItem].Tag;
			plvdi->item.iImage = 0;
		}
		break;

	case 1:
		_itow_s((int) _handler->pTagHelper->Tags[plvdi->item.iItem].UseCount,plvdi->item.pszText,10,10);
		break;

	default:
		break;
	}
}

void FormTagManager::GetListColValue_Files(NMLVDISPINFO* & plvdi)
{
	switch (plvdi->item.iSubItem)
	{
	case 0:
		{
			LPWSTR fileName = _handler->pTagHelper->TargetFileNames[plvdi->item.iItem];
			auto imgIdx = GetImgIdxInList(fileName);
			plvdi->item.pszText = fileName;
			plvdi->item.iImage = imgIdx;
			break;
		}
	default:
		break;
	}
}

void FormTagManager::OnNewTagChanged(WPARAM wParam, LPARAM lParam)
{
	auto code = HIWORD(wParam);
	if ( EN_CHANGE == code || EN_SETFOCUS == code)
	{
		// set the button ADD enabled/disabled up to the new tag.
		wchar_t key[MAXLENGTH_EACHTAG];
		UINT keyLength;
		BOOL x = IsNewTagOk(key,keyLength);

		auto bu = GetDlgItem(_hwnd,IDC_TAGMANAGER_BU_ADD);
		Button_Enable(bu,x);

		if ( x == TRUE )
		{
			ShowMsg(::MyLoadString(IDS_MSG_PRESS_ENTER_TO_SUBMIT),COLOR_MY_REMARK);
		}
		else
		{
			if( keyLength == 0 )
			{
				ShowMsg(::MyLoadString(IDS_MSG_PLS_ENTER_TAG),COLOR_MY_NOTE);
			}else{
				ShowMsg(::MyLoadString(IDS_MSG_TAG_UNAVAILABLE),COLOR_MY_ERROR);
			}
		}
	}
	else if ( EN_KILLFOCUS == code )
	{
		ShowMsg(L"");
	}
}

void FormTagManager::WM_NOTIFY_LIST_Tags(WPARAM & wParam, LPARAM & lParam)
{
	switch (((LPNMHDR) lParam)->code)
	{
	case LVN_GETDISPINFO:
		{
			NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
			GetListColValue_Tags(plvdi);
			break;
		}
	}

	UINT count = ListView_GetSelectedCount(_hListTags);
	Button_Enable(_hBuEdit,count > 0);
	Button_Enable(_hBuDel,count > 0);
}

void FormTagManager::WM_NOTIFY_LIST_Files(WPARAM & wParam, LPARAM & lParam)
{
	switch (((LPNMHDR) lParam)->code)
	{
	case LVN_GETDISPINFO:
		{
			NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
			GetListColValue_Files(plvdi);
			break;
		}
	}
}

#pragma region Messages
LRESULT CALLBACK FormTagManager::DlgProc(     
	_In_  HWND hwnd,     
	_In_  UINT uMsg,     
	_In_  WPARAM wParam,     
	_In_  LPARAM lParam     
	)     
{
	//::PrintLog(L"Message: 0x%x, wParam = 0x%x, lParam = 0x%x, hwnd = 0x%x", uMsg, wParam, lParam, hwnd);

	switch(uMsg)     
	{   
	case WM_INITDIALOG:
		{
			::PrintLog(L"Message: WM_INITDIALOG");

			// init
			this->_hwnd = hwnd;
			this->_handler = (CHandler *)lParam;

			this->_handler->AddRef();	// to keep the instance after the Form opend, will be released in WM_NCDESTROY.

			SHGetImageList(SHIL_SMALL,IID_IImageList,(void **)&_sysImgList);
			_sysImgList->AddRef();	// will be released in ~ctor.

			_hErrorInfo = GetDlgItem(_hwnd,IDC_TAGMANAGER_STATIC_ERROR_INFO);
			_hListTags = GetDlgItem(_hwnd,IDC_TAGMANAGER_LIST_TAGS);
			_hListFiles = GetDlgItem(_hwnd,IDC_TAGMANAGER_LIST_SelectedFiles);
			_hEditCtlNewTag = GetDlgItem(_hwnd,IDC_TAGMANAGER_EDIT_TagWord);
			_hCheckAttachToFiles = GetDlgItem(_hwnd, IDC_TAGMANAGER_CHECK_AttachNewTagToFiles);
			_hBuAdd = GetDlgItem(_hwnd,IDC_TAGMANAGER_BU_ADD);
			_hBuEdit = GetDlgItem(_hwnd,IDC_TAGMANAGER_BU_EDIT);
			_hBuDel = GetDlgItem(_hwnd,IDC_TAGMANAGER_BU_DEL);

			InitText();
			InitTagList();
			InitFileList();

			Button_SetCheck(_hCheckAttachToFiles,BST_CHECKED);
			Button_Enable(_hBuAdd,false);
			Edit_TakeFocus(_hEditCtlNewTag);

			break;
		}
	case MSG_TRANSFER_INSTANCES:
		::PrintLog(L"Message: MSG_TRANSFER_INSTANCES: wParam: %d, lParam: %d",wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:
		{
			// change the text color of static label ERROR_INFO
			if( (UINT)lParam == (UINT)_hErrorInfo){
				HDC hdcStatic = (HDC) wParam;
				SetTextColor(hdcStatic, _msgColor);

				// the background color will be changed to white, that is not what I want, so, restore the background color manual.
				auto oldBkColor = GetSysColor(CTLCOLOR_DLG);
				SetBkColor(hdcStatic,oldBkColor);

				if (_hbrBkgnd == NULL)
				{
					_hbrBkgnd = CreateSolidBrush(oldBkColor);
				}

				// the return value is a handle to a brush that the system uses to paint the background of the static control.
				// http://msdn.microsoft.com/en-us/library/windows/desktop/bb787524(v=vs.85).aspx
				return (INT_PTR)_hbrBkgnd;
			}
		}
	case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
			case SC_KEYMENU:
				{
					::PrintLog(L"Message: SC_KEYMENU: wParam: %d, lParam: %d",wParam, lParam);
					break;
				}
			case SC_CLOSE:
				{
					DestroyWindow(hwnd);
					break;
				}
			default:
				break;
			}
		}
		break;
	case WM_COMMAND:
		{
			::PrintLog(L"Message: WM_COMMAND");
			switch(LOWORD(wParam))     
			{    
			case IDC_TAGMANAGER_BU_DEL:
				{
					DelTags();
					break;
				}
			case IDC_TAGMANAGER_BU_EDIT:
				{
					EditTag();
					break;
				}
			case IDC_TAGMANAGER_BU_ADD:
				{
					AddTag();
					break;
				}
			case IDC_TAGMANAGER_CHECK_AttachNewTagToFiles:
				::PrintLog(L"Message: IDC_CHECK_AttachNewTagToFiles");
				break;
			case IDC_TAGMANAGER_BU_CANCEL:
				DestroyWindow(hwnd);
				break;
			case IDC_TAGMANAGER_EDIT_TagWord:
				{
					OnNewTagChanged(wParam,lParam);
					break;
				}
			default:
				break;
			}
		}
		break;
		//case WM_SIZE:
		//	{
		//		auto hTreeView = GetDlgItem(hdlg, IDC_TREE1);  
		//		if(hTreeView!=NULL)
		//		{

		//		}
		//		return 0;
		//	}
	case WM_KEYDOWN:
		{
			::PrintLog(L"Message: WM_KEYDOWN");
		}
		break;
	case WM_KEYUP:
		{
			::PrintLog(L"Message: WM_KEYUP");
			if(wParam==VK_ESCAPE)
			{
				DestroyWindow(hwnd);
			}
			else if( wParam == VK_RETURN )
			{
				// simulate text changed to validate the new tag.
				OnNewTagChanged(MAKEWPARAM(IDC_TAGMANAGER_EDIT_TagWord, EN_SETFOCUS),(LPARAM)_hEditCtlNewTag);
				// simulate the button ADD to be Clicked.
				SendMessage(_hwnd,WM_COMMAND, MAKEWPARAM(IDC_TAGMANAGER_BU_ADD, BN_CLICKED),(LPARAM)_hBuAdd);
			}
		}
		break;
	case WM_CHAR:
		{
			::PrintLog(L"Message: WM_CHAR");
		}
		break;
	case WM_SYSCHAR:
		{
			::PrintLog(L"Message: WM_SYSCHAR");
		}
		break;
	case WM_DEADCHAR:
		{
			::PrintLog(L"Message: WM_DEADCHAR");
		}
		break;
	case WM_SYSDEADCHAR:
		{
			::PrintLog(L"Message: WM_SYSDEADCHAR");
		}
		break;
	case WM_NOTIFY:
		{
			LPNMHDR pNmhdr = (LPNMHDR) lParam;
			switch (pNmhdr->idFrom)
			{
			case IDC_TAGMANAGER_LIST_TAGS:
				{
					WM_NOTIFY_LIST_Tags(wParam,lParam);
					break;
				}
			case IDC_TAGMANAGER_LIST_SelectedFiles:
				{
					WM_NOTIFY_LIST_Files(wParam,lParam);
					break;
				}
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
	case WM_NCDESTROY:
		{
			::PrintLog(L"Message: WM_NCDESTROY");
			if(this->_handler != NULL){
				auto x = this->_handler->Release();
				if( x == 0 ){
					this->_handler = NULL;
				}
			}
		}
		break;
	default:
		break;
	}

	return (INT_PTR)FALSE;
}

#pragma endregion
