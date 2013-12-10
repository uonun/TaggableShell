#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

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
	if (ILIsChild(pidl))
	{
		if (IsEqualIID(riid,IID_IShellFolder) || IsEqualIID(riid,IID_IShellFolder2))
		{
			::PrintLog(L"ShellFolder::BindToObject: riid = IID_IShellFolder, FolderPath = %s", FolderPath);

			// create a new instance of a CShellFolderImpl for a given child ID
			IShellFolder *psf;
			hr = CoCreateInstance(__uuidof(CShellFolderImpl),NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&psf));
			if( hr == S_OK )
			{
				auto data = m_PidlMgr.GetData(pidl);
				if( NULL != data )
				{
					// HACK: there may be a bug:
					//			when an other application which is explorering shell folders from MyComputer, the NSE will be listed as supposed to,
					//			but when open a physical directory which is in any TAG, the TAGs will be listed in the directory again while not the items in the directory.
					//			absolutely it is not right.
					// As a HACK way to avoid this bug, we make sure the FolderPath will not be empty.
					if ( FolderPath[0] != 0 )
					{
						auto pidlFolder = ILClone(m_pIDFolder);
						auto pidlCurrent = ILClone(pidl);
						hr = ((CShellFolderImpl*)psf)->Init( pidlFolder,(PIDLIST_RELATIVE)pidlCurrent );

						::PrintLog(L"ShellFolder::BindToObject: Got object: %s",data->wszDisplayName);

						hr = psf->QueryInterface(riid,ppvOut);
						psf->Release();

					}else{
						// TODO:
						// 1. get pidl of the specified physical directory.	( not able to finish this step right now.)
						// 2. get shell folder of Desktop by SHGetDesktopFolder
						// 3. pShellFolder->BindToObject(specified pidl,pbc,riid,ppvOut);
					}
				}else{
					// if the pidl is not a TAG(MYPIDLDATA), leave the hr = E_NOINTERFACE to allow the shell explorer as normal.
				}
			}
		}
	}
	else
	{
		IShellFolder* ppsf;
		PCUITEMID_CHILD pidlChild;
		hr = SHBindToFolderIDListParent(this, pidl, IID_PPV_ARGS(&ppsf), &pidlChild);
		if (SUCCEEDED(hr))
		{
			hr = ppsf->BindToObject(pidlChild, pbc, riid, ppvOut);
		}
	}

	return hr;
}

HRESULT CShellFolderImpl::BindToStorage(
	PCUIDLIST_RELATIVE pidl,
	IBindCtx *pbc,
	REFIID riid,
	void **ppvOut
	)
{
	return BindToObject(pidl, pbc, riid, ppvOut);
}

