#pragma once
#include "../include/dllmain.h"
#include "../include/TaggableShellEx.Taghelper.h"
#include "../include/TaggableShellEx.NsExt.CShellFolderImpl.h"
#include "../include/TaggableShellEx.NsExt.CShellViewImpl.h"
#include "../include/TaggableShellEx.NsExt.CEnumIDListImpl.h"

CShellFolderImpl::CShellFolderImpl(void): 
	_cRef(1) // IUnknown
	,m_pIDFolder(NULL)
	,_hdlg(NULL)

{
	::PrintLog(L"CShellFolderImpl.ctor");

	DllAddRef();
}


CShellFolderImpl::~CShellFolderImpl(void)
{
	::PrintLog(L"CShellFolderImpl.~ctor");

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
		QITABENT(CHandler, IShellFolder),
		QITABENT(CHandler, IPersistFolder),
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

// IPersistFolder
HRESULT CShellFolderImpl::Initialize(LPCITEMIDLIST pIDFolder)
{
	// If Initialize has already been called, release the old PIDL
	if(m_pIDFolder!=NULL){
		ILFree(m_pIDFolder);
		m_pIDFolder = NULL;
	}

	//Store the new PIDL.
	if(pIDFolder)
	{
		m_pIDFolder = ILClone(pIDFolder);
	}

	return S_OK;
}

// IPersist which is the base of IPersistFolder
HRESULT CShellFolderImpl::GetClassID(
	CLSID *pClassID
	)
{
	if ( NULL == pClassID )
		return E_POINTER;

	pClassID = (CLSID *)&__uuidof(CShellFolderImpl);

	return S_OK;
}

// IShellFolder
#pragma region IShellFolder
HRESULT CShellFolderImpl::BindToObject(
	PCUIDLIST_RELATIVE pidl,
	IBindCtx *pbc,
	REFIID riid,
	void **ppvOut
	)
{
#ifdef _DEBUG
	LPOLESTR str;
	StringFromIID(riid,&str);
	::PrintLog(L"ShellFolder::BindToObject: riid =%s",str);
	CoTaskMemFree(str);
#endif

	HRESULT hr = S_FALSE;

	IShellFolder *pShellFolder = NULL;
	hr = CoCreateInstance(__uuidof(CShellFolderImpl),NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellFolder));
	if( hr == S_OK )
	{
		hr = ((CShellFolderImpl*)pShellFolder)->_init ( this, pidl );

		if ( FAILED(hr) )
		{
			pShellFolder->Release();
			return hr;
		}

		pShellFolder->AddRef();

		hr = pShellFolder->QueryInterface(riid,ppvOut);
		pShellFolder->Release();
	}

	return hr;
}


HRESULT CShellFolderImpl::CreateViewObject(
	HWND hwndOwner,
	REFIID riid,
	void **ppv
	)
{
	/*
	riid = 
	{IID_IConnectionFactory}
	{IID_IDropTarget}
	{IID_IShellView}
	{IID_ITopViewAwareItem}
	{IID_IFrameLayoutDefinitionFactory}
	{IID_IFrameLayoutDefinition}
	*/
#ifdef _DEBUG
	LPOLESTR str;
	StringFromIID(riid,&str);
	::PrintLog(L"ShellFolder::CreateViewObject: riid =%s",str);
	CoTaskMemFree(str);
#endif

	if ( NULL == ppv )
		return E_POINTER;

	*ppv = NULL;
	HRESULT hr = S_FALSE;
	IShellView* pShellView = NULL;

	// Create a new ShellViewImpl COM object.
	if ( IID_IShellView == riid )
	{
		hr = CoCreateInstance(__uuidof(CShellViewImpl), NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellView)); 

		if ( FAILED(hr) )
			return hr;

		// Object initialization - pass the object its containing folder (this).
		hr = ((CShellViewImpl*)pShellView)->_init ( this );

		if ( FAILED(hr) )
		{
			pShellView->Release();
			return hr;
		}

		// AddRef() the object while we're using it.
		pShellView->AddRef();


		// Return the requested interface back to the shell.
		hr = pShellView->QueryInterface ( riid, ppv );
		pShellView->Release();
	}
	else
	{
		hr = CoCreateInstance(riid, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellView)); 
	}
	return hr;
}

HRESULT CShellFolderImpl::EnumObjects(
	HWND hwndOwner,
	SHCONTF grfFlags,
	IEnumIDList **ppEnumIDList
	)
{
	if ( NULL == ppEnumIDList )
		return E_POINTER;

	*ppEnumIDList = NULL;

	HRESULT hr;

	CEnumIDListImpl* pEnum = new CEnumIDListImpl();
	pEnum->AddRef();

	vector<MYPIDLDATA> items;
	items.clear();

	if ( this->TagHelper.OpenDb() )
	{
		this->TagHelper.LoadTags();
	}

	for (UINT i = 0; i < TagHelper.TagCount; i++)
	{
		MYPIDLDATA d = {sizeof(MYPIDLDATA)};
		d.cb = sizeof(MYPIDLDATA);
		d.TagIdx = TagHelper.Tags[i].TagIdx;
		StringCbCopyW(d.wszDisplayName, 
			sizeof(d.wszDisplayName), TagHelper.Tags[i].Tag);
		items.push_back(d);
	}

	pEnum->Init(items);

	// Return an IEnumIDList interface to the caller.
	hr = pEnum->QueryInterface ( IID_IEnumIDList, (void**) ppEnumIDList );
	pEnum->Release();

	return hr;
}

HRESULT CShellFolderImpl::GetAttributesOf(
	UINT cidl,
	LPCITEMIDLIST *apidl,
	SFGAOF *rgfInOut
	) 
{
	*rgfInOut = 0;
	return S_OK;
}

HRESULT CShellFolderImpl::GetDisplayNameOf(
	PCUITEMID_CHILD pidl,
	SHGDNF uFlags,
	STRRET *pName
	)
{
	return S_OK;
}

HRESULT CShellFolderImpl::GetUIObjectOf(
	HWND hwndOwner,
	UINT cidl,
	PCUITEMID_CHILD_ARRAY apidl,
	REFIID riid,
	UINT *rgfReserved,
	void **ppv
	)
{
	*ppv = NULL;
	return E_NOINTERFACE;
}

HRESULT CShellFolderImpl::ParseDisplayName(
	HWND hwnd,
	IBindCtx *pbc,
	LPWSTR pszDisplayName,
	ULONG *pchEaten,
	PIDLIST_RELATIVE *ppidl,
	ULONG *pdwAttributes
	)
{
	return S_OK;
}
#pragma endregion

