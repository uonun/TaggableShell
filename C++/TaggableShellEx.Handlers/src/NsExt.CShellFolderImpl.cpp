#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

CShellFolderImpl::CShellFolderImpl(void): 
	_cRef(1) // IUnknown
	,m_pIDFolder(NULL),m_PIDLCurrent(NULL)
	,_hSubmenu(NULL)
	,CurrentShellItemData(NULL)
	,pTagHelper(NULL)
	,m_mutex(NULL)
	,m_hwndOwner(NULL)
{
	::PrintLog(L"CShellFolderImpl.ctor");

	m_mutex = ::CreateMutex(NULL, FALSE, NULL);

	StrCpy(FolderPath,L"");

	pTagHelper = CTaghelper::instance();

	DllAddRef();
}


CShellFolderImpl::~CShellFolderImpl(void)
{
	::PrintLog(L"CShellFolderImpl.~ctor");

	if ( NULL != _hSubmenu )
	{
		DestroyMenu ( _hSubmenu );
		_hSubmenu = NULL;
	}

	if( NULL != m_mutex){
		::CloseHandle(m_mutex);
		m_mutex = NULL;
	}

	DllRelease();
}

HRESULT CShellFolderImpl_CreateInstance(REFIID riid, void **ppv)
{
	::PrintLog(L"CShellFolderImpl_CreateInstance");
	CShellFolderImpl *pNew = new(std::nothrow) CShellFolderImpl;
	HRESULT hr = pNew ? S_OK : E_OUTOFMEMORY;
	if (pNew)
	{
		hr = pNew->QueryInterface(riid, ppv);
		pNew->Release();
	}
	return hr;
}


// IUnknown
IFACEMETHODIMP CShellFolderImpl::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CShellFolderImpl, IExtractIcon),
		QITABENT(CShellFolderImpl, IContextMenu),
		QITABENT(CShellFolderImpl, IContextMenuCB),
		QITABENT(CShellFolderImpl, IQueryInfo),
		QITABENT(CShellFolderImpl, IExplorerPaneVisibility),
		QITABENT(CShellFolderImpl, IExplorerCommandProvider),
		QITABENT(CShellFolderImpl, IShellFolder),
		QITABENT(CShellFolderImpl, IShellFolder2),
		//QITABENT(CShellFolderImpl, IPropertyStore),
		QITABENT(CShellFolderImpl, IPersistFolder),
		QITABENT(CShellFolderImpl, IPersistFolder2),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CShellFolderImpl::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CShellFolderImpl::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}


// Init function - call right after constructing a CShellFolderImpl object.
HRESULT CShellFolderImpl::Init ( PIDLIST_ABSOLUTE pidl_perent, PIDLIST_RELATIVE pidl_current )
{
	if( m_pIDFolder != NULL )
		ILFree(m_pIDFolder);

	if( m_PIDLCurrent != NULL )
		ILFree(m_PIDLCurrent);

	m_pIDFolder = pidl_perent;
	m_PIDLCurrent = pidl_current;
	CurrentShellItemData = m_PidlMgr.GetData(m_PIDLCurrent);

	return S_OK;
}

BOOL CShellFolderImpl::IsShowTagList()
{
	if ( NULL != CurrentShellItemData && CurrentShellItemData->Type == MYSHITEMTYPE_TAG)
	{
		return FALSE;
	}
	return TRUE;
}

HRESULT CShellFolderImpl::ExecuteMenuCommand(CExplorerCommandImpl *cmd,IShellItemArray* psiItemArray, IBindCtx* pbc)
{
	HRESULT hr = S_FALSE;
	switch (cmd->m_mii.wID)
	{
	case CMD_CMDBAR_NEWTAG:
		{
			hr = MessageBox(NULL,cmd->m_mii.dwTypeData,L"CMD",MB_OK);;
			break;
		}
	case CMD_CMDBAR_MODIFYTAG:
		{
			hr = MessageBox(NULL,cmd->m_mii.dwTypeData,L"CMD",MB_OK);;
			break;
		}
	case CMD_CMDBAR_DELTAG:
		{
			hr = MessageBox(NULL,cmd->m_mii.dwTypeData,L"CMD",MB_OK);;
			break;
		}
	case CMD_CMDBAR_SETTINGS:
		{
			auto _hdlg = CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_SETTINGS),m_hwndOwner,DlgProc_Settings);
			if(_hdlg != NULL){
				SetWindowText(_hdlg,::MyLoadString(IDS_ProductName));
				ShowWindow(_hdlg, SW_SHOW);
				UpdateWindow(_hdlg);
				SetWindowPos(_hdlg,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
				hr = S_OK;
			}
			break;
		}
	case CMD_CMDBAR_ABOUT:
		{
			auto _hdlg = CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_ABOUT),m_hwndOwner,DlgProc_About);
			if(_hdlg != NULL){
				AnimateWindow(_hdlg,200, AW_VER_POSITIVE);
				UpdateWindow(_hdlg);
				SetWindowPos(_hdlg,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
				hr = S_OK;
			}
			break;
		}
	default:
		//MessageBox(NULL,cmd->m_mii.dwTypeData,L"CMD",MB_OK);
		break;
	}
	return hr;
}