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
			hr = ((CShellFolderImpl*)pShellFolder)->Init ( m_pIDFolder,(PIDLIST_RELATIVE) pidl );

			wchar_t tmp[MAX_PATH];
			SHGetPathFromIDList(m_pIDFolder,tmp);

			auto data = m_PidlMgr.GetData(pidl);
			::PrintLog(L"ShellFolder::BindToObject: Got object: %s, m_pIDFolder = %s",data->wszDisplayName,tmp);

			if ( FAILED(hr) )
			{
				pShellFolder->Release();
				return hr;
			}

			hr = pShellFolder->QueryInterface(riid,ppvOut);
			pShellFolder->Release();
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

	*ppv = NULL;
	HRESULT hr = E_NOINTERFACE;
	IShellView* pShellView = NULL;

	// Create a new ShellViewImpl COM object.
	if ( IID_IShellView == riid )
	{
		::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_IShellView");

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
	MYPIDLDATA* data = NULL;
	*ppEnumIDList = NULL;

	// create a new instance, but released by caller.
	CEnumIDListImpl *pEnum = new CEnumIDListImpl();

	// make sure the tag could not be expanded in the tree on the left side.
	//
	// the value of grfFlags will be:
	// expand:
	//		expand Folder: SHCONTF_NAVIGATION_ENUM | SHCONTF_INCLUDEHIDDEN | SHCONTF_FOLDERS
	//		expand Tag: SHCONTF_NAVIGATION_ENUM | SHCONTF_INCLUDEHIDDEN | SHCONTF_FOLDERS
	//
	// double click on the icon in IExporerBrowser:
	//		SHCONTF_FOLDERS or SHCONTF_NONFOLDERS for the first time, see CShellViewImpl::FillList();
	//		SHCONTF_ENABLE_ASYNC | SHCONTF_FASTITEMS | SHCONTF_NONFOLDERS | SHCONTF_INIT_ON_FIRST_NEXT for the second time.
	//
	// list the tags/files only initialized or open from IExplorerBrowser, see the comment on _initialized.
	if ( _initialized || 0 == (grfFlags & SHCONTF_NAVIGATION_ENUM)){
		if ( grfFlags & SHCONTF_NONFOLDERS )
		{
			data = m_PidlMgr.GetData(m_PIDLCurrent);
			if ( data != NULL && data->Type == MYSHITEMTYPE_TAG)
			{
				// if MYPIDLDATA exists, then show files in tag.
				isShowFilesInTag = TRUE;
			}
		}

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
				dwAttribs |= SFGAO_NONENUMERATED | SFGAO_FILESYSANCESTOR | SFGAO_STORAGE | SFGAO_BROWSABLE | SFGAO_GHOSTED | SFGAO_HASPROPSHEET | SFGAO_CANRENAME;

				// set SFGAO_FOLDER will let the tag could be expand, and response clicks. but also lead to a bug that expanding in a loopping way.
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

	auto data =  m_PidlMgr.GetData ( pidl );
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
		::PrintLog(L"ShellFolder::GetUIObjectOf: riid = IID_IContextMenu");
		IContextMenu *pcm = NULL;
		hr = CoCreateInstance(__uuidof(CShellFolderImpl),NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pcm));
		if( hr == S_OK )
		{
			hr = ((CShellFolderImpl*)pcm)->Init ( m_pIDFolder,(PIDLIST_RELATIVE)*apidl );

			auto data = m_PidlMgr.GetData(*apidl);
			::PrintLog(L"ShellFolder::GetUIObjectOf: Got object: %s",data->wszDisplayName);

			if ( FAILED(hr) )
			{
				pcm->Release();
				return hr;
			}

			hr = pcm->QueryInterface(riid,ppv);
			pcm->Release();
		}
	}
	else if ( IID_IExtractIcon == riid )
	{
		::PrintLog(L"ShellFolder::GetUIObjectOf: riid = IID_IExtractIcon");

		//IExtractIcon *pxi;
		//IDefaultExtractIconInit *pdxi;
		//hr = SHCreateDefaultExtractIcon(IID_PPV_ARGS(&pdxi));
		//if (SUCCEEDED(hr) &&
		//	SUCCEEDED(hr = pdxi->SetFlags(GIL_PERCLASS)) &&
		//	//SUCCEEDED(hr = pdxi->SetKey(hkey)) &&   // optional
		//	SUCCEEDED(hr = pdxi->SetNormalIcon(g_DllFullName, 110)) &&
		//	SUCCEEDED(hr = pdxi->SetOpenIcon(NULL, SIID_FOLDEROPEN)) && // optional
		//	SUCCEEDED(hr = pdxi->SetDefaultIcon(NULL, SIID_FOLDER)) && // optional
		//	SUCCEEDED(hr = pdxi->SetShortcutIcon(g_DllFullName, 110))) // optional
		//{
		//	hr = pdxi->QueryInterface(IID_PPV_ARGS(&pxi));
		//	if (SUCCEEDED(hr))
		//		ppv = (void**)&pxi;
		//}

		//if (pdxi)
		//{
		//	pdxi->Release();
		//}

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

