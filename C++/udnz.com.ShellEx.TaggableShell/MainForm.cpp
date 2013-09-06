#include <ShlObj.h>
#include <shlwapi.h>
#include "MainForm.h"
#include <string>
#include <sstream>
using namespace std;

MainForm::MainForm(void)
{
}


MainForm::~MainForm(void)
{

}

// Tree-View Control Reference http://msdn.microsoft.com/en-us/library/ff486110(v=vs.85).aspx
// Windows Tree-View Control http://www.songho.ca/misc/treeview/treeview.html
HRESULT MainForm::LoadShellItems(void)
{
	LPSHELLFOLDER psfDeskTop = NULL;
	// Get an IShellFolder interface pointer
	HRESULT hr = SHGetDesktopFolder(&psfDeskTop);
	if(FAILED(hr))
		return hr;

	if(psfDeskTop!=NULL){

		// 获取TreeView控件的句柄     
		auto hTreeView = GetDlgItem(hdlg, IDC_TREE1);  
		if(hTreeView==NULL)
			return S_FALSE;

		// 清空
		TreeView_DeleteAllItems(hTreeView);

		LPITEMIDLIST pidlProgFiles = NULL;
		IShellFolder *psfProgFiles = NULL;

		// 获取路径的 LPITEMIDLIST 对象
		hr = SHGetFolderLocation(NULL, CSIDL_DRIVES, NULL, NULL, &pidlProgFiles);

		// 将 LPITEMIDLIST 指定的路径 绑定到 IShellFolder 上
		hr = psfDeskTop->BindToObject(pidlProgFiles, NULL, IID_IShellFolder, (LPVOID *) &psfProgFiles);
		psfDeskTop->Release();

		LPENUMIDLIST ppenum;
		hr = psfProgFiles->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ppenum);

		if(FAILED(hr))
			return hr;

		// 添加根节点
		auto trRoot = InsertItem(hTreeView,L"My Computer",TVI_ROOT,NULL,0,0,NULL);

		LPITEMIDLIST pidlItems = NULL;
		STRRET strDispName;
		TCHAR pszDisplayName[MAX_PATH];
		ULONG celtFetched;
		//IShellFolder *psfFirstFolder = NULL;
		ULONG uAttr;

		while( hr = ppenum->Next(1,&pidlItems, &celtFetched) == S_OK && (celtFetched) == 1)
		{
			psfProgFiles->GetDisplayNameOf(pidlItems, SHGDN_INFOLDER, &strDispName);
			StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

			OutputDebugString(pszDisplayName);
			OutputDebugString(L"\r\n");

			// 添加到 TreeView
			//IShellFolder *s = NULL;
			//hr = psfProgFiles->BindToObject(pidlItems, NULL, IID_IShellFolder, (LPVOID *) &s);

			ITEMIDLIST* item = new ITEMIDLIST(*pidlItems);		
			InsertItem(hTreeView,pszDisplayName,trRoot,NULL,0,0,(LPARAM)item);

			//if(!psfFirstFolder)
			//{
			//	uAttr = SFGAO_FOLDER;
			//	psfProgFiles->GetAttributesOf(1, (LPCITEMIDLIST *) &pidlItems, &uAttr);
			//	if(uAttr & SFGAO_FOLDER)
			//	{
			//		hr = psfProgFiles->BindToObject(pidlItems, NULL, IID_IShellFolder, (LPVOID *) &psfFirstFolder);
			//	}
			//}
			CoTaskMemFree(pidlItems);
		}
		
		// 展开根节点
		TreeView_Expand(hTreeView,trRoot,TVE_EXPAND);
	}

	return S_OK;
}


