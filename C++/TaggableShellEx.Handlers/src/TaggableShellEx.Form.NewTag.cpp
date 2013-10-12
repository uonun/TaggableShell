#include "..\include\TaggableShellEx.Form.NewTag.h"


FormNewTag::FormNewTag(void)
{
}


FormNewTag::~FormNewTag(void)
{

}

#pragma region Messages
LRESULT CALLBACK FormNewTag::DlgProc(     
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
			::PrintLog(L"Message: WM_INITDIALOG");
			SetWindowText(hwnd,::MyLoadString(IDS_DLG_NEWTAG_CAPTION));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_STATIC_TagsExist),::MyLoadString(IDS_DLG_NEWTAG_LABEL_TAGSEXISTED));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_STATIC_NewTag),::MyLoadString(IDS_DLG_NEWTAG_LABEL_NEWTAG));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_CHECK_AttachNewTagToFiles),::MyLoadString(IDS_DLG_NEWTAG_CHECKBOX_ATTACHNEWTAGTOFILE));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_BU_ADD),::MyLoadString(IDS_DLG_NEWTAG_BU_ADD));
			SetWindowText(GetDlgItem(hwnd, IDC_NEWTAG_BU_CANCEL),::MyLoadString(IDS_DLG_NEWTAG_BU_CANCEL));
			return 0;
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
