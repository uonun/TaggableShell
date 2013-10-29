#pragma once
#include "..\include\TaggableShellEx.NsExt.CShellViewImpl.h"
#include <Winuser.h> // GWLP_WNDPROC, GWLP_USERDATA
#include <propkey.h>

CShellViewImpl::CShellViewImpl(void): 
	_cRef(1) // IUnknown
	,m_hwndParent(NULL)
	,m_hWnd(NULL)
	,m_spShellBrowser(NULL)
	,m_psfContainingFolder(NULL)
	, _peb(NULL),_prf(NULL)
	,_isRefreshing(FALSE)
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

	if ( NULL != _peb )
	{
		_peb->Destroy();
		_peb = NULL;
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

	HRESULT hr;

	LPITEMIDLIST last;

	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
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
				last = m_PidlMgr.GetLastItem(pidl);
				if( hr == S_OK )
				{
					auto data = m_PidlMgr.GetData(last);
					if ( data != NULL && (data->Type == MYSHITEMTYPE_TAG || data->Type == MYSHITEMTYPE_FILE ) )
					{
						::PrintLog(L"CShellViewImpl::OnDefaultCommand: %s",data->wszDisplayName);
						hr = m_spShellBrowser->BrowseObject(last, SBSP_DEFBROWSER | SBSP_RELATIVE);
					}
					else
					{
						SHELLEXECUTEINFO ei = { sizeof(ei) };
						ei.fMask        = SEE_MASK_INVOKEIDLIST;
						ei.hwnd         = m_hwndParent;
						ei.nShow        = SW_NORMAL;
						ei.lpIDList     = pidl;

						ShellExecuteEx(&ei);
					}
				}
				CoTaskMemFree(pidl);
			}
			psi->Release();
		}
		pfv2->Release();
	}

	return hr;
}

