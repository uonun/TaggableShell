#pragma once
#include "..\include\NsExt.CShellViewImpl.h"
#include <propkey.h>

CShellViewImpl::CShellViewImpl(void): 
	_cRef(1) // IUnknown
	,m_hwndParent(NULL)
	,m_hWnd(NULL)
	,m_spShellBrowser(NULL)
	,m_psfContainingFolder(NULL)
	, _peb(NULL),_prf(NULL)
	,_isRefreshing(FALSE)
	,m_uUIState(SVUIA_DEACTIVATE)
{
	::PrintLog(L"CShellViewImpl.ctor");

	DllAddRef();
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

	if ( NULL != _peb )
	{
		_peb->Release();
		_peb = NULL;
	}

	DllRelease();
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
		QITABENT(CShellViewImpl, IFolderView),		
		QITABENT(CShellViewImpl, IBrowserFrameOptions),		
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
	/*
	guidService										riid
	---------------------------------------------------------------------------
	{CLSID_Desktop}									{IID_IUnknown}
	{IID_ILayoutModifier}							{IID_ILayoutModifier}
	{IID_IBrowserSettings}							{IID_IBrowserSettings}
	{IID_ICommDlgBrowser}							{IID_ICommDlgBrowser},{IID_ICommDlgBrowser2}
	{IID_IFolderTypeModifier}						{IID_IFolderTypeModifier}
	{IID_IBrowserWithActivationNotification}		{IID_IBrowserWithActivationNotification}
	{IID_IWebBrowserApp}							{IID_IConnectionPointContainer}
	{IID_IFolderView}								{IID_IShellSearchTarget}
	{IID_IFileDialogPrivate}						{IID_IFileDialog},{IID_IFileDialogPrivate}
	{SID_STopLevelBrowser}							{IID_ICommDlgBrowser2},{IID_IShellBrowserService},{IID_IShellBrowser},{IID_IShellItemBrowser},{IID_IConnectionPointContainer},{IID_IProfferService},{D3B1CAF5-EC4F-4B2E-BCB0-60D715C93CB2}
	{3934E4C2-8143-4E4C-A1DC-718F8563F337}			{3934E4C2-8143-4E4C-A1DC-718F8563F337}
	{6CCB7BE0-6807-11D0-B810-00C04FD706EC}			{6CCB7BE0-6807-11D0-B810-00C04FD706EC},{3E24A11C-15B2-4F71-B81E-008F77998E9F}
	{E38FE0F3-3DB0-47EE-A314-25CF7F4BF521}			{E38FE0F3-3DB0-47EE-A314-25CF7F4BF521}
	{FAD451C2-AF58-4161-B9FF-57AFBBED0AD2}			{IID_IUnknown}
	{E07010EC-BC17-44C0-97B0-46C7C95B9EDC}			{E07010EC-BC17-44C0-97B0-46C7C95B9EDC}
	{SID_DefView}									{IID_IUnknown}
	{D7F81F62-491F-49BC-891D-5665085DF969}			{IID_IDelayedVisibility}
	{DD1E21CC-E2C7-402C-BF05-10328D3F6BAD}			{DD1E21CC-E2C7-402C-BF05-10328D3F6BAD}
	*/

	HRESULT hr = E_NOINTERFACE;
	*ppv = NULL;

#ifdef _DEBUG
	LPOLESTR str,str2;
	StringFromIID(guidService,&str);
	StringFromIID(riid,&str2);
	::PrintLog(L"CShellViewImpl::QueryService: guidService = %s, riid = %s",str,str2);
	CoTaskMemFree(str);
	CoTaskMemFree(str2);
#endif

	if (SID_SExplorerBrowserFrame == guidService || IID_ICommDlgBrowser == guidService )
	{
		hr = QueryInterface(riid, ppv);		
	}
	else if ( SID_STopLevelBrowser == guidService )
	{
		if( NULL != _peb )
			hr = _peb->QueryInterface(riid,ppv);
	}
	else if ( SID_SFolderView == guidService ||  IID_IFolderView== guidService )
	{
		if( NULL != _peb )
			hr = _peb->QueryInterface(riid,ppv);
	}
	else if ( SID_DefView == guidService )
	{

	}
	return hr;
}

// IOleWindow
STDMETHODIMP CShellViewImpl::GetWindow ( HWND* phwnd )
{
	// Return our container window's handle to the browser.
	*phwnd = m_hWnd;
	return S_OK;
}

