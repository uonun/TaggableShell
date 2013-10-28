#pragma once
#include "..\include\TaggableShellEx.NsExt.CShellViewImpl.h"
#include <propkey.h>

ULONG_PTR lpPrevWndFunc = 0;	// The previous window procedure
ULONG_PTR lpPrevUserData = 0;	// The previous window procedure
UINT const KFD_SELCHANGE = WM_USER;

CShellViewImpl::CShellViewImpl(void): 
	_cRef(1) // IUnknown
	,m_hwndParent(NULL)
	,m_hWnd(NULL)
	,m_spShellBrowser(NULL)
	,m_psfContainingFolder(NULL)
	, _peb(NULL), _prf(NULL)
{
	::PrintLog(L"CShellViewImpl.ctor");
}


CShellViewImpl::~CShellViewImpl(void)
{
	::PrintLog(L"CShellViewImpl.~ctor");

	if ( NULL != m_psfContainingFolder )
		m_psfContainingFolder->Release();

	if ( NULL != m_spShellBrowser )
		m_spShellBrowser->Release();
}

HRESULT CShellViewImpl_CreateInstance(REFIID riid, void **ppv)
{
	::PrintLog(L"CShellViewImpl_CreateInstance");
	CShellViewImpl *pNew = new(std::nothrow) CShellViewImpl;
	HRESULT hr = pNew ? S_OK : E_OUTOFMEMORY;
	if (pNew)
	{
		hr = pNew->QueryInterface(riid, ppv);
		pNew->Release();
	}
	return hr;
}

// IUnknown
IFACEMETHODIMP CShellViewImpl::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CShellViewImpl, IShellView),
		QITABENT(CShellViewImpl, IOleCommandTarget),
		QITABENT(CShellViewImpl, IServiceProvider),
		QITABENT(CShellViewImpl, ICommDlgBrowser),
		QITABENT(CShellViewImpl, IOleWindow),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CShellViewImpl::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CShellViewImpl::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

// IServiceProvider
STDMETHODIMP CShellViewImpl::QueryService(REFGUID guidService, REFIID riid, void **ppv)
{
	HRESULT hr = E_NOINTERFACE;
	*ppv = NULL;
	if (guidService == SID_SExplorerBrowserFrame)
	{
		// responding to this SID allows us to hook up our ICommDlgBrowser
		// implementation so we get selection change events from the view
		hr = QueryInterface(riid, ppv);
	}
	return hr;
}

// ICommDlgBrowser
STDMETHODIMP CShellViewImpl::OnDefaultCommand(IShellView * /* psv */)
{
	//_OnExplore();
	return S_OK;
}

STDMETHODIMP CShellViewImpl::OnStateChange(IShellView * /* psv */, ULONG uChange)
{
	if (uChange == CDBOSC_SELCHANGE)
	{
		PostMessage(m_hWnd, KFD_SELCHANGE, 0, 0);
	}
	return S_OK;
}

STDMETHODIMP CShellViewImpl::IncludeObject(IShellView * /* psv */, PCUITEMID_CHILD /* pidl */)
{
	return S_OK;
}


STDMETHODIMP CShellViewImpl::GetWindow ( HWND* phwnd )
{
	// Return our container window's handle to the browser.
	*phwnd = m_hWnd;
	return S_OK;
}

unsigned __stdcall CShellViewImpl::ThreadStaticEntryPoint(void * pThis)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		OleInitialize(0);   // for drag and drop

		CShellViewImpl * pthX = (CShellViewImpl*)pThis;

		IKnownFolderManager *pManager;
		HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pManager));
		if (SUCCEEDED(hr))
		{
			UINT cCount;
			KNOWNFOLDERID *pkfid;

			hr = pManager->GetFolderIds(&pkfid, &cCount);
			if (SUCCEEDED(hr))
			{
				for (UINT i = 0; i < cCount; i++)
				{
					IKnownFolder *pKnownFolder;
					hr = pManager->GetFolder(pkfid[i], &pKnownFolder);
					if (SUCCEEDED(hr))
					{
						IShellItem *psi;
						hr = pKnownFolder->GetShellItem(0, IID_PPV_ARGS(&psi));
						if (SUCCEEDED(hr))
						{
							hr = pthX->_prf->AddItem(psi);
							psi->Release();
						}
						pKnownFolder->Release();
					}
				}
				CoTaskMemFree(pkfid);
			}
			pManager->Release();
		}

		UpdateWindow(pthX->m_hWnd);

		OleUninitialize();
		CoUninitialize();
	}

	return 1;          // the thread exit code, If the function succeeds, the return value is nonzero.
}


