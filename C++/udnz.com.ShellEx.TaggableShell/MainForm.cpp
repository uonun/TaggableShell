#include "MainForm.h"


MainForm* MainForm::_instance = NULL;

MainForm::MainForm(void)
{

}


MainForm::~MainForm(void)
{

}

MainForm* MainForm::Instance(void)
{
	auto lock = LocalAlloc(LMEM_FIXED,1);
	LocalLock(lock);
	if(_instance==NULL){
		_instance= new MainForm();
	}
	LocalUnlock(lock);
	return _instance;
}

void MainForm::Init(HWND hdlg,HINSTANCE hApp){
	this->hApp = hApp;
	this->hdlg = hdlg;
}

unsigned __stdcall MainForm::ThreadStaticEntryPoint(void * pThis)
{
	MainForm * pthX = (MainForm*)pThis;
	pthX->LoadImageList();
	pthX->LoadShellItems();
	return 1;          // the thread exit code, If the function succeeds, the return value is nonzero.
}

void MainForm::GetDisplayName(IShellFolder* sf,LPITEMIDLIST pidlItems,_In_ UINT pszDisplayNameBuffSize,_Out_ LPWSTR pszDisplayName){
	STRRET strDispName;	
	sf->GetDisplayNameOf(pidlItems, SHGDN_INFOLDER, &strDispName);
	StrRetToBuf(&strDispName,pidlItems, pszDisplayName,pszDisplayNameBuffSize);

	if(pszDisplayName[0] == '\0'){
		sf->GetDisplayNameOf(pidlItems, SHGDN_FORPARSING, &strDispName);
		StrRetToBuf(&strDispName, pidlItems, pszDisplayName, pszDisplayNameBuffSize);
	}
}

HRESULT MainForm::LoadImageList(void){
	IImageList *imgList;

	HRESULT hr = SHGetImageList(SHIL_SMALL,IID_IImageList,(void **)&imgList);
	TreeView_SetImageList(GetDlgItem(hdlg, IDC_TREE1),imgList,TVSIL_NORMAL);
	return  hr;
}

int MainForm::GetImgIdxInList(LPCTSTR pszPath)
{
	SHFILEINFOW sfi = {0};
	auto hr = SHGetFileInfo(pszPath, -1,&sfi,sizeof(sfi),SHGFI_PIDL | SHGFI_ICON);
	int imgIdx = sfi.iIcon;
	DestroyIcon(sfi.hIcon); // we do not need the handle of icon, free the memory.
	return imgIdx;
}

// Tree-View Control Reference http://msdn.microsoft.com/en-us/library/ff486110(v=vs.85).aspx
// Windows Tree-View Control http://www.songho.ca/misc/treeview/treeview.html
HRESULT MainForm::LoadShellItems(void)
{
	HWND buLoad = GetDlgItem(hdlg, IDC_BU_LOAD);
	Button_Enable(buLoad,false);

	WCHAR str_loading[LOADSTRING_BUFFERSIZE];
	LoadString(NULL,IDS_LOADING,str_loading,sizeof(str_loading)/sizeof(str_loading[0]));
	SetWindowText(buLoad,str_loading);

	LPSHELLFOLDER psfDeskTop = NULL;
	// Get an IShellFolder interface pointer
	HRESULT hr = SHGetDesktopFolder(&psfDeskTop);
	if(FAILED(hr))
		return hr;

	if(psfDeskTop!=NULL){

		// ��ȡTreeView�ؼ��ľ��     
		auto hTreeView = GetDlgItem(hdlg, IDC_TREE1);  
		if(hTreeView==NULL)
			return S_FALSE;

		// ���
		TreeView_DeleteAllItems(hTreeView);

		// �����桱����ʾ����
		WCHAR deskTopName[MAX_PATH];
		GetDisplayName(psfDeskTop,NULL,MAX_PATH,deskTopName);

		// generate a user data for tree-view item.
		LPTVITEMDATA data = new TVITEMDATA();
		data->bExpanded = true;
		data->pShellFolder = psfDeskTop;

		// �������桱��Ϊ���ڵ���ӵ���
		auto tvItem = InsertItem(hTreeView,deskTopName,NULL,NULL,0,0,(LPARAM)data);

		// Ϊ���ڵ���� 1 ���Ӽ�
		hr = LoadSubItem(psfDeskTop,hTreeView,tvItem,1);

		// չ�����ڵ�
		TreeView_Expand(hTreeView,tvItem,TVE_EXPAND);
	}

	WCHAR str_load[LOADSTRING_BUFFERSIZE];
	LoadString(NULL,IDS_BU_LOAD,str_load,sizeof(str_load)/sizeof(str_load[0]));
	SetWindowText(buLoad,str_load);

	Button_Enable(buLoad,true);

	return S_OK;
}