// IBrowserFrameOptions
HRESULT CShellViewImpl::GetFrameOptions(
	BROWSERFRAMEOPTIONS dwMask,
	BROWSERFRAMEOPTIONS *pdwOptions
	)
{
	HRESULT hr = E_NOTIMPL;
	//*pdwOptions = BFO_QUERY_ALL & dwMask;
	//hr = S_OK;
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

unsigned __stdcall CShellViewImpl::FillList_Asyn(void * pThis)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IEnumIDList *pEnum = NULL;
		CShellViewImpl * pthX = (CShellViewImpl*)pThis;
		pthX->AddRef();

		try{

#pragma region do work
			LPWSTR infoLoaded = 0;
			BOOL isShowTag = pthX->IsShowTag();
			if ( isShowTag )
			{
				pthX->m_FolderSettings.ViewMode = FVM_ICON;
				infoLoaded = ::MyLoadString(IDS_MSG_NO_TAG_WITH_DETAIL);
			}
			else
			{
				pthX->m_FolderSettings.ViewMode = FVM_DETAILS;

				LPWSTR currentTag = L"";
				auto &data = pthX->m_psfContainingFolder->CurrentShellItemData;
				if ( NULL != data )
				{
					currentTag = data->wszDisplayName;
				}

				WCHAR tmp2[MAX_PATH];
				wsprintf ( tmp2,::MyLoadString(IDS_MSG_NO_FILE_IN_TAG_WITH_DETAIL),currentTag);
				infoLoaded = tmp2;
			}

			pthX->_peb->SetEmptyText(infoLoaded);
			pthX->_peb->RemoveAll();

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
				int nLoaded = 0;		// total of loaded items, including nNotAvailable.
				int nNotAvailable = 0;	// total of loaded items which is FILE_NOT_FOUND or else..
				while ( pEnum->Next(1, &pidl, &dwFetched) == S_OK )	// the pidl is relative.
				{
					IShellItem *psi;
					auto data = pthX->m_PidlMgr.GetData(pidl);
					if ( data != NULL )
					{
						WCHAR tmp3[MAX_PATH];

						if ( data->Type == MYSHITEMTYPE_FILE )
						{
							hr = SHCreateItemFromParsingName(data->wszDisplayName,NULL,IID_PPV_ARGS(&psi));
							wsprintf ( tmp3,::MyLoadString(IDS_MSG_LOADING_FILE),data->wszDisplayName);
						}
						else
						{
							// TODO: there may be a bug(0x000000C5) that "sf" is not available.
							hr = SHCreateItemWithParent(sf->m_pIDFolder,NULL,pidl,IID_PPV_ARGS(&psi));
							wsprintf ( tmp3,::MyLoadString(IDS_MSG_LOADING_TAG),data->wszDisplayName);
						}

						pthX->m_spShellBrowser->SetStatusTextSB(tmp3);
					}
					else
					{
						hr = S_FALSE;
					}

					if (SUCCEEDED(hr))
					{
						hr = pthX->_prf->AddItem(psi);
						psi->Release();
					}
					else
					{
						// ERROR_FILE_NOT_FOUND
						// ERROR_PATH_NOT_FOUND
						// ERROR_INVALID_DRIVE
						// ....
						// HIDDEN
						nNotAvailable++;
					}

					nLoaded++;

					// free memory allocated by pEnum->Next
					CoTaskMemFree(pidl);
				}

				WCHAR tmp4[MAX_PATH];
				wsprintf ( tmp4,::MyLoadString(IDS_MSG_N_FILES_LOADED_FOR_TAG_WITH_NOT_FOUND),nLoaded,nNotAvailable);
				pthX->m_spShellBrowser->SetStatusTextSB(tmp4);
			}else{
				pthX->m_spShellBrowser->SetStatusTextSB(L"No item.");
			}
#pragma endregion

		}
		catch(...)
		{
		}

		UpdateWindow( pthX->m_hWnd );
		SendMessage( pthX->m_hWnd,WM_SETREDRAW,TRUE,0);	// Restart redrawing to avoid flickering

		// the calling application must free the returned IEnumIDList object by calling its Release method.
		if ( NULL != pEnum )
			pEnum->Release();

		//pthX->HandleActivate(SVUIA_ACTIVATE_NOFOCUS);

		pthX->_isRefreshing = FALSE;
		pthX->Release();

		CoUninitialize();
	}

	return 1;          // the thread exit code, If the function succeeds, the return value is nonzero.
}