// CreateViewWindow() creates the container window.  Once that window is
// created, it will create the list control.
STDMETHODIMP CShellViewImpl::CreateViewWindow ( LPSHELLVIEW pPrevView, 
											   LPCFOLDERSETTINGS lpfs,
											   LPSHELLBROWSER psb, 
											   LPRECT prcView, HWND* phWnd )
{
	*phWnd = NULL;

	// Init member variables.
	m_spShellBrowser = psb;
	m_spShellBrowser->AddRef();	// The view should call this interface's AddRef method and keep the interface pointer to allow communication with the Windows Explorer window
	m_spShellBrowser->GetWindow(&m_hwndParent);

	m_FolderSettings = *lpfs;
	m_FolderSettings.ViewMode = FVM_DETAILS;

	DWORD dwListStyles = WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD dwListExStyles = WS_EX_CLIENTEDGE | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD dwListExtendedStyles = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;

	switch ( m_FolderSettings.ViewMode )
	{
	case FVM_ICON:      dwListStyles |= LVS_ICON;      break;
	case FVM_SMALLICON: dwListStyles |= LVS_SMALLICON; break;
	case FVM_LIST:      dwListStyles |= LVS_LIST;      break;
	case FVM_DETAILS:   dwListStyles |= LVS_REPORT;    break;
		DEFAULT_UNREACHABLE;
	}

	// DirectUIHWND   ExplorerBrowserControl

	*phWnd = NULL;
	m_hWnd = CreateWindowEx ( dwListExStyles,WC_STATIC, NULL, dwListStyles,0, 0,
		prcView->right - prcView->left,prcView->bottom - prcView->top,
		m_hwndParent, NULL, g_hInst, 0 );

	if ( NULL == m_hWnd )
		return -1;
	

	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	MapWindowRect(m_hwndParent,m_hWnd , &rc);
	 //HRESULT hr = SHCoCreateInstance(NULL, &CLSID_ExplorerBrowser, NULL, IID_PPV_ARGS(&_peb));


	HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&_peb));
	if (SUCCEEDED(hr))
	{
		IUnknown_SetSite(_peb, static_cast<IServiceProvider *>(this));

		//FOLDERSETTINGS fs = {0};
		//fs.ViewMode = FVM_DETAILS;
		//fs.fFlags = FWF_AUTOARRANGE | FWF_NOWEBVIEW;
		hr = _peb->Initialize(m_hwndParent, &rc, lpfs);
		if (SUCCEEDED(hr))
		{
			_peb->SetOptions(EBO_NAVIGATEONCE | EBO_SHOWFRAMES); // do not allow navigations

			// Initialize the exporer browser so that we can use the results folder
			// as the data source. This enables us to program the contents of
			// the view via IResultsFolder

			hr = _peb->FillFromObject(NULL, EBF_NONE);
			if (SUCCEEDED(hr))
			{
				IFolderView2 *pfv2;
				hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
				if (SUCCEEDED(hr))
				{
					IColumnManager *pcm;
					hr = pfv2->QueryInterface(&pcm);
					if (SUCCEEDED(hr))
					{
						PROPERTYKEY rgkeys[] = {PKEY_ItemNameDisplay, PKEY_ItemFolderPathDisplay};
						hr = pcm->SetColumns(rgkeys, ARRAYSIZE(rgkeys));
						if (SUCCEEDED(hr))
						{
							CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
							hr = pcm->GetColumnInfo(PKEY_ItemFolderPathDisplay, &ci);
							if (SUCCEEDED(hr))
							{
								ci.uWidth += 100;
								ci.uDefaultWidth += 100;
								ci.uIdealWidth += 100;
								pcm->SetColumnInfo(PKEY_ItemFolderPathDisplay, &ci);
							}
						}
						pcm->Release();
					}

					hr = pfv2->GetFolder(IID_PPV_ARGS(&_prf));
					if (SUCCEEDED(hr))
					{
						//_StartFolderEnum();
						//_beginthreadex(0,0,ThreadStaticEntryPoint,this,0,0);

						IKnownFolderManager *pManager;
						HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pManager));
						if (SUCCEEDED(hr))
						{
							UINT cCount;
							KNOWNFOLDERID *pkfid;

							hr = pManager->GetFolderIds(&pkfid, &cCount);
							if (SUCCEEDED(hr))
							{
								for (UINT i = 0; i < cCount; i++)
								{
									IKnownFolder *pKnownFolder;
									hr = pManager->GetFolder(pkfid[i], &pKnownFolder);
									if (SUCCEEDED(hr))
									{
										IShellItem *psi;
										hr = pKnownFolder->GetShellItem(0, IID_PPV_ARGS(&psi));
										if (SUCCEEDED(hr))
										{
											hr = _prf->AddItem(psi);
											psi->Release();
										}
										pKnownFolder->Release();
									}
								}
								CoTaskMemFree(pkfid);
							}
							pManager->Release();
						}


					}
					pfv2->Release();
				}
			}
		}
	}

	*phWnd = m_hWnd;
	UpdateWindow(m_hWnd);








	//DWORD dwListStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | //WS_BORDER |
	//	LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS | LVS_ALIGNLEFT;
	//DWORD dwListExStyles = WS_EX_CLIENTEDGE;
	//DWORD dwListExtendedStyles = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;

	//switch ( m_FolderSettings.ViewMode )
	//{
	//case FVM_ICON:      dwListStyles |= LVS_ICON;      break;
	//case FVM_SMALLICON: dwListStyles |= LVS_SMALLICON; break;
	//case FVM_LIST:      dwListStyles |= LVS_LIST;      break;
	//case FVM_DETAILS:   dwListStyles |= LVS_REPORT;    break;
	//	DEFAULT_UNREACHABLE;
	//}

	//*phWnd = NULL;
	//m_hWnd = CreateWindowEx ( dwListExStyles,WC_LISTVIEW, NULL, dwListStyles,0, 0,
	//	prcView->right - prcView->left,prcView->bottom - prcView->top,
	//	m_hwndParent, NULL, g_hInst, 0 );

	//if ( NULL == m_hWnd )
	//	return -1;

	//// change the message procedure
	//lpPrevWndFunc = SetWindowLongPtr(m_hWnd,GWLP_WNDPROC, (LONG_PTR)WndProc);
	//lpPrevUserData = SetWindowLongPtr(m_hWnd,GWLP_USERDATA,(LONG_PTR)this);

	//LVCOLUMN lvc;
	//int iCol = 0;

	//// Initialize the LVCOLUMN structure.
	//// The mask specifies that the format, width, text,
	//// and subitem members of the structure are valid.
	//lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	//// Add the columns.
	//lvc.iSubItem = iCol++;
	//lvc.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_TAGNAME);
	//lvc.cx = 200;									// Width of column in pixels.
	//lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
	//// Insert the columns into the list view.
	//ListView_InsertColumn(m_hWnd, iCol, &lvc);

	//// Add other columns for REPORT
	//if ( dwListStyles & LVS_REPORT )
	//{
	//	lvc.iSubItem = iCol++;
	//	lvc.pszText = ::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_USECOUNT);
	//	lvc.cx = 80;									// Width of column in pixels.
	//	lvc.fmt = LVCFMT_RIGHT;  // Left-aligned column.
	//	// Insert the columns into the list view.
	//	ListView_InsertColumn(m_hWnd, iCol, &lvc);
	//}

	//FillList();

	//*phWnd = m_hWnd;
	return S_OK;
}


