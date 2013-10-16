#include "..\include\TaggableShellEx.Form.TagManager.h"

FormTagManager* FormTagManager::p_instance_ = NULL;

FormTagManager::FormTagManager(void):
	_cRef(1)
	,_msgColor(COLOR_MY_DEFAULT)
{
	::PrintLog(L"FormTagManager.ctor.");
}


FormTagManager::~FormTagManager(void)
{
	::PrintLog(L"FormTagManager.~ctor.");
}

// to keep the instance after the Form opend.
// this method must be called after the assignment of this->_handler, to make sure this->_handler.AddRef() be called.
IFACEMETHODIMP_(ULONG) FormTagManager::AddRef()
{
	if(this->_handler != NULL){
		this->_handler->AddRef();
	}

	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) FormTagManager::Release()
{
	if(this->_handler != NULL){
		try
		{
			auto x = this->_handler->Release();
			if( x == 0 ){
				this->_handler = NULL;
			}
		}
		catch(int e)
		{
			this->_handler = NULL;
		}
	}

	long cRef = InterlockedDecrement(&_cRef);
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
	SetDlgItemText(_hwnd, IDC_STATIC_ERROR_INFO, msg);
}

void FormTagManager::LoadTags(void)
{
	::PrintLog(L"FormTagManager.LoadTags.");

	auto &_tagHelper = _handler->TagHelper;

	HWND lv = GetDlgItem(_hwnd, IDC_TAGMANAGER_LIST_TAGS);
	if( lv != NULL )
	{
		ListView_DeleteAllItems(lv);

		if ( _tagHelper.TagCount > 0 )
		{
			LVITEM item = {0};
			for (unsigned int i = 0; i < _tagHelper.TagCount; i++)
			{
				item.pszText = LPSTR_TEXTCALLBACK;
				item.mask = LVIF_TEXT | LVIF_IMAGE|LVIF_STATE;
				item.iItem = i;
				item.stateMask = 0;
				item.iSubItem  = 0;
				item.state     = 0;
				item.iImage = I_IMAGECALLBACK;
				ListView_InsertItem(lv,&item);
			}
		}
	}
}

void FormTagManager::LoadSelectedFiles(void)
{
	::PrintLog(L"FormTagManager.LoadSelectedFiles.");

	auto &_tagHelper = _handler->TagHelper;

	HWND lv = GetDlgItem(_hwnd, IDC_TAGMANAGER_LIST_SelectedFiles);
	if( lv != NULL )
	{
		ListView_DeleteAllItems(lv);

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
				ListView_InsertItem(lv,&item);
			}
		}
	}
}

void FormTagManager::AddTag(void)
{
	::PrintLog(L"FormTagManager.AddTag.");
	HWND editCtl = GetDlgItem(_hwnd, IDC_TAGMANAGER_EDIT_TagWord);
	if(editCtl != NULL)
	{
		wchar_t tmp[MAXLENGTH_EACHTAG];
		LPWSTR key = tmp;
		UINT keyLength;
		BOOL x = IsNewTagOk(key,keyLength,editCtl);

		if( TRUE == x)
		{
			::PrintLog(L"Got new tag =[ %s ], adding to database.", key);
			BOOL isAttchWithFilesChecked = Button_GetCheck(GetDlgItem(_hwnd, IDC_TAGMANAGER_CHECK_AttachNewTagToFiles)) == BST_CHECKED;
			int count = isAttchWithFilesChecked ? _handler->TagHelper.FileCount : 0;
			auto TID = _handler->TagHelper.InsertTag(key,count);
			if(TID==TID_NOT_EXIST)
			{
				// TODO: move the magic string to resource file.
				ShowMsg(L"添加失败",COLOR_MY_ERROR);
			}
			else
			{
				LoadTags();
				ShowMsg(L"添加成功",COLOR_MY_OK);
			}
		}else{
			::PrintLog(L"Got new tag =[ %s ], but it is not available.", key);

		}
	}
}

BOOL FormTagManager::IsNewTagOk(_Out_ wchar_t* key,_Out_  UINT & keyLength, HWND editCtl)
{
	_ASSERT_EXPR(NULL!=key,"key == NULL");

	BOOL result = FALSE;

	if ( NULL == editCtl){
		editCtl = GetDlgItem(_hwnd, IDC_TAGMANAGER_EDIT_TagWord);
		_ASSERT_EXPR(editCtl != NULL,"Can not find the EDIT CONTROL for new tag.");
	}

	int nMaxCount = MAXLENGTH_EACHTAG;
	keyLength = GetWindowText(editCtl,key,nMaxCount);
	if( keyLength > 0 )
	{
		LPCWSTR tmp = key;
		auto &_tagHelper = _handler->TagHelper;
		result = !_tagHelper.IsTagExists(tmp);
	}

	return result;
}

