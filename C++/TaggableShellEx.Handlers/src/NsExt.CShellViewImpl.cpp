#pragma once
#include "..\include\NsExt.CShellViewImpl.h"
#include <propkeydef.h>
#include <propkey.h>

DEFINE_GUID(IID_IBrowserSettings,     0xDD1E21CC, 0xE2C7, 0x402C, 0xBF,0x05, 0x10,0x32,0x8D,0x3F,0x6B,0xAD);

HANDLE CShellViewImpl::m_mutex = ::CreateMutex(NULL, FALSE, NULL);

CShellViewImpl::CShellViewImpl(void): 
	_cRef(1) // IUnknown
	,m_hwndParent(NULL)
	,m_hWnd(NULL)
	,m_spShellBrowser(NULL)
	,m_psfContainingFolder(NULL)
	, _peb(NULL),_prf(NULL)
	,m_isRefreshing(FALSE)
	,m_uUIState(SVUIA_DEACTIVATE)
	,m_hMenu(NULL)
	,_dwCookie(0)
	,_currentIconSize(48)
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

	if( NULL != m_mutex){
		::CloseHandle(m_mutex);
		m_mutex = NULL;
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
		QITABENT(CShellViewImpl, ICommDlgBrowser2),
		QITABENT(CShellViewImpl, ICommDlgBrowser3),
		QITABENT(CShellViewImpl, IOleWindow),
		QITABENT(CShellViewImpl, IFolderView),		
		QITABENT(CShellViewImpl, IFolderView2),
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
	---------------------------------------------------------------------------
	{IID_ICommDlgBrowser}							{IID_ICommDlgBrowser}
	................................................{IID_ICommDlgBrowser2}
	................................................{IID_ICommDlgBrowser3}
	---------------------------------------------------------------------------
	{IID_IFolderTypeModifier}						{IID_IFolderTypeModifier}
	{IID_IBrowserWithActivationNotification}		{IID_IBrowserWithActivationNotification}
	{IID_IWebBrowserApp}							{IID_IConnectionPointContainer}
	---------------------------------------------------------------------------
	{IID_IFolderView}								{IID_IShellSearchTarget}
	................................................{9365FF5E-C1F5-499E-B81F-586BFC067E48}
	---------------------------------------------------------------------------
	{IID_IFileDialogPrivate}						{IID_IFileDialog}
	................................................{IID_IFileDialogPrivate}
	---------------------------------------------------------------------------
	{SID_STopLevelBrowser}							{IID_ICommDlgBrowser2}
	................................................{IID_IShellBrowserService}
	................................................{IID_IShellBrowser}
	................................................{IID_IShellItemBrowser}
	................................................{IID_IConnectionPointContainer}
	................................................{IID_IProfferService}
	................................................{D3B1CAF5-EC4F-4B2E-BCB0-60D715C93CB2}
	---------------------------------------------------------------------------
	{IID_IShellTaskScheduler}						{IID_IShellTaskScheduler}
	................................................{3E24A11C-15B2-4F71-B81E-008F77998E9F}
	---------------------------------------------------------------------------
	{3934E4C2-8143-4E4C-A1DC-718F8563F337}			{3934E4C2-8143-4E4C-A1DC-718F8563F337}
	{E38FE0F3-3DB0-47EE-A314-25CF7F4BF521}			{E38FE0F3-3DB0-47EE-A314-25CF7F4BF521}
	{FAD451C2-AF58-4161-B9FF-57AFBBED0AD2}			{IID_IUnknown}
	{SID_ExplorerPaneVisibility}					{IID_IExplorerPaneVisibility}
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

	hr = QueryInterface(riid, ppv);		

	if( FAILED(hr) && NULL != _peb )
		hr = _peb->QueryInterface(riid,ppv);

	if( FAILED(hr) && NULL != m_spShellBrowser )
		hr = m_spShellBrowser->QueryInterface(riid,ppv);

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
HRESULT CShellViewImpl::GetFrameOptions(BROWSERFRAMEOPTIONS dwMask,BROWSERFRAMEOPTIONS *pdwOptions	)
{
	HRESULT hr = E_NOTIMPL;
	*pdwOptions = BFO_QUERY_ALL;
	hr = S_OK;
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

void CShellViewImpl::InitExplorerBrowserColumns(IFolderView2* pfv2)
{
	IColumnManager *pcm;
	HRESULT hr = pfv2->QueryInterface(&pcm);
	if (SUCCEEDED(hr))
	{
		if( IsShowTag() )
		{
			m_folderSettings.ViewMode = FVM_TILE;
			_peb->SetFolderSettings(&m_folderSettings);

			// columns for TAGs, PKEY_ItemNameDisplay is the Text of IShellItem, for columns, see: CShellFolderImpl::GetDetailsEx
			// PKEY_Comment for USECOUNT
			PROPERTYKEY rgkeys[] = {PKEY_ItemNameDisplay, PKEY_Comment};
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
				hr = pcm->GetColumnInfo(PKEY_Comment, &ci2);
				if (SUCCEEDED(hr))
				{
					ci2.dwState = CM_STATE_VISIBLE;
					ci2.uWidth = 50;
					ci2.uDefaultWidth = 50;
					ci2.uIdealWidth = 50;
					StringCbPrintf(ci2.wszName,sizeof(ci2.wszName),L"%s",::MyLoadString(IDS_DLG_TAGMANAGER_LV_TAGS_HEADER_USECOUNT) );
					pcm->SetColumnInfo(PKEY_Comment, &ci2);
				}
			}
		}
		else
		{
			m_folderSettings.ViewMode = FVM_DETAILS; //FVM_AUTO
			_peb->SetFolderSettings(&m_folderSettings);

			PROPERTYKEY rgkeys[] = {PKEY_ItemNameDisplay,PKEY_DateModified,PKEY_ItemTypeText,PKEY_Size,PKEY_ItemFolderPathDisplay};
			hr = pcm->SetColumns(rgkeys, ARRAYSIZE(rgkeys));
			if (SUCCEEDED(hr))
			{
				CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_NAME | CM_MASK_STATE | CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
				hr = pcm->GetColumnInfo(PKEY_ItemNameDisplay, &ci);
				if (SUCCEEDED(hr))
				{
					ci.dwState = CM_STATE_ALWAYSVISIBLE;
					ci.uWidth = 460;
					ci.uDefaultWidth = 460;
					ci.uIdealWidth = 460;
					pcm->SetColumnInfo(PKEY_ItemNameDisplay, &ci);
				}

				CM_COLUMNINFO ci2 = {sizeof(ci2), CM_MASK_NAME | CM_MASK_STATE | CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH};
				hr = pcm->GetColumnInfo(PKEY_DateModified, &ci2);
				if (SUCCEEDED(hr))
				{
					ci2.dwState = CM_STATE_VISIBLE;
					ci2.uWidth = 120;
					ci2.uDefaultWidth = 120;
					ci2.uIdealWidth = 120;
					pcm->SetColumnInfo(PKEY_DateModified, &ci2);
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
	if ( NULL != m_psfContainingFolder && m_psfContainingFolder->IsShowTag() )
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
	if( !m_isRefreshing )
	{
		if (SUCCEEDED(_peb->RemoveAll()))
		{
			m_isRefreshing = TRUE;
			CBackgroundThread<CShellViewImpl,IResultsFolder> *pfrobt = new (std::nothrow) CBackgroundThread<CShellViewImpl,IResultsFolder>(this);
			if (pfrobt)
			{
				pfrobt->StartThread(_prf);
				pfrobt->Release();
			}
		}
	}
}

HRESULT CShellViewImpl::DoWorkAsyn(IResultsFolder *prf)
{
	WaitForSingleObject(m_mutex, INFINITE);

	IEnumIDList *pEnum = NULL;

	try{

#pragma region do work
		LPWSTR infoLoaded = 0;
		BOOL isShowTag = this->IsShowTag();
		if ( isShowTag )
		{
			this->m_folderSettings.ViewMode = FVM_ICON;
			infoLoaded = ::MyLoadString(IDS_MSG_NO_TAG_WITH_DETAIL);
		}
		else
		{
			this->m_folderSettings.ViewMode = FVM_DETAILS;

			LPWSTR currentTag = L"";
			auto &data = this->m_psfContainingFolder->CurrentShellItemData;
			if ( NULL != data )
			{
				currentTag = data->wszDisplayName;
			}

			WCHAR tmp2[MAX_PATH];
			wsprintf ( tmp2,::MyLoadString(IDS_MSG_NO_FILE_IN_TAG_WITH_DETAIL),currentTag);
			infoLoaded = tmp2;
		}

		if( NULL == this->_peb )
			return S_FALSE;

		this->_peb->SetEmptyText(infoLoaded);

		// Stop redrawing to avoid flickering
		SendMessage( this->m_hWnd,WM_SETREDRAW,FALSE,0);

		LPITEMIDLIST pidl = NULL;
		HRESULT hr;

		DWORD flag = isShowTag ? SHCONTF_FOLDERS : SHCONTF_NONFOLDERS;
		hr = this->m_psfContainingFolder->EnumObjects ( this->m_hWnd, flag,&pEnum );
		if ( SUCCEEDED(hr) )
		{
			// Add items.
			DWORD dwFetched;
			int nLoaded = 0;		// total of loaded items, including nNotAvailable.
			int nNotAvailable = 0;	// total of loaded items which is FILE_NOT_FOUND or else..
			while ( pEnum->Next(1, &pidl, &dwFetched) == S_OK )	// the pidl is relative.
			{
				IShellItem *psi;
				auto data = this->m_PidlMgr.GetData(pidl);
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
						hr = SHCreateItemWithParent(this->m_psfContainingFolder->m_pIDFolder,NULL,pidl,IID_PPV_ARGS(&psi));
						wsprintf ( tmp3,::MyLoadString(IDS_MSG_LOADING_TAG),data->wszDisplayName);
					}


					_ASSERT_EXPR(NULL != this->m_spShellBrowser,L"CShellViewImpl::DoWorkAsyn: m_spShellBrowser could not be NULL.");

					this->m_spShellBrowser->SetStatusTextSB(tmp3);
				}
				else
				{
					hr = S_FALSE;
				}

				if (SUCCEEDED(hr))
				{
					_ASSERT_EXPR( NULL != prf , L"prf could not be NULL!");

					hr = prf->AddItem(psi);
					psi->Release();
					Sleep(0);
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
			this->m_spShellBrowser->SetStatusTextSB(tmp4);
		}else{
			this->m_spShellBrowser->SetStatusTextSB(L"No item.");
		}
#pragma endregion

	}
	catch(...)
	{
	}

	UpdateWindow( m_hWnd );
	SendMessage( this->m_hWnd,WM_SETREDRAW,TRUE,0);	// Restart redrawing to avoid flickering

	// the calling application must free the returned IEnumIDList object by calling its Release method.
	if ( NULL != pEnum )
		pEnum->Release();

	//pthX->HandleActivate(SVUIA_ACTIVATE_NOFOCUS);

	this->m_isRefreshing = FALSE;

	ReleaseMutex(m_mutex);

	return S_OK;          // the thread exit code, If the function succeeds, the return value is nonzero.
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