HRESULT CShellFolderImpl::CompareIDs(
	LPARAM lParam,
	PCUIDLIST_RELATIVE pidl1,
	PCUIDLIST_RELATIVE pidl2
	)
{
	short sResult;
	unsigned uSeverity = 0x000000000;	// Leave uSeverity = 0 if the order is successfully determined.

	auto data1 = m_PidlMgr.GetData(pidl1);
	auto data2 = m_PidlMgr.GetData(pidl2);
	if( data1 != NULL && data2 != NULL )
	{
		// Set sResult = -1 if pidl1 precedes pidl2 (pidl1 < pidl2).
		// Set sResult =  1 if pidl1 follows pidl2. (pidl1 > pidl2).
		// Set sResult =  0 if pidl1 and pidl2 are equivalent in terms of ordering. (pidl1 = pidl2).

		if ( data1->TagID == data2->TagID || data1->UseCount == data2->UseCount)
			sResult =  0 ;
		else if( data1->UseCount > data2->UseCount )
			sResult = -1;
		else
			sResult =  1;
	}
	else
	{
		// Set uSeverity = 0x00000001 if there is an error.
		uSeverity = 0x00000001;
	}

	return MAKE_HRESULT(uSeverity, 0, (unsigned short)sResult);
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
	{IID_IIdentityName}
	{IID_IShellLinkW}
	{IID_IExplorerCommandProvider}
	*/
	if ( NULL == ppv )
		return E_POINTER;

	HRESULT hr = E_NOINTERFACE;

	// Create a new ShellViewImpl COM object.
	if ( IsEqualIID(IID_IShellView,riid) )
	{
		::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_IShellView");

		if ( NULL != hwndOwner )
			m_hwndOwner = hwndOwner;

		// A new view object must be created each time this method is called.
		CShellViewImpl* v = NULL;
		hr = CoCreateInstance(__uuidof(CShellViewImpl), NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS((IShellView**)&v)); 

		if ( FAILED(hr) )
			return hr;

		// Object initialization - pass the object its containing folder (this).
		hr = ((CShellViewImpl*)v)->Init ( this );		
		this->m_currentView = v;

		if ( FAILED(hr) )
		{
			v->Release();
			return hr;
		}

		// Return the requested interface back to the shell.
		hr = v->QueryInterface ( riid, ppv );
		v->Release();
	}
	else if ( IsEqualIID(IID_IExplorerCommandProvider, riid))
	{
		::PrintLog(L"ShellFolder::CreateViewObject: riid = IID_IExplorerCommandProvider");
		hr = this->QueryInterface ( riid, ppv );
	}
	else if( riid == IID_IContextMenu )
	{
		DEFCONTEXTMENU dcm = { hwndOwner, static_cast<IContextMenuCB*>(this), m_pIDFolder, static_cast<IShellFolder*>(this), 0, NULL, NULL, 0, NULL };
		hr = ::SHCreateDefaultContextMenu(&dcm, riid, ppv);
	}
	//else if( riid == IID_IShellFolder )
	//{
	//	// Allows internal objects to query the C++ implementation of this IShellFolder instance.
	//	// This violates basic COM sense, but we need this as a programming shortcut. It restricts
	//	// our Shell Extension to only work as an in-proc component.
	//	AddRef(); 
	//	*ppv = this;
	//	return S_OK;
	//}
#ifdef _DEBUG
	//else if ( IsEqualIID(IID_IDropTarget, riid))
	//{
	//	::PrintLog(L"ShellFolder::CreateViewObject: Not implemented riid = IID_IDropTarget");
	//}
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
			this->pTagHelper->LoadTags(true);
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
				dwAttribs |= SFGAO_FILESYSANCESTOR | SFGAO_STORAGE | SFGAO_BROWSABLE | SFGAO_HASPROPSHEET | SFGAO_CANRENAME;

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

	if ( IsEqualIID(IID_IContextMenu,riid) || IsEqualIID(IID_IContextMenuCB,riid))
	{
		// 	CDefFolderMenu_Create2(m_pIDFolder,hwndOwner,cidl,apidl,static_cast<IShellFolder *>(this),CallBack,0,NULL,(IContextMenu**)ppv);

		DEFCONTEXTMENU dcm = { hwndOwner, static_cast<IContextMenuCB*>(this), m_pIDFolder, static_cast<IShellFolder*>(this), cidl, apidl, NULL, 0, NULL };
		hr = SHCreateDefaultContextMenu(&dcm, IID_IContextMenu, ppv);

		//hr = this->QueryInterface(riid,ppv);
	}
	else if ( IsEqualIID(IID_IExtractIcon,riid) || IsEqualIID(IID_IExtractImage,riid) )
	{
		hr = this->QueryInterface(riid,ppv);
	}
	else if ( IsEqualIID(IID_IQueryAssociations,riid) )	// {C46CA590-3C3F-11D2-BEE6-0000F805CA57}
	{
		// not implement, see: AssocCreateForClasses
		wchar_t tmp[MAX_PATH];
		LPWSTR bstrCLSID = tmp;
		::m2w(CLSID_CShellFolderImpl,bstrCLSID);
		ASSOCIATIONELEMENT rgAssoc[] = { { ASSOCCLASS_CLSID_STR, NULL, bstrCLSID }, { ASSOCCLASS_FOLDER, NULL, NULL } };
		hr = ::AssocCreateForClasses(rgAssoc, 2U, riid, ppv);

	}
	else if ( IsEqualIID(IID_IDataObject,riid) )	// {0000010E-0000-0000-C000-000000000046}
	{
		hr = SHCreateDataObject(m_pIDFolder, cidl, apidl, NULL, riid, ppv);
	}
	else if ( IsEqualIID(IID_IQueryInfo, riid ) )
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
		ULONG n = wcsnlen_s(pszDisplayName,MAX_PATH);
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
	HRESULT	hr = S_FALSE;
	auto data = m_PidlMgr.GetData(pidl);
	if( NULL != data )
	{
		LPCWSTR oldTag = data->wszDisplayName;
		BOOL x = this->pTagHelper->RenameTag(oldTag,pszName);
		if( x )
		{
			MYPIDLDATA d = {sizeof(MYPIDLDATA)};
			d.cb = sizeof(MYPIDLDATA);
			d.Type = MYSHITEMTYPE_TAG;
			StringCbCopyW(d.wszDisplayName,sizeof(d.wszDisplayName), pszName);
			*ppidlOut = m_PidlMgr.Create(&d);

			UpdateWindow(hwndOwner);

			hr = S_OK;
		}
		else
		{
			MessageBox(NULL,::MyLoadString(IDS_MSG_TAG_UNAVAILABLE),L"Error",MB_OK | MB_ICONERROR);
		}
	}
#if _DEBUG
	else
	{
		MessageBox(NULL,L"Selected item can not be renamed.",L"Error",MB_OK | MB_ICONERROR);
	}
#endif

	return hr;	
};
#pragma endregion