// DestroyViewWindow() is responsible for destroying our windows & cleaning
// up stuff.
STDMETHODIMP CShellViewImpl::DestroyViewWindow()
{
	// Clean up the UI.
	UIActivate ( SVUIA_DEACTIVATE );
	DestroyWindow(m_hWnd);

	Release();

	return S_OK;
}


// GetCurrentInfo() returns our FODLERSETTINGS to the browser.
STDMETHODIMP CShellViewImpl::GetCurrentInfo ( LPFOLDERSETTINGS lpfs )
{
	*lpfs = m_FolderSettings;
	return S_OK;
}


// Refresh() refreshes the shell view.
STDMETHODIMP CShellViewImpl::Refresh()
{
	// Repopulate the list control.

	//ListView_DeleteAllItems(m_hWnd);
	//FillList();

	return S_OK;
}


// UIActivate() is called whenever the focus switches among the Address bar,
// the tree, and our shell view.
STDMETHODIMP CShellViewImpl::UIActivate ( UINT uState )
{
	// Nothing to do if the state hasn't changed since the last call.
	//
	//if ( m_uUIState == uState )
	//    return S_OK;
	//
	//// Modify the Explorer menu and status bar.

	//HandleActivate ( uState );

	return S_OK;
}


// QueryStatus() is called by the browser to determine which standard commands
// our extension supports.
STDMETHODIMP CShellViewImpl::QueryStatus ( const GUID* pguidCmdGroup, ULONG cCmds,
										  OLECMD prgCmds[], OLECMDTEXT* pCmdText )
{
	if ( NULL == prgCmds )
		return E_POINTER;

	// The only useful standard command I've figured out is "refresh".  I've put
	// some trace messages in so you can see the other commands that the
	// browser sends our way.  If you can figure out what they're all for,
	// let me know!

	if ( NULL == pguidCmdGroup )
	{
		for ( UINT u = 0; u < cCmds; u++ )
		{
			PrintLog(">> Query - DEFAULT: %u\n", prgCmds[u]);

			switch ( prgCmds[u].cmdID )
			{
			case OLECMDID_REFRESH:
				prgCmds[u].cmdf = OLECMDF_SUPPORTED | OLECMDF_ENABLED;
				break;
			}
		}

		return S_OK;
	}
	else if ( CGID_Explorer == *pguidCmdGroup )
	{
		for ( UINT u = 0; u < cCmds; u++ )
		{
			PrintLog(">> Query - EXPLORER: %u\n", prgCmds[u]);
		}
	}
	else if ( CGID_ShellDocView == *pguidCmdGroup )
	{
		for ( UINT u = 0; u < cCmds; u++ )
		{
			PrintLog(">> Query - DOCVIEW: %u\n", prgCmds[u]);
		}
	}

	return OLECMDERR_E_UNKNOWNGROUP;
}