int FormTagManager::GetImgIdxInList(LPWSTR & pszPath)
{
	SHFILEINFOW sfi = {0};
	auto hr = SHGetFileInfo(pszPath, -1,&sfi,sizeof(sfi), SHGFI_ICON|SHGFI_LARGEICON);
	int imgIdx = sfi.iIcon;
	DestroyIcon(sfi.hIcon); // we do not need the handle of icon, free the memory.
	return imgIdx;
}

static HBRUSH hbrBkgnd = NULL;

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

			// to keep the instance after the Form opend.
			// this method must be called after the assignment of this->_handler, to make sure this->_handler.AddRef() be called.
			this->AddRef();

			SetWindowText(_hwnd,::MyLoadString(IDS_DLG_TAGMANAGER_CAPTION));
			SetDlgItemText(_hwnd, IDC_TAGMANAGER_STATIC_TagsExist,::MyLoadString(IDS_DLG_TAGMANAGER_LABEL_TAGSEXISTED));
			SetDlgItemText(_hwnd, IDC_TAGMANAGER_STATIC_NewTag,::MyLoadString(IDS_DLG_TAGMANAGER_LABEL_NEWTAG));

			LPWSTR tmpFormater = ::MyLoadString(IDS_DLG_TAGMANAGER_CHECKBOX_ATTACHNEWTAGTOFILE);
			wchar_t str[LOADSTRING_BUFFERSIZE];
			wsprintf ( str,tmpFormater,this->_handler->TagHelper.FileCount );
			SetDlgItemText(_hwnd, IDC_TAGMANAGER_CHECK_AttachNewTagToFiles,str);
			Button_SetCheck(GetDlgItem(_hwnd, IDC_TAGMANAGER_CHECK_AttachNewTagToFiles),BST_CHECKED);

			SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_EDIT,::MyLoadString(IDS_DLG_TAGMANAGER_BU_EDIT));
			SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_DEL,::MyLoadString(IDS_DLG_TAGMANAGER_BU_DEL));
			SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_ADD,::MyLoadString(IDS_DLG_TAGMANAGER_BU_ADD));
			SetDlgItemText(_hwnd, IDC_TAGMANAGER_BU_CANCEL,::MyLoadString(IDS_DLG_TAGMANAGER_BU_CANCEL));
			SetDlgItemText(_hwnd, IDC_STATIC_ERROR_INFO,L"");

			Button_Enable(GetDlgItem(_hwnd,IDC_TAGMANAGER_BU_ADD),false);

			HWND lv = NULL;
			// init tag list
			lv = GetDlgItem(_hwnd, IDC_TAGMANAGER_LIST_TAGS);
			if( lv != NULL )
			{
				IImageList *imgList;
				HRESULT hr = SHGetImageList(SHIL_SMALL,IID_IImageList,(void **)&imgList);
				ListView_SetImageList(lv,imgList,LVSIL_SMALL);
				imgList->Release();

				UINT cIdx = 0;
				LVCOLUMN c = {0};
				c.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_TAGNAME);
				c.mask = LVCF_TEXT | LVCF_MINWIDTH | LVCF_WIDTH;
				c.cxMin = 80;
				c.cx = 115;
				ListView_InsertColumn(lv,cIdx++,&c);

				c.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_USECOUNT);
				c.mask = LVCF_TEXT | LVCF_MINWIDTH | LVCF_WIDTH | LVCF_FMT;
				c.cxMin = 30;
				c.cx = 40;
				c.fmt = LVCFMT_RIGHT;
				ListView_InsertColumn(lv,cIdx++,&c);

				LoadTags();
			}

			// init file list
			lv = GetDlgItem(_hwnd, IDC_TAGMANAGER_LIST_SelectedFiles);
			if( lv != NULL )
			{
				IImageList *imgList;
				HRESULT hr = SHGetImageList(SHIL_SMALL,IID_IImageList,(void **)&imgList);
				ListView_SetImageList(lv,imgList,LVSIL_SMALL);
				imgList->Release();

				UINT cIdx = 0;
				LVCOLUMN c = {0};
				c.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_FILES_HEADER_SELECTEDITEMS);
				c.mask = LVCF_TEXT | LVCF_MINWIDTH | LVCF_WIDTH;
				c.cxMin = 100;
				c.cx = 600;
				ListView_InsertColumn(lv,cIdx++,&c);

				LoadSelectedFiles();
			}

			return 0;
		}
	case MSG_TRANSFER_INSTANCES:
		::PrintLog(L"Message: MSG_TRANSFER_INSTANCES: wParam: %d, lParam: %d",wParam, lParam);
		return 0;
	case WM_CTLCOLORSTATIC:
		{
			// change the text color of static label ERROR_INFO
			if( (UINT)lParam == (UINT)GetDlgItem(_hwnd,IDC_STATIC_ERROR_INFO)){
				HDC hdcStatic = (HDC) wParam;
				SetTextColor(hdcStatic, _msgColor);

				// the background color will be changed to white, that is not what I want, so, restore the background color manual.
				auto oldBkColor = GetSysColor(CTLCOLOR_DLG);
				SetBkColor(hdcStatic,oldBkColor);

				if (hbrBkgnd == NULL)
				{
					hbrBkgnd = CreateSolidBrush(oldBkColor);
				}

				// the return value is a handle to a brush that the system uses to paint the background of the static control.
				// http://msdn.microsoft.com/en-us/library/windows/desktop/bb787524(v=vs.85).aspx
				return (INT_PTR)hbrBkgnd;
			}
			else
			{
				return 0;
			}
		}
	case WM_SYSCOMMAND:
		{
			if(wParam == SC_CLOSE)
			{
				DestroyWindow(hwnd);
				return 0;
			}
		}
	case WM_COMMAND:
		{
			::PrintLog(L"Message: WM_COMMAND");
			switch(LOWORD(wParam))     
			{     
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
					// set the button ADD enabled/disabled up to the new tag.
					wchar_t key[MAXLENGTH_EACHTAG];
					UINT keyLength;
					BOOL x = IsNewTagOk(key,keyLength,(HWND)lParam);

					auto bu = GetDlgItem(_hwnd,IDC_TAGMANAGER_BU_ADD);
					Button_Enable(bu,x);

					if ( x == TRUE || keyLength == 0)
					{
						ShowMsg(L"");
					}
					else
					{
						ShowMsg(::MyLoadString(IDS_ERR_PLS_ENTER_TEXT_OF_TAG),COLOR_MY_ERROR);
					}
				}
				break;
			default:
				break;
			}
			return 0;
		}
		//case WM_SIZE:
		//	{
		//		auto hTreeView = GetDlgItem(hdlg, IDC_TREE1);  
		//		if(hTreeView!=NULL)
		//		{

		//		}
		//		return 0;
		//	}
	case  WM_NOTIFY:
		{
			NMLVDISPINFO* plvdi;

			switch (((LPNMHDR) lParam)->code)
			{
			case LVN_GETDISPINFO:
				{
					switch (wParam)
					{
					case IDC_TAGMANAGER_LIST_TAGS:
						{
							plvdi = (NMLVDISPINFO*)lParam;
							switch (plvdi->item.iSubItem)
							{
							case 0:
								{
									plvdi->item.pszText = _handler->TagHelper.Tags[plvdi->item.iItem].Tag;
									plvdi->item.iImage = 0;
								}
								break;

							case 1:
								_itow_s((int) _handler->TagHelper.Tags[plvdi->item.iItem].UseCount,plvdi->item.pszText,10,10);
								break;

							default:
								break;
							}
							break;
						}
					case IDC_TAGMANAGER_LIST_SelectedFiles:
						{
							plvdi = (NMLVDISPINFO*)lParam;
							switch (plvdi->item.iSubItem)
							{
							case 0:
								{
									LPWSTR fileName = _handler->TagHelper.TargetFileNames[plvdi->item.iItem];
									auto imgIdx = GetImgIdxInList(fileName);
									plvdi->item.pszText = fileName;
									plvdi->item.iImage = imgIdx;
									break;
								}
							default:
								break;
							}
							break;
						}
					default:
						break;
					}
					break;
				}
			}
			return 0;     
		}
	case WM_DESTROY:     
		{
			PostQuitMessage(0);
			return 0;     
		}
	case WM_NCDESTROY:
		{
			::PrintLog(L"Message: WM_NCDESTROY");
			this->Release();
			return 0;     
		}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

#pragma endregion
