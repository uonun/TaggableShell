#pragma once
#include "../include/dllmain.h"
#include "../include/TaggableShellEx.Taghelper.h"
#include "../include/TaggableShellEx.NsExt.CShellFolderImpl.h"
#include "../include/TaggableShellEx.NsExt.CShellViewImpl.h"

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
		QITABENT(CHandler, IEnumIDList),
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

	auto c = &__uuidof(CShellFolderImpl);
	pClassID->Data1 = c->Data1;
	pClassID->Data2 = c->Data2;
	pClassID->Data3 = c->Data3;
	memcpy(pClassID->Data4,c->Data4,sizeof(pClassID->Data4));
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
	::PrintLog(L"ShellFolder::BindToObject: riid = %X-%X-%X-%X",riid.Data1,riid.Data2,riid.Data3,riid.Data4);

	HRESULT hr = S_FALSE;

	IShellFolder *pShellFolder = NULL;
	hr = CoCreateInstance(__uuidof(CShellFolderImpl),NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellFolder));
	if( hr == S_OK )
	{
		pShellFolder->AddRef();
		hr = ((CShellFolderImpl*)pShellFolder)->_init ( this, pidl );

		hr =pShellFolder->QueryInterface(riid,ppvOut);
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
	::PrintLog(L"ShellFolder::CreateViewObject: riid = %X-%X-%X-%X",riid.Data1,riid.Data2,riid.Data3,riid.Data4);

	HRESULT hr = S_FALSE;

	IShellView* pShellView = NULL;
	*ppv = NULL;
	// Create a new ShellViewImpl COM object.
	if ( IID_IShellView == riid )
	{
		hr = CoCreateInstance(__uuidof(CShellViewImpl), NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellView)); 

		// Object initialization - pass the object its containing folder (this).
		hr = ((CShellViewImpl*)pShellView)->_init ( this );
	}
	else
	{
		hr = CoCreateInstance(riid, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellView)); 
	}

	if ( FAILED(hr) )
		return hr;

	// AddRef() the object while we're using it.
	//pShellView->AddRef();

	if ( FAILED(hr) )
	{
		pShellView->Release();
		return hr;
	}

	// Return the requested interface back to the shell.
	hr = pShellView->QueryInterface ( riid, ppv );
	pShellView->Release();

	return hr;
}

HRESULT CShellFolderImpl::EnumObjects(
	HWND hwndOwner,
	SHCONTF grfFlags,
	IEnumIDList **ppenumIDList
	)
{
	ppenumIDList = NULL;
	return S_OK;
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


// IEnumIDList
#pragma region IEnumIDList
HRESULT CShellFolderImpl::Clone(
	IEnumIDList **ppenum
	)
{
	return S_OK;
}


HRESULT CShellFolderImpl::Next(
	ULONG celt,
	LPITEMIDLIST *rgelt,
	ULONG *pceltFetched
	)
{
	return S_OK;
}


HRESULT CShellFolderImpl::Skip(
	ULONG celt
	)
{
	return S_OK;
}

#pragma endregion