// Exec() is called when the user executes a command in Explorer that we
// have to deal with.
STDMETHODIMP CShellViewImpl::Exec ( const GUID* pguidCmdGroup, DWORD nCmdID,
								   DWORD nCmdExecOpt, VARIANTARG* pvaIn,
								   VARIANTARG* pvaOut )
{
	HRESULT hrRet = OLECMDERR_E_UNKNOWNGROUP;

	// The only standard command we act on is "refresh".  I've put
	// some trace messages in so you can see the other commands that the
	// browser sends our way.  If you can figure out what they're all for,
	// let me know!

	if ( NULL == pguidCmdGroup )
	{
		PrintLog(">> Exec - DEFAULT: %u\n", nCmdID);

		if ( OLECMDID_REFRESH == nCmdID )
		{
			Refresh();
			hrRet = S_OK;
		}
	}
	else if ( CGID_Explorer == *pguidCmdGroup )
	{
		PrintLog(">> Exec - EXPLORER : %u\n", nCmdID);
	}
	else if ( CGID_ShellDocView == *pguidCmdGroup )
	{
		PrintLog(">> Exec - DOCVIEW: %u\n", nCmdID);
	}

	return hrRet;
}


// FillList() populates the list control.
void CShellViewImpl::FillList()
{
	IEnumIDList *pEnum = NULL;
	LPITEMIDLIST pidl = NULL;
	HRESULT hr;

	// Get an enumerator object for the folder's contents.  Since this simple
	// extension doesn't deal with subfolders, we request only non-folder
	// objects.

	hr = m_psfContainingFolder->EnumObjects ( m_hWnd, SHCONTF_NONFOLDERS,&pEnum );

	if ( FAILED(hr) )
		return;

	// Stop redrawing to avoid flickering
	SendMessage(m_hWnd,WM_SETREDRAW,FALSE,0);

	// Add items.
	DWORD dwFetched;
	int i = 0;
	while ( pEnum->Next(1, &pidl, &dwFetched) == S_OK )
	{
		// Store PIDL to the drive letter, using the lParam member for each item
		MYPIDLDATA* data = m_PidlMgr.GetData ( pidl );
		// free memory allocated by pEnum->Next
		CoTaskMemFree(pidl);

		LVITEM lvi = {0};
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lvi.iImage = 0;
		lvi.iItem = i;	//Zero-based index of the item to which this structure refers.
		lvi.lParam = (LPARAM) m_PidlMgr.Create ( data );

		// Column 1
		lvi.pszText = data->wszDisplayName;

		auto x = ListView_InsertItem(m_hWnd,&lvi);
		if( x != -1 ){
			// show more details in other column.
			auto style = GetWindowStyle(m_hWnd);
			if ( style & LVS_REPORT )
			{
				// the max length of UINT is 64 + \0 = 65.
				wchar_t buffer[65];
				_ui64tow_s(data->UseCount,buffer,65,10);
				ListView_SetItemText(m_hWnd,i,1,buffer);
			}

			i++;
		}

	}

	// the calling application must free the returned IEnumIDList object by calling its Release method.
	pEnum->Release();

	// Redraw the list view.
	SendMessage(m_hWnd,WM_SETREDRAW,TRUE,0);
	UpdateWindow(m_hWnd);
}