STDMETHODIMP CShellViewImpl::OnStateChange(IShellView * psv, ULONG uChange)
{
	::PrintLog(L"CShellViewImpl::OnStateChange, uChange = %d",uChange);
	if (uChange == CDBOSC_SELCHANGE)
	{
		//PostMessage(m_hWnd, KFD_SELCHANGE, 0, 0);
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

unsigned __stdcall CShellViewImpl::FillList_Asyn(void * pThis)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IEnumIDList *pEnum = NULL;

		CShellViewImpl * pthX = (CShellViewImpl*)pThis;
		pthX->_isRefreshing = TRUE;

		LPWSTR infoLoading = 0,infoLoaded = 0;
		BOOL isShowTag = pthX->IsShowTag();
		if ( isShowTag )
		{
			infoLoading = ::MyLoadString(IDS_MSG_LOADING_TAGS);
			infoLoaded = ::MyLoadString(IDS_MSG_NO_TAG_WITH_DETAIL);
		}
		else
		{
			auto data = pthX->m_PidlMgr.GetData(pthX->m_psfContainingFolder->m_PIDLCurrent);
			LPWSTR currentTag = data->wszDisplayName;

			WCHAR tmp[MAX_PATH];
			wsprintf ( tmp,::MyLoadString(IDS_MSG_LOADING_FILES_FOR_TAG),currentTag);
			infoLoading = tmp;

			WCHAR tmp2[MAX_PATH];
			wsprintf ( tmp2,::MyLoadString(IDS_MSG_NO_FILE_IN_TAG_WITH_DETAIL),currentTag);
			infoLoaded = tmp2;			
		}

		try{
			pthX->_peb->SetEmptyText(infoLoading);
			pthX->_peb->RemoveAll();

			UpdateWindow(pthX->m_hWnd);

			// Stop redrawing to avoid flickering
			SendMessage( pthX->m_hWnd,WM_SETREDRAW,FALSE,0);

			LPITEMIDLIST pidl = NULL;
			HRESULT hr;

			DWORD flag = isShowTag ? SHCONTF_FOLDERS : SHCONTF_NONFOLDERS;
			CShellFolderImpl *sf = pthX->m_psfContainingFolder;

			hr = sf->EnumObjects ( pthX->m_hWnd, flag,&pEnum );
			if ( SUCCEEDED(hr) )
			{
				// Add items.
				DWORD dwFetched;
				while ( pEnum->Next(1, &pidl, &dwFetched) == S_OK )	// the pidl is relative.
				{
					IShellItem *psi;
					if( pthX->IsShowTag() )
					{
						hr = SHCreateShellItem(pthX->m_psfContainingFolder->m_pIDFolder,NULL,pidl,&psi);
					}else{
						auto data = pthX->m_PidlMgr.GetData(pidl);
						if ( data != NULL )
						{
							hr = SHCreateItemFromParsingName(data->wszDisplayName,NULL,IID_PPV_ARGS(&psi));
						}
					}

					if (SUCCEEDED(hr))
					{
						hr = pthX->_prf->AddItem(psi);
						psi->Release();
					}else{
						// ERROR_FILE_NOT_FOUND
						// ERROR_PATH_NOT_FOUND
						// ERROR_INVALID_DRIVE
						// ....
					}

					// free memory allocated by pEnum->Next
					CoTaskMemFree(pidl);
				}
			}
		}
		catch(int e)
		{
		}

		// the calling application must free the returned IEnumIDList object by calling its Release method.
		if ( NULL != pEnum )
			pEnum->Release();

		pthX->_peb->SetEmptyText(infoLoaded);

		// Restart redrawing to avoid flickering
		SendMessage( pthX->m_hWnd,WM_SETREDRAW,TRUE,0);

		UpdateWindow( pthX->m_hWnd );

		pthX->_isRefreshing = FALSE;

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
	HRESULT hr;

	*phWnd = NULL;

	// Init member variables.
	m_spShellBrowser = psb;
	m_spShellBrowser->AddRef();
	m_spShellBrowser->GetWindow(&m_hwndParent);
	m_spShellBrowser->SetStatusTextSB(::MyLoadString(IDS_ProductIntro));

	m_FolderSettings = *lpfs;

	DWORD dwListStyles = WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD dwListExStyles = WS_EX_CLIENTEDGE | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;

	*phWnd = NULL;
	m_hWnd = CreateWindowEx ( dwListExStyles,WC_STATIC, NULL, dwListStyles,0, 0,
		prcView->right - prcView->left,prcView->bottom - prcView->top,
		m_hwndParent, NULL, g_hInst, 0 );

	if ( NULL == m_hWnd )
		return -1;

	hr = CoCreateInstance(CLSID_ExplorerBrowser, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_peb));
	if (SUCCEEDED(hr))
	{
		IUnknown_SetSite(_peb, static_cast<IServiceProvider *>(this));

		hr = _peb->Initialize(m_hWnd,prcView,&m_FolderSettings);
		if (SUCCEEDED(hr))
		{
			// initialized
			_peb->SetOptions(EBO_ALWAYSNAVIGATE | EBO_SHOWFRAMES);
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

				IFolderView2 *pfv2;
				hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
				if (SUCCEEDED(hr))
				{					
					hr = pfv2->GetFolder(IID_PPV_ARGS(&_prf));
					if (SUCCEEDED(hr))
					{
						FillList();
					}

					if ( m_FolderSettings.ViewMode == FVM_DETAILS )
					{
						IColumnManager *pcm;
						hr = pfv2->QueryInterface(&pcm);
						if (SUCCEEDED(hr))
						{
							if( IsShowTag() )
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
							else
							{
								PROPERTYKEY rgkeys[] = {PKEY_ItemNameDisplay,PKEY_DateModified,PKEY_ItemTypeText,PKEY_Size,PKEY_ItemFolderPathDisplay};
								hr = pcm->SetColumns(rgkeys, ARRAYSIZE(rgkeys));
								if (SUCCEEDED(hr))
								{
									CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_NAME | CM_MASK_STATE | CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
									hr = pcm->GetColumnInfo(PKEY_ItemNameDisplay, &ci);
									if (SUCCEEDED(hr))
									{
										ci.dwState = CM_STATE_ALWAYSVISIBLE;
										ci.uWidth = 250;
										ci.uDefaultWidth = 250;
										ci.uIdealWidth = 250;
										pcm->SetColumnInfo(PKEY_ItemNameDisplay, &ci);
									}

									CM_COLUMNINFO ci2 = {sizeof(ci2), CM_MASK_NAME | CM_MASK_STATE | CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
									hr = pcm->GetColumnInfo(PKEY_DateModified, &ci2);
									if (SUCCEEDED(hr))
									{
										ci2.dwState = CM_STATE_VISIBLE;
										ci2.uWidth = 180;
										ci2.uDefaultWidth = 180;
										ci2.uIdealWidth = 180;
										pcm->SetColumnInfo(PKEY_DateModified, &ci2);
									}

									CM_COLUMNINFO ci3 = {sizeof(ci2), CM_MASK_NAME | CM_MASK_STATE | CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
									hr = pcm->GetColumnInfo(PKEY_ItemFolderPathDisplay, &ci2);
									if (SUCCEEDED(hr))
									{
										ci2.dwState = CM_STATE_VISIBLE;
										ci2.uWidth = 350;
										ci2.uDefaultWidth = 350;
										ci2.uIdealWidth = 350;
										pcm->SetColumnInfo(PKEY_ItemFolderPathDisplay, &ci2);
									}
								}
								pcm->Release();
							}
						}
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

STDMETHODIMP 	CShellViewImpl::AddPropertySheetPages(DWORD dwReserved, LPFNADDPROPSHEETPAGE lpfn, LPARAM lparam)
{ return E_NOTIMPL; }
STDMETHODIMP 	CShellViewImpl::EnableModeless(BOOL fEnable)
{ return E_NOTIMPL; }
STDMETHODIMP 	CShellViewImpl::GetItemObject(UINT uItem, REFIID riid, void** ppv)
{ return E_NOTIMPL; }
STDMETHODIMP 	CShellViewImpl::SaveViewState()
{ return E_NOTIMPL; }
STDMETHODIMP 	CShellViewImpl::SelectItem(LPCITEMIDLIST pidlItem, UINT uFlags)
{ return E_NOTIMPL; }
STDMETHODIMP 	CShellViewImpl::TranslateAccelerator(LPMSG lpmsg)
{ return E_NOTIMPL; }


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
	return S_OK;
}

BOOL CShellViewImpl::IsShowTag()
{
	if ( NULL != m_psfContainingFolder && NULL != m_psfContainingFolder->m_PIDLCurrent )
	{
		return FALSE;
	}
	return TRUE;
}

// FillList() populates the list control.
void CShellViewImpl::FillList()
{
	if( !_isRefreshing )
		_beginthreadex(0,0,FillList_Asyn,this,0,0);
}
