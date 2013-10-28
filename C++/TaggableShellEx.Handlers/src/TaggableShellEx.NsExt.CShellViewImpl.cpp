#pragma once
#include "..\include\TaggableShellEx.NsExt.CShellViewImpl.h"
#include <Winuser.h> // GWLP_WNDPROC, GWLP_USERDATA
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
	{
		m_psfContainingFolder->Release();
		m_psfContainingFolder = NULL;
	}

	if ( NULL != m_spShellBrowser )
	{
		m_spShellBrowser->Release();
		m_spShellBrowser = NULL;
	}
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


// pass -1 for the current selected item
// returns an IShellItem type object
HRESULT CShellViewImpl::GetItemFromView(IFolderView2 *pfv, int iItem, REFIID riid, void **ppv)
{
	*ppv = NULL;
	HRESULT hr = S_OK;
	if (iItem == -1)
	{
		hr = pfv->GetSelectedItem(-1, &iItem); // Returns S_FALSE if none selected
	}

	if (S_OK == hr)
	{
		hr = pfv->GetItem(iItem, riid, ppv);
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

// ICommDlgBrowser
STDMETHODIMP CShellViewImpl::OnDefaultCommand(IShellView * psv)
{
	::PrintLog(L"CShellViewImpl::OnDefaultCommand");




	/*
	IFolderView2 *pfv2;
	HRESULT hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if (SUCCEEDED(hr))
	{
	IShellItem *psi;
	hr = GetItemFromView(pfv2, -1, IID_PPV_ARGS(&psi));
	if (SUCCEEDED(hr))
	{
	PIDLIST_ABSOLUTE pidl;
	hr = SHGetIDListFromObject(psi, &pidl);
	if (SUCCEEDED(hr))
	{
	SHELLEXECUTEINFO ei = { sizeof(ei) };
	ei.fMask        = SEE_MASK_INVOKEIDLIST;
	ei.hwnd         = m_hwndParent;
	ei.nShow        = SW_NORMAL;
	ei.lpIDList     = pidl;

	ShellExecuteEx(&ei);

	CoTaskMemFree(pidl);
	}
	psi->Release();
	}
	pfv2->Release();
	}*/

	HRESULT hr = SHGetIDListFromObject(psv,&m_psfContainingFolder->m_pIDFolder);
	//auto last = m_PidlMgr.GetLastItem(m_psfContainingFolder->m_pIDFolder);
	if( hr == S_OK )
	{
		m_spShellBrowser->BrowseObject(m_psfContainingFolder->m_pIDFolder,SBSP_DEFBROWSER | SBSP_ABSOLUTE);
	}

	return S_OK;
}

STDMETHODIMP CShellViewImpl::OnStateChange(IShellView * psv, ULONG uChange)
{
	::PrintLog(L"CShellViewImpl::OnStateChange, uChange = %d",uChange);
	if (uChange == CDBOSC_SELCHANGE)
	{
		PostMessage(m_hWnd, KFD_SELCHANGE, 0, 0);
	}
	return S_OK;
}

STDMETHODIMP CShellViewImpl::IncludeObject(IShellView * psv, PCUITEMID_CHILD pidl)
{
	::PrintLog(L"CShellViewImpl::IncludeObject");
	return S_OK;
}


STDMETHODIMP CShellViewImpl::GetWindow ( HWND* phwnd )
{
	// Return our container window's handle to the browser.
	*phwnd = m_hWnd;
	return S_OK;
}

unsigned __stdcall CShellViewImpl::FillList_Tags(void * pThis)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		OleInitialize(0);   // for drag and drop

		CShellViewImpl * pthX = (CShellViewImpl*)pThis;
		pthX->_peb->RemoveAll();
		pthX->_peb->SetEmptyText(::MyLoadString(IDS_MSG_LOADING_TAGS));

		IEnumIDList *pEnum = NULL;
		LPITEMIDLIST pidl = NULL;
		HRESULT hr;

		DWORD flag = SHCONTF_FOLDERS;
		IShellFolder *sf = pthX->m_psfContainingFolder;
		// Get an enumerator object for the folder's contents.
		if ( pthX->m_psfContainingFolder->m_PIDLCurrent == NULL )
		{
			sf = pthX->m_psfContainingFolder;
		}
		else
		{
			flag = SHCONTF_CHECKING_FOR_CHILDREN;
			//auto data = pthX->m_PidlMgr.GetData(pthX->m_psfContainingFolder->m_PIDLCurrent);
			//::PrintLog(data->wszDisplayName);

			//pthX->m_psfContainingFolder->BindToObject(pthX->m_psfContainingFolder->m_PIDLCurrent,NULL,IID_PPV_ARGS(&sf));
		}

		hr = sf->EnumObjects ( pthX->m_hWnd, flag,&pEnum );

		if ( FAILED(hr) )
			return 0;

		// Add items.
		DWORD dwFetched;
		while ( pEnum->Next(1, &pidl, &dwFetched) == S_OK )	// the pidl is relative.
		{
			IShellItem *psi;
			hr = SHCreateShellItem(pthX->m_psfContainingFolder->m_pIDFolder,NULL,pidl,&psi);
			if (SUCCEEDED(hr))
			{
				hr = pthX->_prf->AddItem(psi);
				psi->Release();
			}

			// free memory allocated by pEnum->Next
			CoTaskMemFree(pidl);
		}

		// the calling application must free the returned IEnumIDList object by calling its Release method.
		pEnum->Release();

		pthX->_peb->SetEmptyText(::MyLoadString(IDS_MSG_NO_TAG_WITH_DETAIL));

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
	m_spShellBrowser->GetWindow(&m_hwndParent);
	m_spShellBrowser->SetStatusTextSB(::MyLoadString(IDS_ProductIntro));


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

	*phWnd = NULL;
	m_hWnd = CreateWindowEx ( dwListExStyles,WC_STATIC, NULL, dwListStyles,0, 0,
		prcView->right - prcView->left,prcView->bottom - prcView->top,
		m_hwndParent, NULL, g_hInst, 0 );

	if ( NULL == m_hWnd )
		return -1;

	HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_peb));
	if (SUCCEEDED(hr))
	{
		IUnknown_SetSite(_peb, static_cast<IServiceProvider *>(this));

		hr = _peb->Initialize(m_hWnd,prcView,&m_FolderSettings);
		if (SUCCEEDED(hr))
		{
			// initialized
			_peb->SetOptions(EBO_NAVIGATEONCE | EBO_SHOWFRAMES); // do not allow navigations
			_peb->SetEmptyText(::MyLoadString(IDS_MSG_LOADING_TAGS));

			// Initialize the exporer browser so that we can use the results folder
			// as the data source. This enables us to program the contents of
			// the view via IResultsFolder
			hr = _peb->FillFromObject(NULL, EBF_NONE);
			if (SUCCEEDED(hr))
			{
				// get the window handle
				IShellView* spSV;
				_peb->GetCurrentView(IID_PPV_ARGS(&spSV));
				spSV->GetWindow(phWnd);

				// change the message procedure
				lpPrevWndFunc = SetWindowLongPtr(*phWnd,GWLP_WNDPROC, (LONG_PTR)WndProc);
				lpPrevUserData = SetWindowLongPtr(*phWnd,GWLP_USERDATA,(LONG_PTR)this);

				IFolderView2 *pfv2;
				hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
				if (SUCCEEDED(hr))
				{
					IColumnManager *pcm;
					hr = pfv2->QueryInterface(&pcm);
					if (SUCCEEDED(hr))
					{
						PROPERTYKEY rgkeys[] = {PKEY_ItemNameDisplay, PKEY_FileCount};
						hr = pcm->SetColumns(rgkeys, ARRAYSIZE(rgkeys));
						if (SUCCEEDED(hr))
						{
							// tag name
							CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_NAME | CM_MASK_STATE | CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
							hr = pcm->GetColumnInfo(PKEY_ItemNameDisplay, &ci);
							if (SUCCEEDED(hr))
							{
								ci.dwState = CM_STATE_ALWAYSVISIBLE;
								ci.uWidth = 250;
								ci.uDefaultWidth = 250;
								ci.uIdealWidth = 250;
								StringCbPrintf(ci.wszName,sizeof(ci.wszName),L"%s",::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_TAGNAME) );
								pcm->SetColumnInfo(PKEY_ItemNameDisplay, &ci);
							}

							// use count
							CM_COLUMNINFO ci2 = {sizeof(ci2), CM_MASK_NAME | CM_MASK_STATE | CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
							hr = pcm->GetColumnInfo(PKEY_FileCount, &ci2);
							if (SUCCEEDED(hr))
							{
								ci2.dwState = CM_STATE_ALWAYSVISIBLE;
								ci2.uWidth = 100;
								ci2.uDefaultWidth = 100;
								ci2.uIdealWidth = 100;
								StringCbPrintf(ci2.wszName,sizeof(ci2.wszName),L"%s",::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_USECOUNT) );
								pcm->SetColumnInfo(PKEY_FileCount, &ci2);
							}

						}
						pcm->Release();
					}

					hr = pfv2->GetFolder(IID_PPV_ARGS(&_prf));
					if (SUCCEEDED(hr))
					{
						FillList();
					}
					pfv2->Release();
				}
			}
		}
	}

	return hr;
}


// DestroyViewWindow() is responsible for destroying our windows & cleaning
// up stuff.
STDMETHODIMP CShellViewImpl::DestroyViewWindow()
{
	// Clean up the UI.
	UIActivate ( SVUIA_DEACTIVATE );

	_peb->Destroy();
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

	FillList();

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

// Other stuff
HRESULT CShellViewImpl::Init ( CShellFolderImpl* pContainingFolder )
{
	m_psfContainingFolder = pContainingFolder;

	if ( NULL != m_psfContainingFolder )
		m_psfContainingFolder->AddRef();

	return S_OK;
}

// FillList() populates the list control.
void CShellViewImpl::FillList()
{
	_beginthreadex(0,0,FillList_Tags,this,0,0);
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