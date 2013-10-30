#pragma once
#include "../include/TaggableShellEx.NsExt.CShellFolderImpl.h"

CShellFolderImpl::CShellFolderImpl(void): 
	_cRef(1) // IUnknown
	,m_pIDFolder(NULL),m_PIDLCurrent(NULL)
	,_expanded_in_tree(FALSE)
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
		QITABENT(CShellFolderImpl, IQueryInfo),
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

// IPersistFolder
HRESULT CShellFolderImpl::Initialize(LPCITEMIDLIST pIDFolder)
{
	::PrintLog(L"CShellFolderImpl::Initialize");

	// If Initialize has already been called, release the old PIDL
	if(m_pIDFolder!=NULL){
		ILFree(m_pIDFolder);
		m_pIDFolder = NULL;
	}

	//Store the new PIDL.
	if(pIDFolder)
	{
		m_pIDFolder = ILClone(pIDFolder);

		HRESULT hr;
		IShellItem* si;
		hr = SHCreateShellItem(NULL,NULL,m_pIDFolder,&si);
		if ( SUCCEEDED(hr) )
		{
			LPWSTR path;
			si->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,&path);
			::PrintLog(L"Get in path: %s",path);
		}
		si->Release();
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

	HRESULT hr = E_NOINTERFACE;
	if ( riid == IID_IShellFolder )
	{
		IShellFolder *pShellFolder = NULL;
		hr = CoCreateInstance(__uuidof(CShellFolderImpl),NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellFolder));
		if( hr == S_OK )
		{
			hr = ((CShellFolderImpl*)pShellFolder)->Init ( m_pIDFolder,(PIDLIST_RELATIVE) pidl );

			auto data = m_PidlMgr.GetData(pidl);
			::PrintLog(L"ShellFolder::BindToObject: %s",data->wszDisplayName);

			if ( FAILED(hr) )
			{
				pShellFolder->Release();
				return hr;
			}

			hr = pShellFolder->QueryInterface(riid,ppvOut);
			pShellFolder->Release();
		}
	}

	return hr;
}

HRESULT CShellFolderImpl::BindToStorage(
	PCUIDLIST_RELATIVE pidl,
	IBindCtx *pbc,
	REFIID riid,
	void **ppvOut
	){	return S_OK;	};

HRESULT CShellFolderImpl::CompareIDs(
	LPARAM lParam,
	PCUIDLIST_RELATIVE pidl1,
	PCUIDLIST_RELATIVE pidl2
	){	return S_OK;	};

