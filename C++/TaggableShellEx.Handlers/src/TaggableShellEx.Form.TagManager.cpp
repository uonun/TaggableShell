#include "..\include\TaggableShellEx.Form.TagManager.h"

FormTagManager::FormTagManager(void) : _handler(NULL)
{
	::PrintLog(L"FormTagManager.ctor.");
}


FormTagManager::~FormTagManager(void)
{
	::PrintLog(L"FormTagManager.~ctor.");
}

void FormTagManager::LoadTags(void)
{
	::PrintLog(L"FormTagManager.LoadTags.");

	auto &_tagHelper = this->_handler->TagHelper;

	HWND lv = GetDlgItem(_hwnd, IDC_NEWTAG_LIST_TAGS);
	if( lv != NULL && _tagHelper.TagCount > 0)
	{
		//IImageList *imgList;
		//HRESULT hr = SHGetImageList(SHIL_SMALL,IID_IImageList,(void **)&imgList);
		//TreeView_SetImageList(lv,imgList,TVSIL_NORMAL);
		//imgList->Release();

		for (unsigned int i = 0; i < _tagHelper.TagCount; i++)
		{
			LVITEM item = {0};
			item.pszText = _tagHelper.Tags[i].Tag;
			item.mask = LVIF_TEXT;
			ListView_InsertItem(lv,&item);
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
	switch(uMsg)     
	{   
	case WM_INITDIALOG:
		{
			// init
			this->_handler = (CHandler *)lParam;
			this->_hwnd = hwnd;

			::PrintLog(L"Message: WM_INITDIALOG");
			SetWindowText(hwnd,::MyLoadString(IDS_DLG_NEWTAG_CAPTION));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_STATIC_TagsExist),::MyLoadString(IDS_DLG_NEWTAG_LABEL_TAGSEXISTED));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_STATIC_NewTag),::MyLoadString(IDS_DLG_NEWTAG_LABEL_NEWTAG));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_CHECK_AttachNewTagToFiles),::MyLoadString(IDS_DLG_NEWTAG_CHECKBOX_ATTACHNEWTAGTOFILE));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_BU_ADD),::MyLoadString(IDS_DLG_NEWTAG_BU_ADD));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_BU_CANCEL),::MyLoadString(IDS_DLG_NEWTAG_BU_CANCEL));


			LoadTags();
			return 0;
		}
	case MSG_TRANSFER_INSTANCES:
		::PrintLog(L"Message: MSG_TRANSFER_INSTANCES: wParam: %d, lParam: %d",wParam, lParam);
		return 0;
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
			case IDC_NEWTAG_BU_ADD:
				::PrintLog(L"Message: IDC_BU_ADD");
				break;
			case IDC_NEWTAG_CHECK_AttachNewTagToFiles:
				::PrintLog(L"Message: IDC_CHECK_AttachNewTagToFiles");
				break;
			case IDC_NEWTAG_BU_CANCEL:
				DestroyWindow(hwnd);
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

		}
	case WM_DESTROY:     
		{
			PostQuitMessage(0);     
			return 0;     
		}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

#pragma endregion
