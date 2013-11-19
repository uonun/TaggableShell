#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

HRESULT CShellFolderImpl::BindToObject(
	PCUIDLIST_RELATIVE pidl,
	IBindCtx *pbc,
	REFIID riid,
	void **ppvOut
	)
{
	HRESULT hr = E_NOINTERFACE;
	if ( riid == IID_IShellFolder )
	{
		::PrintLog(L"ShellFolder::BindToObject: riid = IID_IShellFolder");

		IShellFolder *pShellFolder = NULL;
		hr = CoCreateInstance(__uuidof(CShellFolderImpl),NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pShellFolder));
		if( hr == S_OK )
		{
			auto pidlFolder = ILClone(m_pIDFolder);
			auto pidlCurrent = ILClone(pidl);
			hr = ((CShellFolderImpl*)pShellFolder)->Init ( pidlFolder,(PIDLIST_RELATIVE)pidlCurrent );

			auto data = m_PidlMgr.GetData(pidl);
			::PrintLog(L"ShellFolder::BindToObject: Got object: %s, FolderPath = %s",data->wszDisplayName,FolderPath);

			if ( FAILED(hr) )
			{
				pShellFolder->Release();
				return hr;
			}
			else
			{
				hr = pShellFolder->QueryInterface(riid,ppvOut);
				pShellFolder->Release();
			}
		}
	}
#ifdef _DEBUG
	else
	{
		LPOLESTR str;
		StringFromIID(riid,&str);
		::PrintLog(L"ShellFolder::BindToObject: riid =%s",str);
		CoTaskMemFree(str);
	}
#endif

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
	if ( NULL == ppv )
		return E_POINTER;

	HRESULT hr = E_NOINTERFACE;

	// Create a new ShellViewImpl COM object.
	// A new view object must be created each time this method is called.
	if ( IID_IShellView == riid )
	{
		::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_IShellView");

		CShellViewImpl* v = NULL;
		hr = CoCreateInstance(__uuidof(CShellViewImpl), NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS((IShellView**)&v)); 

		if ( FAILED(hr) )
			return hr;

		// Object initialization - pass the object its containing folder (this).
		hr = ((CShellViewImpl*)v)->Init ( this );

		if ( FAILED(hr) )
		{
			v->Release();
			return hr;
		}

		// Return the requested interface back to the shell.
		hr = v->QueryInterface ( riid, ppv );
		v->Release();

	}
#ifdef _DEBUG
	else if ( IID_IDropTarget == riid)
	{
		::PrintLog(L"ShellFolder::CreateViewObject: Not implemented riid = IID_IDropTarget");
	}
	//else if ( IID_IConnectionFactory == riid)
	//{
	//	::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_IConnectionFactory");
	//}
	//else if ( IID_ITopViewAwareItem == riid)
	//{
	//	::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_ITopViewAwareItem");
	//}
	//else if ( IID_IFrameLayoutDefinitionFactory == riid)
	//{
	//	::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_IFrameLayoutDefinitionFactory");
	//}
	//else if ( IID_IFrameLayoutDefinition == riid)
	//{
	//	::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_IFrameLayoutDefinition");
	//}
	else
	{
		LPOLESTR str;
		StringFromIID(riid,&str);
		::PrintLog(L"ShellFolder::CreateViewObject: Not implemented riid =%s",str);
		CoTaskMemFree(str);
	}
