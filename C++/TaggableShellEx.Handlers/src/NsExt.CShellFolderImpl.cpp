#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

CShellFolderImpl::CShellFolderImpl(void): 
	_cRef(1) // IUnknown
	,m_pIDFolder(NULL),m_PIDLCurrent(NULL)
	,_hSubmenu(NULL)
	,CurrentShellItemData(NULL)
	,pTagHelper(NULL)
	,m_mutex(NULL)
{
	::PrintLog(L"CShellFolderImpl.ctor");

	m_mutex = ::CreateMutex(NULL, FALSE, NULL);

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
		QITABENT(CShellFolderImpl, IQueryInfo),
		QITABENT(CShellFolderImpl, IExplorerPaneVisibility),
		QITABENT(CShellFolderImpl, IExplorerCommandProvider),
		QITABENT(CShellFolderImpl, IShellFolder),
		QITABENT(CShellFolderImpl, IPersistFolder),
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
	m_pIDFolder = pidl_perent;
	m_PIDLCurrent = pidl_current;
	CurrentShellItemData = m_PidlMgr.GetData(m_PIDLCurrent);

	return S_OK;
}

BOOL CShellFolderImpl::IsShowTag()
{
	if ( NULL != CurrentShellItemData )
	{
		//auto data = m_PidlMgr.GetData(m_psfContainingFolder->m_PIDLCurrent);
		//return data != NULL && data->Type == MYSHITEMTYPE_TAG ? TRUE : FALSE;
		return TRUE;
	}
	return FALSE;
}
