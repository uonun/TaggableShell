#include "..\include\TaggableShellEx.NsExt.CShellViewImpl.h"


CShellViewImpl::CShellViewImpl(void): 
	_cRef(1) // IUnknown
	,m_hwndParent(NULL)
	,m_hWnd(NULL)
	,m_spShellBrowser(NULL)
	,m_psfContainingFolder(NULL)
{
	::PrintLog(L"CShellViewImpl.ctor");
}


CShellViewImpl::~CShellViewImpl(void)
{
	::PrintLog(L"CShellViewImpl.ctor");

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
		QITABENT(CHandler, IShellView),
		QITABENT(CHandler, IOleWindow),
		QITABENT(CHandler, IOleCommandTarget),
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


STDMETHODIMP CShellViewImpl::GetWindow ( HWND* phwnd )
{
	// Return our container window's handle to the browser.
	*phwnd = m_hWnd;
	return S_OK;
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
	//m_spShellBrowser->AddRef();
	m_spShellBrowser->GetWindow(&m_hwndParent);

	m_FolderSettings = *lpfs;


	DWORD dwListStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER |
		LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS;
	DWORD dwListExStyles = WS_EX_CLIENTEDGE;
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
	m_hWnd = CreateWindowEx ( dwListExStyles, WC_LISTVIEW, NULL, dwListStyles,0, 0, 0, 0, m_hwndParent, NULL, g_hInst, 0 );

	if ( NULL == m_hWnd )
		return -1;

	UpdateWindow (m_hWnd) ;

	*phWnd = m_hWnd;
	return S_OK;
}


// DestroyViewWindow() is responsible for destroying our windows & cleaning
// up stuff.
STDMETHODIMP CShellViewImpl::DestroyViewWindow()
{
	// Clean up the UI.

	UIActivate ( SVUIA_DEACTIVATE );

	// Destroy our windows & other resources.

	//if ( NULL != m_hMenu )
	//   DestroyMenu ( m_hMenu );

	//m_wndList.DeleteAllItems();
	//m_wndList.DestroyWindow();
	DestroyWindow(m_hWnd);

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

	//m_wndList.DeleteAllItems();
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
