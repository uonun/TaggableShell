#pragma once
#include "..\include\NsExt.CShellViewImpl.h"
/*
 Most of these functions are redirected to _peb, in other words, why not just return IFolderView/IFolderView2 from _peb directly like this:

 See: CShellViewImpl::QueryService
		if( IsEqualIID(riid, IID_IFolderView ) || IsEqualIID(riid, IID_IFolderView2 ) )
		{
			hr = _peb->QueryInterface(riid,ppv);
		}

 The reason is, there still are some work to do in the other of functions which is not redirected to _peb.
 so we have to implement IFolderView/IFolderView2 by ourselves.
*/

// IFolderView
HRESULT CShellViewImpl::GetCurrentViewMode(UINT *pViewMode)
{ 
	*pViewMode = m_folderSettings.ViewMode;
	return S_OK;
}

HRESULT CShellViewImpl::SetCurrentViewMode(UINT ViewMode)
{
	m_folderSettings.ViewMode = ViewMode;
	HRESULT hr = _peb->SetFolderSettings(&m_folderSettings);
	return hr;
}

HRESULT CShellViewImpl::GetFolder( 
	REFIID riid,
	void **ppv)
{
	// Check parameters.
	if( !ppv )
		return E_INVALIDARG;

	// Reset out parameters.
	*ppv = NULL;

	// Query for the requested itf.
	HRESULT hr = m_psfContainingFolder->QueryInterface( riid, ppv );
	return hr;
}

HRESULT CShellViewImpl::Item( 
	int iItemIndex,
	PITEMID_CHILD *ppidl)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->Item(iItemIndex,ppidl);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::ItemCount( 
	UINT uFlags,
	int *pcItems)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->ItemCount(uFlags,pcItems);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::Items( 
	UINT uFlags,
	REFIID riid,
	void **ppv)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->Items(uFlags,riid,ppv);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::GetSelectionMarkedItem( 
	int *piItem)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->GetSelectionMarkedItem(piItem);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::GetFocusedItem( 
	int *piItem)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->GetFocusedItem(piItem);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::GetItemPosition( 
	PCUITEMID_CHILD pidl,
	POINT *ppt)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->GetItemPosition(pidl,ppt);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::GetSpacing( 
	POINT *ppt)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->GetSpacing(ppt);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::GetDefaultSpacing( 
	POINT *ppt)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->GetDefaultSpacing(ppt);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::GetAutoArrange( void)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->GetAutoArrange();
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::SelectItem( 
	int iItem,
	DWORD dwFlags)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->SelectItem(iItem,dwFlags);
		pfv2->Release();
	}
	return hr;
}

HRESULT CShellViewImpl::SelectAndPositionItems( 
	UINT cidl,
	PCUITEMID_CHILD_ARRAY apidl,
	POINT *apt,
	DWORD dwFlags)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->SelectAndPositionItems(cidl,apidl,apt,dwFlags);
		pfv2->Release();
	}
	return hr;
}


// IFolderView2
HRESULT CShellViewImpl::SetGroupBy(REFPROPERTYKEY key,BOOL fAscending)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetGroupBy( PROPERTYKEY *pkey,BOOL *pfAscending)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::SetText( FVTEXTTYPE iType,LPCWSTR pwszText)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::SetCurrentFolderFlags(DWORD dwMask,DWORD dwFlags)
{
	m_folderSettings.fFlags = dwMask & dwFlags;
	HRESULT hr = _peb->SetFolderSettings(&m_folderSettings);
	return hr;
}
HRESULT CShellViewImpl::GetCurrentFolderFlags(DWORD *pdwFlags)
{
	*pdwFlags = m_folderSettings.fFlags;
	return S_OK;
}
HRESULT CShellViewImpl::GetSortColumnCount(int *pcColumns)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::SetSortColumns(const SORTCOLUMN *rgSortColumns,int cColumns)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetSortColumns(SORTCOLUMN *rgSortColumns,int cColumns)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetItem(int iItem,REFIID riid,void **ppv)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetVisibleItem(int iStart,BOOL fPrevious,int *piItem)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetSelectedItem(int iStart,int *piItem)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetSelection(BOOL fNoneImpliesFolder,IShellItemArray **ppsia)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetSelectionState(PCUITEMID_CHILD pidl,DWORD *pdwFlags)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::InvokeVerbOnSelection(LPCSTR pszVerb)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::SetViewModeAndIconSize(FOLDERVIEWMODE uViewMode,int iImageSize)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->SetViewModeAndIconSize(uViewMode,iImageSize);
		pfv2->Release();
	}
	return hr;
}
HRESULT CShellViewImpl::GetViewModeAndIconSize(FOLDERVIEWMODE *puViewMode,int *piImageSize)
{
	HRESULT hr;
	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if( SUCCEEDED(hr) )
	{
		hr = pfv2->GetViewModeAndIconSize(puViewMode,piImageSize);
		pfv2->Release();
	}
	return hr;
}
HRESULT CShellViewImpl::SetGroupSubsetCount(UINT cVisibleRows)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::GetGroupSubsetCount(UINT *pcVisibleRows)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::SetRedraw(BOOL fRedrawOn)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::IsMoveInSameFolder( void)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
HRESULT CShellViewImpl::DoRename( void)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
