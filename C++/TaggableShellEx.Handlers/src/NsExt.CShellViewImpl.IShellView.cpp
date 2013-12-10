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
	// show tags in a folder way while not like the desktop
	m_folderSettings.fFlags = FWF_AUTOARRANGE | FWF_NOWEBVIEW | FWF_NOHEADERINALLVIEWS;

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
	//RECT rc;
	//GetWindowRect(m_hWnd, &rc);
	//MapWindowRect(HWND_DESKTOP, m_hwndParent, &rc);

	//hr = SHCoCreateInstance(NULL, &CLSID_ExplorerBrowser, NULL, IID_PPV_ARGS(&_peb));
	hr = CoCreateInstance(CLSID_ExplorerBrowser, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_peb));
	if (SUCCEEDED(hr))
	{
		hr = IUnknown_SetSite(_peb, static_cast<IServiceProvider *>(this));

		// initialized
		_peb->SetOptions( EBO_SHOWFRAMES | EBO_NAVIGATEONCE);	// EBO_SHOWFRAMES
		_peb->SetEmptyText(::MyLoadString(IDS_MSG_LOADING_TAGS));

		// In Win7, the command bar will not request commands from a shell folder (via 
		// IShellFolder::CreateViewObject) unless the shell view proffers itself to the 
		// browser that hosts it. This is a change from Vista.
		IProfferService *pps;
		if (SUCCEEDED(m_spShellBrowser->QueryInterface(IID_PPV_ARGS(&pps))))
		{
			hr = pps->ProfferService(SID_SFolderView, static_cast<IServiceProvider *>(this),&_dwCookie);
			pps->Release();
		}

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
						FillList(_prf);
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

	if( NULL != _peb )
	{
        IUnknown_SetSite(_peb, NULL);
		_peb->Destroy();
		_peb = NULL;
	}

	if ( NULL != _prf )
	{
		_prf->RemoveAll();
		_prf->Release();
		_prf = NULL;
	}

	if ( NULL != m_spShellBrowser )
	{
		IProfferService *pps;
		if (SUCCEEDED(m_spShellBrowser->QueryInterface(IID_PPV_ARGS(&pps))))
		{
			auto hr = pps->RevokeService(_dwCookie);
			pps->Release();
		}
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

	_ASSERT_EXPR( _prf != NULL, L"_prf could not be NULL.");

	FillList(_prf);

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
		//MessageBox(hwnd,L"PageCallbackProc_ShellViewImpl",L"PSPCB_RELEASE",MB_OK);		
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
	psp.pszTemplate = MAKEINTRESOURCE(IDD_SETTINGS);
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

STDMETHODIMP CShellViewImpl::EnableModeless(BOOL fEnable)
{ 
	return m_spShellBrowser->EnableModelessSB(fEnable);
}
STDMETHODIMP CShellViewImpl::GetItemObject(UINT uItem, REFIID riid, void** ppv)
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
STDMETHODIMP CShellViewImpl::SaveViewState()
{
	// seems take no effect...
	m_spShellBrowser->GetViewStateStream(STGM_WRITE,&m_pViewState);
	return E_NOTIMPL;
}
STDMETHODIMP CShellViewImpl::SelectItem(LPCITEMIDLIST pidlItem, UINT uFlags)
{
	return E_NOTIMPL; 
}
STDMETHODIMP CShellViewImpl::TranslateAccelerator(LPMSG lpmsg)
{ 	
	::PrintLog(L"CShellViewImpl::TranslateAccelerator");
	return E_NOTIMPL; 
}


#ifdef IMPL_IShellFolderViewCB
// define some undocumented messages. See "shlext.h" from Henk Devos & Andrew Le Bihan, at http://www.whirlingdervishes.com/nselib/public
#define SFVCB_SELECTIONCHANGED    0x0008
#define SFVCB_DRAWMENUITEM        0x0009
#define SFVCB_MEASUREMENUITEM     0x000A
#define SFVCB_EXITMENULOOP        0x000B
#define SFVCB_VIEWRELEASE         0x000C
#define SFVCB_GETNAMELENGTH       0x000D
#define SFVCB_WINDOWCLOSING       0x0010
#define SFVCB_LISTREFRESHED       0x0011
#define SFVCB_WINDOWFOCUSED       0x0012
#define SFVCB_REGISTERCOPYHOOK    0x0014
#define SFVCB_COPYHOOKCALLBACK    0x0015
#define SFVCB_ADDINGOBJECT        0x001D
#define SFVCB_REMOVINGOBJECT      0x001E

#define SFVCB_GETCOMMANDDIR       0x0021
#define SFVCB_GETCOLUMNSTREAM     0x0022
#define SFVCB_CANSELECTALL        0x0023
#define SFVCB_ISSTRICTREFRESH     0x0025
#define SFVCB_ISCHILDOBJECT       0x0026
#define SFVCB_GETEXTVIEWS         0x0028
#define SFVCB_WNDMAIN              46
#define SFVCB_COLUMNCLICK2   0x32

STDMETHODIMP CShellViewImpl::MessageSFVCB(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	::PrintLog(L"CShellViewImpl::MessageSFVCB: 0x%X,0x%X,0x%X",uMsg,wParam,lParam);

	HRESULT hr = E_NOTIMPL;
	switch (uMsg)
	{
	case SFVM_INVOKECOMMAND:
		{
			UINT idCmd = wParam;
		}
		break;
	default:
		//hr = SHShellFolderView_Message(m_hwndParent, uMsg, lParam);
		//bResult = ProcessWindowMessage(NULL, uMsg, wParam, lParam, lResult, 0);
		break;
	}
	return hr;
}
#endif