#pragma region IShellFolder2
STDMETHODIMP CShellFolderImpl::GetDefaultSearchGUID(__out GUID *pguid)
{
	return E_NOTIMPL;
}

STDMETHODIMP CShellFolderImpl::EnumSearches(__deref_out IEnumExtraSearch **ppenum)
{
	return E_NOTIMPL;
}

STDMETHODIMP CShellFolderImpl::GetDefaultColumn(DWORD dwRes, __out ULONG *plSort, __out ULONG *plDisplay)
{
	*plSort = 0;
	*plDisplay = 0;
	return S_OK;
}

STDMETHODIMP CShellFolderImpl::GetDefaultColumnState(UINT iColumn, __out SHCOLSTATEF *pcsFlags)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

#pragma region value of columns
// to display details for TAGs. some of them will be listed as column in detail view.
const CShellFolderImpl::COLUMNINFO CShellFolderImpl::c_rgColumnInfo[] =
{
	{ PKEY_ItemNameDisplay, SHCOLSTATE_ONBYDEFAULT, &CShellFolderImpl::_GetDisplayName },
	{ PKEY_ItemTypeText, SHCOLSTATE_VIEWONLY, &CShellFolderImpl::_GetContainedItems },		// for USECOUNT in TILE view.
	{ PKEY_Comment, SHCOLSTATE_VIEWONLY, &CShellFolderImpl::_GetContainedItems },			// for USECOUNT in DETAIL view.
};

// Gets detailed information, identified by a property set identifier (FMTID) and a property identifier (PID), on an item in a Shell folder.
STDMETHODIMP CShellFolderImpl::GetDetailsEx(PCUITEMID_CHILD pidl, const PROPERTYKEY *pkey, __out VARIANT *pvar)
{
	PROPVARIANT spropvar;
	HRESULT hr = _GetPropertyForItem(pidl, *pkey, &spropvar);
	if (SUCCEEDED(hr))
	{
		hr = PropVariantToVariant(&spropvar, pvar);
	}
	return hr;
}

// the display name here will not used to be shown on the folder as item name, but will be used when sorting by column.
HRESULT CShellFolderImpl::_GetDisplayName(PCUITEMID_CHILD pidl,REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)
{
	HRESULT hr = E_NOTIMPL;
	auto tmp = ILClone(pidl);
	if( tmp ) 
	{
		auto data = m_PidlMgr.GetData(tmp);
		if( data != NULL && data->Type == MYSHITEMTYPE_TAG )
		{
			ppropvar->vt = VT_LPWSTR;
			ppropvar->pwszVal = new wchar_t[MAXLENGTH_EACHTAG];
			StrCpy(ppropvar->pwszVal,data->wszDisplayName);
			hr = S_OK;
		}
		ILFree(tmp);
	}
	return hr;
}

HRESULT CShellFolderImpl::_GetContainedItems(PCUITEMID_CHILD pidl,REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)
{
	HRESULT hr = E_NOTIMPL;
	auto tmp = ILClone(pidl);
	if( tmp ) 
	{
		auto data = m_PidlMgr.GetData(tmp);
		if( data != NULL && data->Type == MYSHITEMTYPE_TAG )
		{
			ppropvar->vt = VT_LPWSTR;
			ppropvar->pwszVal = new wchar_t[20];	// 20 for ¡°%d Items¡±
			wsprintf(ppropvar->pwszVal,::MyLoadString(IDS_MSG_N_FILES_LOADED_FOR_TAG_SHORT),data->UseCount);
			//_itow(data->UseCount,ppropvar->pwszVal,10); //PKEY_Comment
			hr = S_OK;
		}
		ILFree(tmp);
	}
	return hr;
}