void CShellViewImpl::InitExplorerBrowserColumns(IFolderView2* pfv2)
{
	IColumnManager *pcm;
	HRESULT hr = pfv2->QueryInterface(&pcm);
	if (SUCCEEDED(hr))
	{
		if( IsShowTag() )
		{
			m_FolderSettings.ViewMode = FVM_AUTO;
			_peb->SetFolderSettings(&m_FolderSettings);

			PROPERTYKEY rgkeys[] = {PKEY_ItemNameDisplay};
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
			}
		}
		else
		{
			m_FolderSettings.ViewMode = FVM_DETAILS;
			_peb->SetFolderSettings(&m_FolderSettings);

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
		}

		pcm->Release();
	}
}

HRESULT CShellViewImpl::Init ( CShellFolderImpl* pContainingFolder )
{
	m_psfContainingFolder = pContainingFolder;
	m_psfContainingFolder->AddRef();	// release in ~ctor
	return S_OK;
}

BOOL CShellViewImpl::IsShowTag()
{
	if ( NULL != m_psfContainingFolder && NULL != m_psfContainingFolder->CurrentShellItemData )
	{
		//auto data = m_PidlMgr.GetData(m_psfContainingFolder->m_PIDLCurrent);
		//return data != NULL && data->Type == MYSHITEMTYPE_TAG ? TRUE : FALSE;
		return FALSE;
	}
	return TRUE;
}

// FillList() populates the list control.
void CShellViewImpl::FillList()
{
	if( !_isRefreshing )
	{
		_isRefreshing = TRUE;
		_beginthreadex(0,0,FillList_Asyn,this,0,0);
	}
}

void CShellViewImpl::HandleActivate ( UINT uState )
{
	// Undo our previous changes to the menu.

	HandleDeactivate();

	// If we are being activated
	if ( SVUIA_DEACTIVATE != uState )
	{
		m_spShellBrowser->OnViewWindowActive(this);

		/*
		// First, create a new menu.
		if ( NULL != m_hMenu )
		{		
		DestroyMenu ( m_hMenu );    // also destroys the SimpleNSExt submenu
		m_hMenu = NULL;
		}

		m_hMenu = CreateMenu();
		if ( NULL != m_hMenu )
		{
		// Let the browser insert its standard items first.
		OLEMENUGROUPWIDTHS omw = { 0, 0, 0, 0, 0, 0 };
		m_spShellBrowser->InsertMenusSB ( m_hMenu, &omw );

		// Insert our SimpleExt menu before the Explorer Help menu.
		HMENU hmenuSimpleNS;
		hmenuSimpleNS = LoadMenu ( g_hInst,
		uState == SVUIA_ACTIVATE_FOCUS ?
		MAKEINTRESOURCE(IDR_MENU_Folder) :
		MAKEINTRESOURCE(IDR_MENU_Folder) );

		if ( NULL != hmenuSimpleNS )
		{
		InsertMenu ( m_hMenu, FCIDM_MENU_HELP, MF_BYCOMMAND | MF_POPUP,
		(UINT_PTR) GetSubMenu ( hmenuSimpleNS, 0 ),
		L"&SimpleNSExt" );
		}

		// Add an about item to Explorer's help menu.  We first need to get
		// the handle to the Help submenu.  We could do this by position,
		// using GetSubMenu(), since we can calculate its offset using the
		// numbers in the OLEMENUGROUPWIDTHS struct, but since we know the
		// command ID, that's less error-prone to use.

		MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_SUBMENU };

		if ( GetMenuItemInfo ( m_hMenu, FCIDM_MENU_HELP, FALSE, &mii ))
		{
		InsertMenu ( mii.hSubMenu, -1, MF_BYPOSITION,
		(UINT_PTR)&L"Show the namespace extension's about box", L"About &SimpleNSExt" );
		}

		// If our UI state is active with the focus, we have a bit more
		// work to do.
		if ( SVUIA_ACTIVATE_FOCUS == uState )
		{
		// The Edit menu created by Explorer is empty, so we can 
		// nuke it.

		DeleteMenu ( m_hMenu, FCIDM_MENU_EDIT, MF_BYCOMMAND );
		}

		// Set the new menu.
		m_spShellBrowser->SetMenuSB ( m_hMenu, NULL, m_hWnd );
		}
		*/
	}

	// Save the current state
	m_uUIState = uState;
}

void CShellViewImpl::HandleDeactivate()
{
	if ( SVUIA_DEACTIVATE != m_uUIState )
	{
		/*
		if ( NULL != m_hMenu )
		{
		m_spShellBrowser->SetMenuSB ( NULL, NULL, NULL );
		m_spShellBrowser->RemoveMenusSB ( m_hMenu );

		DestroyMenu ( m_hMenu );    // also destroys the SimpleNSExt submenu
		m_hMenu = NULL;
		}*/

		m_uUIState = SVUIA_DEACTIVATE;
	}
}