HRESULT MainForm::LoadSubItem(IShellFolder* sf,HWND tv,HTREEITEM parent,int deep)
{	
	HRESULT hr = S_FALSE;

	if(deep > 0 && sf != NULL)
	{
		LPENUMIDLIST ppenum;
		hr = sf->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ppenum);
		if(hr == S_OK)
		{
			LPITEMIDLIST pidlItems = NULL;
			STRRET strDispName;
			TCHAR pszDisplayName[MAX_PATH];
			ULONG celtFetched;
			ULONG uAttr;

			while( hr = ppenum->Next(1,&pidlItems, &celtFetched) == S_OK && (celtFetched) == 1)
			{
				WCHAR pszDisplayName[MAX_PATH];
				GetDisplayName(sf,pidlItems,MAX_PATH,pszDisplayName);

				OutputDebugString(pszDisplayName);
				OutputDebugString(L"\r\n");

				// bind LPITEMIDLIST to IShellFolder
				IShellFolder *sub = NULL;
				hr = sf->BindToObject(pidlItems, NULL, IID_IShellFolder, (LPVOID *)&sub);

				// get image index in system image list.
				int imgIdx = GetImgIdxInList((LPCTSTR)pidlItems);

				// generate a user data for tree-view item.
				LPTVITEMDATA data = new TVITEMDATA();
				data->bExpanded = deep > 1;
				data->pShellFolder = sub;
				 
				// add tree-view item with user data((LPARAM)pidlItems).
				auto tvItem = InsertItem(tv,pszDisplayName,parent,NULL,imgIdx,imgIdx,(LPARAM)data);

				// update tree-view
				UpdateWindow(tv);

#pragma region Get sub items.
				if(hr == S_OK)
				{
					uAttr = SFGAO_FOLDER ;
					sf->GetAttributesOf(1, (LPCITEMIDLIST *) &pidlItems, &uAttr);
					if(uAttr & SFGAO_FOLDER)
					{
						deep--;
						LoadSubItem(sub,tv,tvItem,deep);
						deep++;
					}
				}
#pragma endregion

				CoTaskMemFree(pidlItems);
			}

		}
		sf->Release();
	}
	return hr;
}


HTREEITEM MainForm::InsertItem(HWND hwnd,const LPWSTR str, HTREEITEM parent, HTREEITEM insertAfter,int imageIndex, int selectedImageIndex,LPARAM pidl)
{
	// build TVINSERTSTRUCT
	TVINSERTSTRUCT insertStruct;
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = insertAfter;
	insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	insertStruct.item.pszText = str;
	insertStruct.item.cchTextMax = sizeof(str)/sizeof(str[0]);
	insertStruct.item.iImage = imageIndex;
	insertStruct.item.iSelectedImage = selectedImageIndex;
	insertStruct.item.lParam = pidl;

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
		{
			WCHAR str_load[LOADSTRING_BUFFERSIZE];
			LoadString(NULL,IDS_BU_LOAD,str_load,sizeof(str_load)/sizeof(str_load[0]));
			SetWindowText(GetDlgItem(hwnd, IDC_BU_LOAD),str_load);
			return 0;
		}
	case WM_SYSCOMMAND:
		if(wParam == SC_CLOSE)
		{
			// ���ִ���˹ر�
			// ���ٶԻ��򣬽��յ�WM_DESTROY��Ϣ
			DestroyWindow(hdlg);
			return 0;
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))     
			{     
			case IDC_BU_LOAD:
				_beginthreadex(0,0,ThreadStaticEntryPoint,this,0,0);
				break;
			case ID_M_EXIT:
				DestroyWindow(hdlg);
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
		{
			auto pnmtv = (LPNMTREEVIEW)lParam;
			TVITEMDATA *data = (LPTVITEMDATA)pnmtv->itemNew.lParam;
			if(data!=NULL && !data->bExpanded){
				auto tv=pnmtv->hdr.hwndFrom;
				auto tCurrentItem =pnmtv->itemNew.hItem;
				auto sf = data->pShellFolder;
				LoadSubItem(sf,tv,tCurrentItem,1);
				data->bExpanded=true;
				TreeView_Expand(tv,tCurrentItem,TVE_EXPAND);
			}
			OutputDebugStringW(L"TVN_SELCHANGED\r\n");
		}
		break;

	default:
		break;
	}

	return 0;
}