HRESULT CShellFolderImpl::CreateViewObject(
	HWND hwndOwner,
	REFIID riid,
	void **ppv
	)
{
	// TODO: THERE MUST BE A BUG THAT IShellView* WILL NOT BE RELEASED AFTER THE SHELL FOLDER CLOSED!!
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
	HRESULT hr = E_NOINTERFACE;
	IShellView* pShellView = NULL;

	// Create a new ShellViewImpl COM object.
	if ( IID_IShellView == riid )
	{
		hr = CoCreateInstance(__uuidof(CShellViewImpl), NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellView)); 

		if ( FAILED(hr) )
			return hr;

		// Object initialization - pass the object its containing folder (this).
		hr = ((CShellViewImpl*)pShellView)->Init ( this );

		if ( FAILED(hr) )
		{
			pShellView->Release();
			return hr;
		}

		// Return the requested interface back to the shell.
		hr = pShellView->QueryInterface ( riid, ppv );
		pShellView->Release();
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

	HRESULT hr;
	vector<MYPIDLDATA> items;
	BOOL isShowFilesInTag = FALSE;
	MYPIDLDATA* data = NULL;
	*ppEnumIDList = NULL;

	// TODO: make sure the tag could not be expanded in the tree on the left side.
	// the value of grfFlags will be:
	// expand: SHCONTF_NAVIGATION_ENUM | SHCONTF_INCLUDEHIDDEN | SHCONTF_FOLDERS
	// double click on the icon in ExporerBrowser: SHCONTF_FOLDERS or SHCONTF_NONFOLDERS, see CShellViewImpl::FillList();
	// see more: CShellFolderImpl::GetAttributesOf
	if ( grfFlags & SHCONTF_NONFOLDERS )
	{
		data = m_PidlMgr.GetData(m_PIDLCurrent);
		if ( data != NULL && data->Type == MYSHITEMTYPE_TAG)
		{
			// if MYPIDLDATA exists, then show files in tag.
			isShowFilesInTag = TRUE;
		}
	}

	// create a new instance, but released by caller.
	CEnumIDListImpl *pEnum = new CEnumIDListImpl();

	if ( this->TagHelper.OpenDb() )
	{
		this->TagHelper.LoadTags();
	}

	GlobalLock((HGLOBAL)pEnum);

	if ( isShowFilesInTag )
	{
		LPWSTR *files = new LPWSTR[MAXCOUNT_TOTAL_ITEMS];
		UINT count = 0;
		TagHelper.GetFilesByTagID(files,count,data->TagID);
		for (UINT i = 0; i < count; i++)
		{
			MYPIDLDATA d = {sizeof(MYPIDLDATA)};
			d.cb = sizeof(MYPIDLDATA);
			d.Type = MYSHITEMTYPE_FILE;
			StringCbCopyW(d.wszDisplayName,sizeof(d.wszDisplayName), files[i]);

			items.push_back(d);
		}
		delete [] files;
	}
	else
	{
		// TODO: make sure the tag could not be expanded in the tree on the left side.
		for (UINT i = 0; i < TagHelper.TagCount; i++)
		{
			MYPIDLDATA d = {sizeof(MYPIDLDATA)};
			d.cb = sizeof(MYPIDLDATA);
			d.Type = MYSHITEMTYPE_TAG;
			d.TagID = TagHelper.Tags[i].TagID;
			d.TagIdx = TagHelper.Tags[i].TagIdx;
			d.UseCount = TagHelper.Tags[i].UseCount;
			StringCbCopyW(d.wszDisplayName,sizeof(d.wszDisplayName), TagHelper.Tags[i].Tag);

			items.push_back(d);
		}
	}

	GlobalUnlock((HGLOBAL)pEnum);

	pEnum->Init(m_pIDFolder,items);

	// Return an IEnumIDList interface to the caller.
	hr = pEnum->QueryInterface ( IID_IEnumIDList, (void**) ppEnumIDList );
	pEnum->Release();

	return hr;
}

HRESULT CShellFolderImpl::GetAttributesOf(
	UINT uCount,
	LPCITEMIDLIST *aPidls,
	SFGAOF *pdwAttribs
	) 
{
	UINT  i;

	for(i = 0; i < uCount; i++)
	{
		DWORD dwAttribs = 0;

		auto data = m_PidlMgr.GetData(aPidls[i]);
		// Is this item a MYPIDLDATA?
		if(data != NULL)
		{
			if( data->Type = MYSHITEMTYPE_TAG )
			{
				dwAttribs |= SFGAO_NONENUMERATED | SFGAO_FILESYSANCESTOR | SFGAO_STORAGE | SFGAO_BROWSABLE | SFGAO_GHOSTED | SFGAO_HASPROPSHEET | SFGAO_CANRENAME;

				// set SFGAO_FOLDER will let the tag could be expand, and response clicks. but also lead to a bug that expanding in a loopping way.
				//dwAttribs |= SFGAO_FOLDER;
			}
			else
			{
				IShellItem *psi;
				HRESULT	hr = SHCreateItemFromParsingName(data->wszDisplayName,NULL,IID_PPV_ARGS(&psi));
				if( SUCCEEDED(hr) )
				{
					psi->GetAttributes( 0xFFFFFFFF,pdwAttribs);
				}
			}
		}
		else
		{
			IShellItem *psi;
			HRESULT hr = SHCreateShellItem(this->m_pIDFolder,NULL,aPidls[i],&psi);
			if( SUCCEEDED(hr) )
			{
				psi->GetAttributes( 0xFFFFFFFF,pdwAttribs);
			}
		}

		/*
		On entry, *pdwAttributes contains the attributes that are being
		requested, so just use it as a mask.
		*/ 
		*pdwAttribs &= dwAttribs;
	}
	return S_OK;
}

