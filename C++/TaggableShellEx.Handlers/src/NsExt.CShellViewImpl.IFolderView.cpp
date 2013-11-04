#pragma once
#include "..\include\NsExt.CShellViewImpl.h"

HRESULT CShellViewImpl::GetCurrentViewMode( 
	UINT *pViewMode)
{ 
	*pViewMode = m_FolderSettings.ViewMode;
	return S_OK; }

HRESULT CShellViewImpl::SetCurrentViewMode( 
	UINT ViewMode)
{
	m_FolderSettings.ViewMode = ViewMode;
	_peb->SetFolderSettings(&m_FolderSettings);
	return S_OK;
}

HRESULT CShellViewImpl::GetFolder( 
	REFIID riid,
	void **ppv)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::Item( 
	int iItemIndex,
	PITEMID_CHILD *ppidl)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::ItemCount( 
	UINT uFlags,
	int *pcItems)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::Items( 
	UINT uFlags,
	REFIID riid,
	void **ppv)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::GetSelectionMarkedItem( 
	int *piItem)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::GetFocusedItem( 
	int *piItem)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::GetItemPosition( 
	PCUITEMID_CHILD pidl,
	POINT *ppt)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::GetSpacing( 
	POINT *ppt)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::GetDefaultSpacing( 
	POINT *ppt)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::GetAutoArrange( void)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::SelectItem( 
	int iItem,
	DWORD dwFlags)
{ return E_NOTIMPL; }

HRESULT CShellViewImpl::SelectAndPositionItems( 
	UINT cidl,
	PCUITEMID_CHILD_ARRAY apidl,
	POINT *apt,
	DWORD dwFlags)
{ return E_NOTIMPL; }