HTREEITEM MainForm::InsertItem(HWND hwnd,const wchar_t* str, HTREEITEM parent, HTREEITEM insertAfter,int imageIndex, int selectedImageIndex,LPARAM pidl)
{
	// build TVINSERTSTRUCT
	TVINSERTSTRUCT insertStruct;
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = insertAfter;
	insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	insertStruct.item.pszText = (LPWSTR)str;
	insertStruct.item.cchTextMax = sizeof(str)/sizeof(str[0]);
	insertStruct.item.iImage = imageIndex;
	insertStruct.item.iSelectedImage = selectedImageIndex;
	insertStruct.itemex.lParam = pidl;

	// insert the item
	//return (HTREEITEM)::SendMessage(hwnd, TVM_INSERTITEM,0, (LPARAM)&insertStruct);
	return TreeView_InsertItem(hwnd,&insertStruct);
}

LRESULT CALLBACK MainForm::DlgProc(     
	_In_  HWND hwnd,     
	_In_  UINT uMsg,     
	_In_  WPARAM wParam,     
	_In_  LPARAM lParam     
	)     
{     
	switch(uMsg)     
	{   
	case WM_INITDIALOG:
		return 0;
	case WM_SYSCOMMAND:
		if(wParam == SC_CLOSE)
		{
			// 如果执行了关闭
			// 销毁对话框，将收到WM_DESTROY消息
			DestroyWindow(hdlg);
			return 0;
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))     
			{     
			case IDC_BU_LOAD:
				LoadShellItems();
				break;
			case ID_M_EXIT:
				DestroyWindow(hdlg);
				break;
			}
			return 0;
		}
	case  WM_NOTIFY:
		{
			return TreeViewNotify(wParam,lParam);
		}
	case WM_DESTROY:     
		PostQuitMessage(0);     
		return 0;     
	default:
		WCHAR msg[260];
		wsprintf(msg,L"Message: %d\r\n",uMsg);
		OutputDebugStringW((LPCWSTR)msg);
	}
	return (INT_PTR)FALSE;
}

// handle WM_NOTIFY message
// Tree-View Notifications http://www.songho.ca/misc/treeview/treeviewnotify.html
int MainForm::TreeViewNotify(WPARAM wParam, LPARAM lParam)
{
    // first cast lParam to NMHDR* to know what event is
    NMHDR* nmhdr = (NMHDR*)lParam;

    // TreeView notifications start with TVN_
    switch(nmhdr->code)
    {
    // drag-and-drop operation has begun
    case TVN_BEGINDRAG:
        // cast again lParam to NMTREEVIEW*
        break;

    // drag-and-drop operation using right mouse button has begun
    case TVN_BEGINRDRAG:
        break;

    // label editing has begun
    case TVN_BEGINLABELEDIT:
        // cast again lParam to NMTVDISPINFO*
        break;

    // label editing has ended
    case TVN_ENDLABELEDIT:
        // cast again lParam to NMTVDISPINFO*
        break;

    // an item has been deleted
    case TVN_DELETEITEM:
        break;

    // TreeView needs info(such as item text) to display an item
    case TVN_GETDISPINFO:
        break;

    // parent window must update the item information
    case TVN_SETDISPINFO:
		break;

		// list of items was expanded or collapsed
	case TVN_ITEMEXPANDED:
		break;

		// list of items are about to be expanded or collapsed
	case TVN_ITEMEXPANDING:
		break;

		// a keyboard event has occurred
    case TVN_KEYDOWN:
        // When the TreeView control is contained in a dialog box,
        // IsDialogMessage() processes the ESC and ENTER keystrokes and
        // does not pass them on to the edit control that is created by
        // the TreeView control. The result is that the keystrokes have
        // no effect.
        // Cast again lParam to NMTVKEYDOWN*
        break;

		// the item selection has changed
	case TVN_SELCHANGED:
		{
			auto pnmtv = (LPNMTREEVIEW)lParam;
			HTREEITEM item = pnmtv->itemNew.hItem;
			
			OutputDebugStringW(L"TVN_SELCHANGED\r\n");
		}
		break;

    // the item selection is about to change
    case TVN_SELCHANGING:
        break;

    default:
        break;
    }

    return 0;
}