HRESULT CShellFolderImpl::GetDisplayNameOf(
	PCUITEMID_CHILD pidl,
	SHGDNF uFlags,
	STRRET *pName
	)
{
	::PrintLog(L"ShellFolder::GetDisplayNameOf");
	/*
	SIGDN_NORMALDISPLAY:				新建文本文档.txt
	SIGDN_PARENTRELATIVEPARSING:		新建文本文档.txt
	SIGDN_DESKTOPABSOLUTEPARSING:		E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\新建文本文档.txt
	SIGDN_PARENTRELATIVEEDITING:		新建文本文档.txt
	SIGDN_DESKTOPABSOLUTEEDITING:		E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\新建文本文档.txt
	SIGDN_FILESYSPATH:					E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\新建文本文档.txt
	SIGDN_URL:							file:///E:/Works/UDNZ/udnz.com.ShellEx.TaggableShell/C++/_Debug/x64/新建文本文档.txt
	SIGDN_PARENTRELATIVEFORADDRESSBAR:	新建文本文档.txt
	SIGDN_PARENTRELATIVE:				新建文本文档.txt
	SIGDN_PARENTRELATIVEFORUI:			新建文本文档.txt
	*/

	auto data =  m_PidlMgr.GetData ( pidl );
	pName->uType = STRRET_WSTR;
	pName->pOleStr = (LPWSTR) CoTaskMemAlloc(sizeof(data->wszDisplayName));

	StringCbCopy(pName->pOleStr,sizeof(data->wszDisplayName),data->wszDisplayName);

	::PrintLog(L"Got name: %s",pName->pOleStr);

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
#ifdef _DEBUG
	LPOLESTR str;
	StringFromIID(riid,&str);
	::PrintLog(L"ShellFolder::GetUIObjectOf: riid =%s",str);
	CoTaskMemFree(str);
#endif

	HRESULT hr = E_NOINTERFACE;
	*ppv = NULL;

	if ( IID_IContextMenu == riid )
	{
		hr = QueryInterface(riid,ppv);
	}
	else if ( IID_IExtractIcon == riid )
	{
		IExtractIcon *pxi;
		IDefaultExtractIconInit *pdxi;
		hr = SHCreateDefaultExtractIcon(IID_PPV_ARGS(&pdxi));
		if (SUCCEEDED(hr) &&
			SUCCEEDED(hr = pdxi->SetFlags(GIL_PERCLASS)) &&
			//SUCCEEDED(hr = pdxi->SetKey(hkey)) &&   // optional
			SUCCEEDED(hr = pdxi->SetNormalIcon(g_DllFullName, 110)) &&
			SUCCEEDED(hr = pdxi->SetOpenIcon(NULL, SIID_FOLDEROPEN)) && // optional
			SUCCEEDED(hr = pdxi->SetDefaultIcon(NULL, SIID_FOLDER)) && // optional
			SUCCEEDED(hr = pdxi->SetShortcutIcon(g_DllFullName, 110))) // optional
		{
			hr = pdxi->QueryInterface(IID_PPV_ARGS(&pxi));
			if (SUCCEEDED(hr))
				ppv = (void**)&pxi;
		}

		if (pdxi)
		{
			pdxi->Release();
		}

	}

	return hr;
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
	::PrintLog(L"ShellFolder::ParseDisplayName: %s",pszDisplayName);
	HRESULT hr = S_FALSE;

	hr = SHILCreateFromPath(pszDisplayName,ppidl,pdwAttributes);
	if ( SUCCEEDED(hr) )
	{
		ULONG n = sizeof(pszDisplayName) / sizeof(pszDisplayName[0]);
		pchEaten = &n;
	}
	return hr;
}

HRESULT CShellFolderImpl::SetNameOf(
	HWND hwndOwner,
	PCUITEMID_CHILD pidl,
	LPCWSTR pszName,
	SHGDNF uFlags,
	PITEMID_CHILD *ppidlOut
	){	return S_OK;	};

#pragma endregion

// IQueryInfo
HRESULT CShellFolderImpl::GetInfoTip(DWORD dwFlags, PWSTR *ppwszTip)
{
	ppwszTip = (PWSTR*)CoTaskMemAlloc(12);
	memcpy(ppwszTip,L"SDFSDF",12);
	return S_OK;
}
