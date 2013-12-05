#pragma once
#include "..\include\NsExt.CShellViewImpl.h"


// ICommDlgBrowser
STDMETHODIMP CShellViewImpl::OnDefaultCommand(IShellView * psv)
{
	::PrintLog(L"CShellViewImpl::OnDefaultCommand");

	HRESULT hr;

	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if (SUCCEEDED(hr))
	{
		IShellItem *psi;
		hr = GetItemFromView(pfv2, -1, IID_PPV_ARGS(&psi));
		if (SUCCEEDED(hr))
		{
			PIDLIST_ABSOLUTE pidl;
			hr = SHGetIDListFromObject(psi, &pidl);
			if (SUCCEEDED(hr))
			{
				LPITEMIDLIST last;
				last = m_PidlMgr.GetLastItem(pidl);
				if( hr == S_OK )
				{
					auto data = m_PidlMgr.GetData(last);
					if ( data != NULL && (data->Type == MYSHITEMTYPE_TAG || data->Type == MYSHITEMTYPE_FILE ) )
					{
						::PrintLog(L"CShellViewImpl::OnDefaultCommand: %s",data->wszDisplayName);
						hr = m_spShellBrowser->BrowseObject(last, SBSP_DEFBROWSER | SBSP_RELATIVE);
						m_spShellBrowser->OnViewWindowActive(psv);
					}
					else
					{
						SHELLEXECUTEINFO ei = { sizeof(ei) };
						ei.fMask        = SEE_MASK_INVOKEIDLIST;
						ei.hwnd         = m_hwndParent;
						ei.nShow        = SW_NORMAL;
						ei.lpIDList     = pidl;

						hr = ShellExecuteEx(&ei);
					}
				}
				CoTaskMemFree(pidl);
			}
			psi->Release();
		}
		pfv2->Release();
	}

	return hr;
}

STDMETHODIMP CShellViewImpl::OnStateChange(IShellView * psv, ULONG uChange)
{
	::PrintLog(L"CShellViewImpl::OnStateChange, uChange = %d",uChange);
	if (uChange == CDBOSC_SELCHANGE)
	{
		//PostMessage(m_hWnd, KFD_SELCHANGE, 0, 0);
	}
	return S_OK;
}

// When dealing with data sources that have many items, such as libraries and searches, 
// the callback to this method results in poor performance. To avoid that situation, 
// implement GetViewFlags and return CDB2GVF_NOINCLUDEITEM. Doing so enables the view 
// to skip calling ICommDlgBrowser::IncludeObject, thereby improving performance.
STDMETHODIMP CShellViewImpl::IncludeObject(IShellView * psv, PCUITEMID_CHILD pidl)
{
	::PrintLog(L"CShellViewImpl::IncludeObject");
	return S_OK;
}


// ICommDlgBrowser2
HRESULT CShellViewImpl::Notify(IShellView *ppshv,DWORD dwNotifyType)
{
	HRESULT hr = E_NOTIMPL;

	switch (dwNotifyType)
	{
	case CDB2N_CONTEXTMENU_START:
		{
			// not finished.
			// GetContextMenuForItems(0,NULL);
			hr = S_OK;
		}
		break;
	case CDB2N_CONTEXTMENU_DONE:
		if( NULL != m_hMenu )
		{
			DestroyMenu(m_hMenu);
			m_hMenu = NULL;
			hr = S_OK;
		}
		break;
	default:
		break;
	}
	return hr;
}

HRESULT CShellViewImpl::GetDefaultMenuText(IShellView *ppshv,LPWSTR pszText,int cchMax)
{
	return E_NOTIMPL;
}
HRESULT CShellViewImpl::GetViewFlags(DWORD *pdwFlags)
{
	*pdwFlags = CDB2GVF_SHOWALLFILES | CDB2GVF_NOSELECTVERB | CDB2GVF_NOINCLUDEITEM;
	return S_OK;
}

// ICommDlgBrowser3
HRESULT CShellViewImpl::OnColumnClicked(IShellView *ppshv,int iColumn)
{
	return E_NOTIMPL;
}
HRESULT CShellViewImpl::GetCurrentFilter(LPWSTR pszFileSpec,int cchFileSpec)
{
	return E_NOTIMPL;
}
HRESULT CShellViewImpl::OnPreViewCreated(IShellView *ppshv)
{
	return E_NOTIMPL;
}

HMENU CShellViewImpl::GetContextMenuForItems(UINT uItems,LPITEMIDLIST *aItems)
{
	IContextMenu* pContextMenu = NULL;

	m_psfContainingFolder->GetUIObjectOf(m_hwndParent,
		uItems,
		(LPCITEMIDLIST*)aItems,
		IID_IContextMenu,
		NULL,
		(LPVOID*)&pContextMenu);

	if(pContextMenu)
	{
		m_hMenu = CreatePopupMenu();
		// MF_BYCOMMAND  MF_BYPOSITION
		AppendMenu ( m_hMenu, MF_BYPOSITION, CMD_NEWTAG , ::MyLoadString(IDS_CONTEXTMENU_SUB_NEWTAG));
		AppendMenu ( m_hMenu, MF_BYPOSITION, CMD_SETTINGS , ::MyLoadString(IDS_CONTEXTMENU_SUB_SETTINGS));
		AppendMenu ( m_hMenu, MF_BYPOSITION, CMD_ABOUT , ::MyLoadString(IDS_CONTEXTMENU_SUB_ABOUT));

		/*
		See if we are in Explore or Open mode. If the browser's tree is
		present, then we are in Explore mode.
		*/ 
		BOOL  fExplore = FALSE;
		HWND  hwndTree = NULL;
		if(SUCCEEDED(m_spShellBrowser->GetControlWindow(FCW_TREE,
			&hwndTree)) && hwndTree)
		{
			fExplore = TRUE;
		}

		UINT uIDAdjust = GetMenuItemCount(m_hMenu);
		UINT uIDAdjustMax = 0xFFFF; //To allow all IDs, set this parameter to 0xFFFF.
		HRESULT hr = pContextMenu->QueryContextMenu( m_hMenu,
			0,
			uIDAdjust,
			uIDAdjustMax,
			CMF_NORMAL | CMF_EXTENDEDVERBS | (fExplore ? CMF_EXPLORE : 0));
		if(m_hMenu && SUCCEEDED(hr))
		{
			POINT p;
			GetCursorPos(&p);
			TrackPopupMenu(m_hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,p.x,p.y,0,m_hWnd,NULL);
		}
		else
		{
			DestroyMenu(m_hMenu);
			m_hMenu = NULL;
		}
	}
	return m_hMenu;
}