LRESULT CALLBACK WndProc(
	HWND hwnd,        // handle to window
	UINT uMsg,        // message identifier
	WPARAM wParam,    // first message parameter
	LPARAM lParam)    // second message parameter
{ 
	//::PrintLog(L"Message: 0x%x, wParam = 0x%x, lParam = 0x%x, hwnd = 0x%x", uMsg, wParam, lParam, hwnd);

	CShellViewImpl* pView = NULL;
	LONG_PTR x = GetWindowLongPtr(hwnd,GWLP_USERDATA);
	if ( x > 0)
	{
		pView = (CShellViewImpl*)x;
	}

	switch (uMsg) 
	{ 
		//case WM_CREATE: 
		//	// Initialize the window. 
		//	return 0; 

		//case WM_PAINT: 
		//	// Paint the window's client area. 
		//	return 0; 

		//case WM_SIZE: 
		//	// Set the size and position of the window. 
		//	return 0; 

		//case WM_DESTROY: 
		//	return 0; 

	case WM_NCDESTROY: 
		// Clean up window-specific data objects. 
		{
			// Remove the subclass from the control. 
			SetWindowLongPtr(hwnd, GWLP_WNDPROC,(LONG_PTR) lpPrevWndFunc); 
			SetWindowLongPtr(hwnd,GWLP_USERDATA,lpPrevUserData);
		}
		return 0; 
		// 
		// Process other messages. 
		// 

	default:
		return CallWindowProc((WNDPROC)lpPrevWndFunc,hwnd, uMsg, wParam, lParam); 
	} 
	return 0; 
} 