HRESULT CShellFolderImpl::_GetPropertyForItem(PCUITEMID_CHILD pidl, REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)
{
	HRESULT hr = E_FAIL;
	for (int i = 0; i < ARRAYSIZE(c_rgColumnInfo); i++)
	{
		if (IsEqualPropertyKey(key, c_rgColumnInfo[i].key))
		{
			hr = (this->*c_rgColumnInfo[i].pfnGetProperty)(pidl,key, ppropvar);
			break;
		}
	}
	return hr;
}
#pragma endregion

// The IShellFolder2::GetDetailsOf method provides access to the information that is displayed in the Windows Explorer Details view of a Shell folder.
STDMETHODIMP CShellFolderImpl::GetDetailsOf(__in_opt PCUITEMID_CHILD pidl, UINT iColumn, __out SHELLDETAILS *psd)
{
	HRESULT hr = E_INVALIDARG;
	if (iColumn < ARRAYSIZE(c_rgColumnInfo))
	{
		if (pidl)
		{
			PROPVARIANT spropvar;
			hr = _GetPropertyForItem(pidl, c_rgColumnInfo[iColumn].key, &spropvar);
			if (SUCCEEDED(hr))
			{
				psd->str.uType = STRRET_WSTR;
				psd->str.pOleStr = spropvar.pwszVal;
				spropvar.vt = VT_EMPTY;
			}
		}
		else
		{
			// see ..\_Reference\RegNamespace
			/*
			IPropertyDescription* sppropdesc;
			hr = PSGetPropertyDescription(c_rgColumnInfo[iColumn].key, IID_PPV_ARGS(&sppropdesc));
			if (SUCCEEDED(hr))
			{
			int fmt = LVCFMT_LEFT;          // default
			PROPDESC_VIEW_FLAGS dvf;
			HRESULT hr = sppropdesc->GetViewFlags(&dvf);
			if (SUCCEEDED(hr))
			{
			// Handle the mutually exclusive part
			if (dvf & PDVF_RIGHTALIGN)
			{
			fmt = LVCFMT_RIGHT;
			}
			else if (dvf & PDVF_CENTERALIGN)
			{
			fmt = LVCFMT_CENTER;
			}
			}

			psd->fmt = fmt;

			hr = sppropdesc->GetDefaultColumnWidth((UINT *)&psd->cxChar);
			if (SUCCEEDED(hr))
			{
			PROPDESC_TYPE_FLAGS dtf;
			hr = sppropdesc->GetTypeFlags(PDTF_ISVIEWABLE, &dtf);
			if (SUCCEEDED(hr))
			{
			WCHAR spszDisplayName[MAX_PATH] = {0};
			LPWSTR	pName = spszDisplayName;
			hr = sppropdesc->GetDisplayName(&pName);
			if (FAILED(hr) && !(dtf & PDTF_ISVIEWABLE))
			{
			// Hidden columns don't have to have a display name in the schema
			hr = SHStrDup(L"", &pName);
			}

			if (SUCCEEDED(hr))
			{
			psd->str.uType = STRRET_WSTR;
			psd->str.pOleStr = spszDisplayName;
			}
			}
			}

			}
			*/
		}
	}
	return hr;
}

STDMETHODIMP CShellFolderImpl::MapColumnToSCID(UINT iColumn, __out PROPERTYKEY *pkey)
{
	HRESULT hr = E_INVALIDARG;
	if (iColumn < ARRAYSIZE(c_rgColumnInfo))
	{
		*pkey = c_rgColumnInfo[iColumn].key;
		hr = S_OK;
	}
	return hr;
}
#pragma endregion

//// IPropertyStore
//HRESULT CShellFolderImpl::GetCount(DWORD *cProps){
//	*cProps = 3;
//	return S_OK;
//}
//HRESULT CShellFolderImpl::GetAt(DWORD iProp,PROPERTYKEY *pkey){
//	if (iProp < ARRAYSIZE(c_rgColumnInfo))
//	{
//		*pkey = c_rgColumnInfo[iProp].key;
//		return S_OK;
//	}
//	return S_FALSE;
//}	
//HRESULT CShellFolderImpl::GetValue(REFPROPERTYKEY key,PROPVARIANT *pv){
//	pv->vt = VT_EMPTY;
//	return S_OK;
//}
