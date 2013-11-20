#pragma once
#include "..\include\NsExt.CShellViewImpl.h"
#include <Winuser.h> // GWLP_WNDPROC, GWLP_USERDATA

#define USE_ExplorerBrowser


// CreateViewWindow() creates the container window.  Once that window is
// created, it will create the list control.
STDMETHODIMP CShellViewImpl::CreateViewWindow ( LPSHELLVIEW pPrevView, 
											   LPCFOLDERSETTINGS lpfs,
											   LPSHELLBROWSER psb, 
											   LPRECT prcView, HWND* phWnd )
{
	HRESULT hr = S_FALSE;

	*phWnd = NULL;

	// Init member variables.
	m_spShellBrowser = psb;
	m_spShellBrowser->AddRef(); // release in ~ctor
	m_spShellBrowser->GetWindow(&m_hwndParent);
	m_spShellBrowser->SetStatusTextSB(::MyLoadString(IDS_ProductIntro));
	m_spShellBrowser->EnableModelessSB(TRUE);
	//m_spShellBrowser->GetViewStateStream(STGM_READ,&m_pViewState);
	m_folderSettings = *lpfs;
	m_folderSettings.fFlags = FWF_AUTOARRANGE | FWF_NOWEBVIEW | FWF_NOHEADERINALLVIEWS;	// show tags in a folder way while not like the desktop

#pragma region prepare window handler
	DWORD dwListStyles = WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD dwListExStyles = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT;

	if ( m_hWnd != NULL )
	{
		DestroyWindow(m_hWnd);
	}

	m_hWnd = CreateWindowEx ( dwListExStyles,WC_STATIC, NULL, dwListStyles,0, 0,
		prcView->right - prcView->left,prcView->bottom - prcView->top,
		m_hwndParent, NULL, g_hInst, 0 );

	if ( NULL == m_hWnd )
		return -1;
#pragma endregion

#ifdef USE_ExplorerBrowser
	//hr = SHCoCreateInstance(NULL, &CLSID_ExplorerBrowser, NULL, IID_PPV_ARGS(&_peb));
	hr = CoCreateInstance(CLSID_ExplorerBrowser, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_peb));
	if (SUCCEEDED(hr))
	{
		hr = IUnknown_SetSite(_peb, static_cast<IServiceProvider *>(this));

		// initialized
		_peb->SetOptions( EBO_SHOWFRAMES | EBO_ALWAYSNAVIGATE);	// EBO_SHOWFRAMES
		_peb->SetEmptyText(::MyLoadString(IDS_MSG_LOADING_TAGS));

		hr = _peb->Initialize(m_hWnd,prcView,&m_folderSettings);
		if (SUCCEEDED(hr))
		{
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
				spSV->Release();

				IFolderView2 *pfv2;
				hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
				if (SUCCEEDED(hr))
				{	
					InitExplorerBrowserColumns(pfv2);

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
#else
	*phWnd = hwndHolder;
#endif
	return hr;
}


// DestroyViewWindow() is responsible for destroying our windows & cleaning up stuff.
STDMETHODIMP CShellViewImpl::DestroyViewWindow()
{
	// Clean up the UI.
	UIActivate ( SVUIA_DEACTIVATE );

	if ( NULL != _prf )
	{
		_prf->RemoveAll();
		_prf->Release();
		_prf = NULL;
	}

	if( NULL != _peb )
	{
		_peb->Destroy();
		_peb = NULL;
	}

	return S_OK;
}


// GetCurrentInfo() returns our FODLERSETTINGS to the browser.
STDMETHODIMP CShellViewImpl::GetCurrentInfo ( LPFOLDERSETTINGS lpfs )
{
	*lpfs = m_folderSettings;
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
	if ( m_uUIState == uState )
		return S_OK;

	// Modify the Explorer menu and status bar.
	HandleActivate ( uState );

	return S_OK;
}

#pragma region AddPropertySheetPages
INT_PTR CALLBACK PageDlgProc_ShellViewImpl(     
	_In_  HWND hwnd,     
	_In_  UINT uMsg,     
	_In_  WPARAM wParam,     
	_In_  LPARAM lParam     
	)     
{
	BOOL bRet = FALSE;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		//bRet = OnInitDialog ( hwnd, lParam );
		break;

	case WM_NOTIFY:
		{
			NMHDR* phdr = (NMHDR*) lParam;

			switch ( phdr->code )
			{
			case PSN_APPLY:
				MessageBox(hwnd,L"Apply",L"Caption",MB_OK);
				break;
			}
		}
		break;
	}

	return bRet;
}

// http://msdn.microsoft.com/en-us/library/windows/desktop/bb760813(v=vs.85).aspx
UINT CALLBACK PageCallbackProc_ShellViewImpl(
	HWND hwnd,
	_In_     UINT uMsg,
	_Inout_  LPPROPSHEETPAGE ppsp
	)
{
	switch ( uMsg )
	{
	case PSPCB_RELEASE:
		CShellViewImpl* tmp = (CShellViewImpl*)ppsp->lParam;
		tmp->Release();
		MessageBox(hwnd,L"PSPCB_RELEASE",L"PSPCB_RELEASE",MB_OK);		
		break;
	}

	return 1;   // use nonzero let the page be created
}

// add pages to the Options property sheet from the View menu.
STDMETHODIMP CShellViewImpl::AddPropertySheetPages(DWORD dwReserved, LPFNADDPROPSHEETPAGE lpfn, LPARAM lparam)
{ 
	::PrintLog(L"CShellViewImpl::AddPropertySheetPages");

	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;

	// Set up the PROPSHEETPAGE struct.
	ZeroMemory ( &psp, sizeof(PROPSHEETPAGE) );

	psp.dwSize      = sizeof(PROPSHEETPAGE);
	psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE | PSP_DEFAULT | PSP_USECALLBACK;
	psp.hInstance   = ::g_hInst;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPERTYPAGE_FOLDER);
	psp.pszIcon     = 0;
	psp.pszTitle    = MAKEINTRESOURCE(IDS_DLG_PROPERTYPAGE_CAPTION);
	psp.pfnDlgProc  = PageDlgProc_ShellViewImpl;
	psp.lParam      = (LPARAM) this;
	psp.pfnCallback = PageCallbackProc_ShellViewImpl;
	psp.pcRefParent = (UINT*)&_cRef;

	// Create the page & get a handle.
	hPage = CreatePropertySheetPage ( &psp );

	if ( NULL != hPage )
	{
		// Call the shell's callback function, so it adds the page to
		// the property sheet.
		if ( lpfn ( hPage, lparam )){
			this->AddRef(); // released in the callback with the message PSPCB_RELEASE.
		}
		else
		{
			DestroyPropertySheetPage ( hPage );
		}
	}else{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}
#pragma endregion

STDMETHODIMP 	CShellViewImpl::EnableModeless(BOOL fEnable)
{ 
	return m_spShellBrowser->EnableModelessSB(fEnable);
}
STDMETHODIMP 	CShellViewImpl::GetItemObject(UINT uItem, REFIID riid, void** ppv)
{
	/* riid:
	{05B718B0-0000-0000-B01A-7B6225D90000}
	{05B718B0-0000-0000-0000-000000000000}
	{05D44A50-0000-0000-304A-D40500000000}
	{0047FB70-0000-0000-1406-A58128D90000}
	IID_IDispatch
	*/

	HRESULT hr = E_NOTIMPL;
	//IID_IContextMenu

	if ( IsEqualIID(IID_IDispatch, riid) )
	{
		// 
	}
	else
	{
		// 
	}

	hr = this->QueryInterface(riid,ppv);

	return hr;
}
STDMETHODIMP 	CShellViewImpl::SaveViewState()
{
	// seems take no effect...
	m_spShellBrowser->GetViewStateStream(STGM_WRITE,&m_pViewState);
	return E_NOTIMPL;
}
STDMETHODIMP 	CShellViewImpl::SelectItem(LPCITEMIDLIST pidlItem, UINT uFlags)
{
	return E_NOTIMPL; 
}
STDMETHODIMP 	CShellViewImpl::TranslateAccelerator(LPMSG lpmsg)
{ 	
	return E_NOTIMPL; 
}