#endif

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
	*ppEnumIDList = NULL;

	if ( CurrentShellItemData != NULL && CurrentShellItemData->Type == MYSHITEMTYPE_TAG)
	{
		// if MYPIDLDATA exists, then show files in tag.
		isShowFilesInTag = TRUE;
	}

	// create a new instance, but released by caller.
	CEnumIDListImpl *pEnum = new CEnumIDListImpl();
	// make sure the tag could not be expanded in the tree on the left side.
	//
	// the value of grfFlags will be:
	// expand:
	//		expand Folder:	0x10A0 = SHCONTF_NAVIGATION_ENUM | SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN
	//		expand Tag:		0x10A0 = SHCONTF_NAVIGATION_ENUM | SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN
	//		(the flag SHCONTF_INCLUDEHIDDEN depends on the settings of system.)
	//
	// double click on the icon in IExporerBrowser:
	//		0x0020 = SHCONTF_FOLDERS or 0x0040 = SHCONTF_NONFOLDERS for the first time, see CShellViewImpl::FillList();
	//		0xA0A0 = SHCONTF_ENABLE_ASYNC | SHCONTF_FASTITEMS | SHCONTF_NONFOLDERS | SHCONTF_INIT_ON_FIRST_NEXT for the second time.
	//
	// list the tags/files only initialized or open from IExplorerBrowser

	if (
		(NULL == CurrentShellItemData && (grfFlags & SHCONTF_NAVIGATION_ENUM))	// list the details when expanding the Folder.(CurrentShellItemData will not be NULL while expanding tags)
		|| 0 == (grfFlags & SHCONTF_NAVIGATION_ENUM	)							// list the details when opening from IExplorerBrowser
		)
	{

		WaitForSingleObject(m_mutex, INFINITE);

		items.clear();

		if ( this->pTagHelper->OpenDb() )
		{
			this->pTagHelper->LoadTags();
		}

		if ( isShowFilesInTag )
		{
			LPWSTR *files = new LPWSTR[MAXCOUNT_TOTAL_ITEMS];
			UINT count = 0;
			pTagHelper->GetFilesByTagID(files,count,CurrentShellItemData->TagID);
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
			for (UINT i = 0; i < pTagHelper->TagCount; i++)
			{
				MYPIDLDATA d = {sizeof(MYPIDLDATA)};
				d.cb = sizeof(MYPIDLDATA);
				d.Type = MYSHITEMTYPE_TAG;
				d.TagID = pTagHelper->Tags[i].TagID;
				d.TagIdx = pTagHelper->Tags[i].TagIdx;
				d.UseCount = pTagHelper->Tags[i].UseCount;
				StringCbCopyW(d.wszDisplayName,sizeof(d.wszDisplayName), pTagHelper->Tags[i].Tag);

				items.push_back(d);
			}
		}

		ReleaseMutex(m_mutex);
	}

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
				dwAttribs |= SFGAO_FILESYSANCESTOR | SFGAO_STORAGE | SFGAO_BROWSABLE | SFGAO_HASPROPSHEET;// | SFGAO_CANRENAME;

				// set SFGAO_FOLDER will let the tag could be expand, and response clicks. but also lead to a bug that expanding in a loopping way on the left tree.
				dwAttribs |= SFGAO_FOLDER;
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
	::PrintLog(L"ShellFolder::GetDisplayNameOf: uFlags = 0x%X",uFlags);
	/*
	SHGDN_NORMAL | SHGDN_FORPARSING
	Returns the fully qualified parsing name.

	SHGDN_INFOLDER | SHGDN_FORPARSING
	Returns the parsing name relative to the parent folder.

	SHGDN_INFOLDER | SHGDN_FOREDITING
	Returns the editing name relative to the parent folder.

	SHGDN_INFOLDER
	Returns the display name relative to the parent folder.

	SHGDN_NORMAL
	Returns the display name relative to the desktop.
	*/

	HRESULT hr = S_OK;

	auto data = m_PidlMgr.GetData ( pidl );
	if ( data != NULL )
	{
		pName->uType = STRRET_WSTR;
		pName->pOleStr = (LPWSTR)CoTaskMemAlloc(MAX_PATH);
		if ( data->Type == MYSHITEMTYPE_TAG )
		{
			// relative to the parent folder.
			if ( SHGDN_INFOLDER & uFlags )
			{
				StringCbCopy(pName->pOleStr,MAX_PATH,data->wszDisplayName);

				// show usecount in IExplorerBrowser
				if( !(uFlags & SHGDN_FOREDITING) && !(uFlags & SHGDN_FORADDRESSBAR) && !(uFlags & SHGDN_FORPARSING) )
				{
					StrCat(pName->pOleStr,L"(");
					wchar_t szUseCount[8]; // max 99999999 files for each tag.
					_itow_s(data->UseCount,szUseCount,10);
					StrCat(pName->pOleStr,szUseCount);
					StrCat(pName->pOleStr,L")");
				}
			}
			else
			{
				LPWSTR _path;
				HRESULT hr;
				IShellItem* si;
				hr = SHCreateShellItem(NULL,NULL,m_pIDFolder,&si);
				if ( SUCCEEDED(hr) )
				{
					si->GetDisplayName(SIGDN_DESKTOPABSOLUTEEDITING,&_path);
					StringCbCopy(pName->pOleStr,MAX_PATH,_path);
					CoTaskMemFree(_path);

					StrCat(pName->pOleStr,L"\\");
					StrCat(pName->pOleStr,data->wszDisplayName);
				}
			}

		}else{
			// relative to the parent folder.
			if ( SHGDN_INFOLDER & uFlags )
			{
				std::tr2::sys::wpath p(data->wszDisplayName);
				wstring itemname = p.filename();
				auto a = itemname.c_str();
				StringCbCopy(pName->pOleStr,MAX_PATH,a);
			}
			else
			{
				StringCbCopy(pName->pOleStr,MAX_PATH,data->wszDisplayName);
			}
		}
	}
	else
	{
		IShellFolder* psfParent;
		hr = SHBindToParent(m_pIDFolder,IID_PPV_ARGS(&psfParent),&pidl);
		if( SUCCEEDED(hr) )
		{
			psfParent->GetDisplayNameOf(pidl, uFlags, pName);
			psfParent->Release();
		}
	}

	return hr;
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
	HRESULT hr = E_NOINTERFACE;
	*ppv = NULL;

	if ( IID_IContextMenu == riid )
	{
		hr = this->QueryInterface(riid,ppv);
	}
	else if ( IID_IExtractIcon == riid )
	{
		hr = this->QueryInterface(riid,ppv);
	}
#ifdef _DEBUG
	else
	{
		LPOLESTR str;
		StringFromIID(riid,&str);
		::PrintLog(L"ShellFolder::GetUIObjectOf: riid =%s",str);
		CoTaskMemFree(str);
	}
#endif

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
	)
{	
	return S_OK;	
};

